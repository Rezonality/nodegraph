#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include <nodegraph/fonts.h>
#define FONTSTASH_IMPLEMENTATION
#include <nodegraph/fontstash.h>

namespace NodeGraph 
{

namespace 
{
// Texture handling bits using the imgui API
static int fonts_imgui_update_texture(void* uptr, int image, int x, int y, int w, int h, const unsigned char* data)
{
}

static int fonts_imgui_create_texture(void* uptr, int w, int h, const unsigned char* data)
{
}

static void fonts_imgui_image_size(FontContext& ctx, int image, int* w, int* h)
{
}
}

void fonts_init(FontContext& ctx)
{
    FONSparams fontParams;
    for (uint32_t i = 0; i < NVG_MAX_FONTIMAGES; i++)
    {
        ctx.fontImages[i] = 0;
    }

    // Init font rendering
    memset(&fontParams, 0, sizeof(fontParams));
    fontParams.width = NVG_INIT_FONTIMAGE_SIZE;
    fontParams.height = NVG_INIT_FONTIMAGE_SIZE;
    fontParams.flags = FONS_ZERO_TOPLEFT;
    fontParams.renderCreate = NULL;
    fontParams.renderUpdate = NULL;
    fontParams.renderDraw = NULL;
    fontParams.renderDelete = NULL;
    fontParams.userPtr = NULL;
    ctx.fs = fonsCreateInternal(&fontParams);
}

void fonts_destroy(FontContext& ctx)
{
    if (ctx.fs)
    {
        fonsDeleteInternal(ctx.fs);
    }
    for (uint32_t i = 0; i < NVG_MAX_FONTIMAGES; i++)
    {
        if (ctx.fontImages[i] != 0)
        {
            // nvgDeleteImage(ctx, ctx.fontImages[i]);
            ctx.fontImages[i] = 0;
        }
    }
}

// Add fonts
int fonts_create(FontContext& ctx, const char* name, const char* filename)
{
    return fonsAddFont(ctx.fs, name, filename, 0);
}

int fonts_create(FontContext& ctx, const char* name, const char* filename, const int fontIndex)
{
    return fonsAddFont(ctx.fs, name, filename, fontIndex);
}

int fonts_create_mem(FontContext& ctx, const char* name, unsigned char* data, int ndata, int freeData)
{
    return fonsAddFontMem(ctx.fs, name, data, ndata, freeData, 0);
}

int fonts_create_mem(FontContext& ctx, const char* name, unsigned char* data, int ndata, int freeData, const int fontIndex)
{
    return fonsAddFontMem(ctx.fs, name, data, ndata, freeData, fontIndex);
}

int fonts_find(FontContext& ctx, const char* name)
{
    if (name == NULL)
        return -1;
    return fonsGetFontByName(ctx.fs, name);
}

int fonts_add_fallback(FontContext& ctx, int baseFont, int fallbackFont)
{
    if (baseFont == -1 || fallbackFont == -1)
        return 0;
    return fonsAddFallbackFont(ctx.fs, baseFont, fallbackFont);
}

int fonts_add_fallback(FontContext& ctx, const char* baseFont, const char* fallbackFont)
{
    return fonts_add_fallback(ctx, fonts_find(ctx, baseFont), fonts_find(ctx, fallbackFont));
}

void fonts_reset_fallback(FontContext& ctx, int baseFont)
{
    fonsResetFallbackFont(ctx.fs, baseFont);
}

void fonts_reset_fallback(FontContext& ctx, const char* baseFont)
{
    fonts_reset_fallback(ctx, fonts_find(ctx, baseFont));
}

void nvgFontFace(FontContext& ctx, const char* font)
{
    ctx.fontId = fonsGetFontByName(ctx.fs, font);
}

static float quantize(float a, float d)
{
    return ((int)(a / d + 0.5f)) * d;
}

static float get_average_scale(float *t)
{
	float sx = sqrtf(t[0]*t[0] + t[2]*t[2]);
	float sy = sqrtf(t[1]*t[1] + t[3]*t[3]);
	return (sx + sy) * 0.5f;
}

static float get_font_scale(FontContext& ctx)
{
    return std::min(quantize(get_average_scale(ctx.xform), 0.01f), 4.0f);
}

static void fonts_flush_texture(FontContext& ctx)
{
    int dirty[4];

    if (fonsValidateTexture(ctx.fs, dirty))
    {
        int fontImage = ctx.fontImages[ctx.fontImageIdx];
        // Update texture
        if (fontImage != 0)
        {
            int iw, ih;
            const unsigned char* data = fonsGetTextureData(ctx.fs, &iw, &ih);
            int x = dirty[0];
            int y = dirty[1];
            int w = dirty[2] - dirty[0];
            int h = dirty[3] - dirty[1];
            fonts_imgui_update_texture(ctx.userPtr, fontImage, x, y, w, h, data);
        }
    }
}

static int fonts_alloc_atlas(FontContext& ctx)
{
    int iw, ih;
    fonts_flush_texture(ctx);
    if (ctx.fontImageIdx >= NVG_MAX_FONTIMAGES - 1)
    {
        return 0;
    }
    // if next fontImage already have a texture
    if (ctx.fontImages[ctx.fontImageIdx + 1] != 0)
    {
        fonts_imgui_image_size(ctx, ctx.fontImages[ctx.fontImageIdx + 1], &iw, &ih);
    }
    else
    { // calculate the new font image size and create it.
        fonts_imgui_image_size(ctx, ctx.fontImages[ctx.fontImageIdx], &iw, &ih);
        if (iw > ih)
        {
            ih *= 2;
        }
        else
        {
            iw *= 2;
        }
        if (iw > NVG_MAX_FONTIMAGE_SIZE || ih > NVG_MAX_FONTIMAGE_SIZE)
        {
            iw = ih = NVG_MAX_FONTIMAGE_SIZE;
        }
        ctx.fontImages[ctx.fontImageIdx + 1] = fonts_imgui_create_texture(ctx.userPtr, iw, ih, nullptr);
    }
    ++ctx.fontImageIdx;
    fonsResetAtlas(ctx.fs, iw, ih);
    return 1;
}

/*
static void nvg__renderText(FontContext& ctx, NVGvertex* verts, int nverts)
{
    NVGpaint paint = ctx.fill;

    // Render triangles.
    paint.image = ctx.fontImages[ctx.fontImageIdx];

    // Apply global alpha
    paint.innerColor.a *= ctx.alpha;
    paint.outerColor.a *= ctx.alpha;

    ctx.params.renderTriangles(ctx.params.userPtr, &paint, ctx.compositeOperation, &ctx.scissor, verts, nverts, ctx.fringeWidth);

    ctx.drawCallCount++;
    ctx.textTriCount += nverts / 3;
}
*/

static int is_transform_flipped(const float* xform)
{
    float det = xform[0] * xform[3] - xform[2] * xform[1];
    return (det < 0);
}
/*

float nvgText(FontContext& ctx, float x, float y, const char* string, const char* end)
{
    FONStextIter iter, prevIter;
    FONSquad q;
    NVGvertex* verts;
    float scale = nvg__getFontScale(state) * ctx.devicePxRatio;
    float invscale = 1.0f / scale;
    int cverts = 0;
    int nverts = 0;
    int isFlipped = nvg__isTransformFlipped(ctx.xform);

    if (end == NULL)
        end = string + strlen(string);

    if (ctx.fontId == FONS_INVALID)
        return x;

    fonsSetSize(ctx.fs, ctx.fontSize * scale);
    fonsSetSpacing(ctx.fs, ctx.letterSpacing * scale);
    fonsSetBlur(ctx.fs, ctx.fontBlur * scale);
    fonsSetAlign(ctx.fs, ctx.textAlign);
    fonsSetFont(ctx.fs, ctx.fontId);

    cverts = nvg__maxi(2, (int)(end - string)) * 6; // conservative estimate.
    verts = nvg__allocTempVerts(ctx, cverts);
    if (verts == NULL)
        return x;

    fonsTextIterInit(ctx.fs, &iter, x * scale, y * scale, string, end, FONS_GLYPH_BITMAP_REQUIRED);
    prevIter = iter;
    while (fonsTextIterNext(ctx.fs, &iter, &q))
    {
        float c[4 * 2];
        if (iter.prevGlyphIndex == -1)
        { // can not retrieve glyph?
            if (nverts != 0)
            {
                nvg__renderText(ctx, verts, nverts);
                nverts = 0;
            }
            if (!nvg__allocTextAtlas(ctx))
                break; // no memory :(
            iter = prevIter;
            fonsTextIterNext(ctx.fs, &iter, &q); // try again
            if (iter.prevGlyphIndex == -1) // still can not find glyph?
                break;
        }
        prevIter = iter;
        if (isFlipped)
        {
            float tmp;

            tmp = q.y0;
            q.y0 = q.y1;
            q.y1 = tmp;
            tmp = q.t0;
            q.t0 = q.t1;
            q.t1 = tmp;
        }
        // Transform corners.
        nvgTransformPoint(&c[0], &c[1], ctx.xform, q.x0 * invscale, q.y0 * invscale);
        nvgTransformPoint(&c[2], &c[3], ctx.xform, q.x1 * invscale, q.y0 * invscale);
        nvgTransformPoint(&c[4], &c[5], ctx.xform, q.x1 * invscale, q.y1 * invscale);
        nvgTransformPoint(&c[6], &c[7], ctx.xform, q.x0 * invscale, q.y1 * invscale);
        // Create triangles
        if (nverts + 6 <= cverts)
        {
            nvg__vset(&verts[nverts], c[0], c[1], q.s0, q.t0);
            nverts++;
            nvg__vset(&verts[nverts], c[4], c[5], q.s1, q.t1);
            nverts++;
            nvg__vset(&verts[nverts], c[2], c[3], q.s1, q.t0);
            nverts++;
            nvg__vset(&verts[nverts], c[0], c[1], q.s0, q.t0);
            nverts++;
            nvg__vset(&verts[nverts], c[6], c[7], q.s0, q.t1);
            nverts++;
            nvg__vset(&verts[nverts], c[4], c[5], q.s1, q.t1);
            nverts++;
        }
    }

    // TODO: add back-end bit to do this just once per frame.
    nvg__flushTextTexture(ctx);

    nvg__renderText(ctx, verts, nverts);

    return iter.nextx / scale;
}

void nvgTextBox(FontContext& ctx, float x, float y, float breakRowWidth, const char* string, const char* end)
{
    NVGtextRow rows[2];
    int nrows = 0, i;
    int oldAlign = ctx.textAlign;
    int haling = ctx.textAlign & (NVG_ALIGN_LEFT | NVG_ALIGN_CENTER | NVG_ALIGN_RIGHT);
    int valign = ctx.textAlign & (NVG_ALIGN_TOP | NVG_ALIGN_MIDDLE | NVG_ALIGN_BOTTOM | NVG_ALIGN_BASELINE);
    float lineh = 0;

    if (ctx.fontId == FONS_INVALID)
        return;

    nvgTextMetrics(ctx, NULL, NULL, &lineh);

    ctx.textAlign = NVG_ALIGN_LEFT | valign;

    while ((nrows = nvgTextBreakLines(ctx, string, end, breakRowWidth, rows, 2)))
    {
        for (i = 0; i < nrows; i++)
        {
            NVGtextRow* row = &rows[i];
            if (haling & NVG_ALIGN_LEFT)
                nvgText(ctx, x, y, row->start, row->end);
            else if (haling & NVG_ALIGN_CENTER)
                nvgText(ctx, x + breakRowWidth * 0.5f - row->width * 0.5f, y, row->start, row->end);
            else if (haling & NVG_ALIGN_RIGHT)
                nvgText(ctx, x + breakRowWidth - row->width, y, row->start, row->end);
            y += lineh * ctx.lineHeight;
        }
        string = rows[nrows - 1].next;
    }

    ctx.textAlign = oldAlign;
}

int nvgTextGlyphPositions(FontContext& ctx, float x, float y, const char* string, const char* end, NVGglyphPosition* positions, int maxPositions)
{
    float scale = nvg__getFontScale(state) * ctx.devicePxRatio;
    float invscale = 1.0f / scale;
    FONStextIter iter, prevIter;
    FONSquad q;
    int npos = 0;

    if (ctx.fontId == FONS_INVALID)
        return 0;

    if (end == NULL)
        end = string + strlen(string);

    if (string == end)
        return 0;

    fonsSetSize(ctx.fs, ctx.fontSize * scale);
    fonsSetSpacing(ctx.fs, ctx.letterSpacing * scale);
    fonsSetBlur(ctx.fs, ctx.fontBlur * scale);
    fonsSetAlign(ctx.fs, ctx.textAlign);
    fonsSetFont(ctx.fs, ctx.fontId);

    fonsTextIterInit(ctx.fs, &iter, x * scale, y * scale, string, end, FONS_GLYPH_BITMAP_OPTIONAL);
    prevIter = iter;
    while (fonsTextIterNext(ctx.fs, &iter, &q))
    {
        if (iter.prevGlyphIndex < 0 && nvg__allocTextAtlas(ctx))
        { // can not retrieve glyph?
            iter = prevIter;
            fonsTextIterNext(ctx.fs, &iter, &q); // try again
        }
        prevIter = iter;
        positions[npos].str = iter.str;
        positions[npos].x = iter.x * invscale;
        positions[npos].minx = nvg__minf(iter.x, q.x0) * invscale;
        positions[npos].maxx = nvg__maxf(iter.nextx, q.x1) * invscale;
        npos++;
        if (npos >= maxPositions)
            break;
    }

    return npos;
}
*/

enum NVGcodepointType
{
    NVG_SPACE,
    NVG_NEWLINE,
    NVG_CHAR,
    NVG_CJK_CHAR,
};

/*
int nvgTextBreakLines(FontContext& ctx, const char* string, const char* end, float breakRowWidth, NVGtextRow* rows, int maxRows)
{
    float scale = get_font_scale(ctx) * ctx.devicePxRatio;
    float invscale = 1.0f / scale;
    FONStextIter iter, prevIter;
    FONSquad q;
    int nrows = 0;
    float rowStartX = 0;
    float rowWidth = 0;
    float rowMinX = 0;
    float rowMaxX = 0;
    const char* rowStart = NULL;
    const char* rowEnd = NULL;
    const char* wordStart = NULL;
    float wordStartX = 0;
    float wordMinX = 0;
    const char* breakEnd = NULL;
    float breakWidth = 0;
    float breakMaxX = 0;
    int type = NVG_SPACE, ptype = NVG_SPACE;
    unsigned int pcodepoint = 0;

    if (maxRows == 0)
        return 0;
    if (ctx.fontId == FONS_INVALID)
        return 0;

    if (end == NULL)
        end = string + strlen(string);

    if (string == end)
        return 0;

    fonsSetSize(ctx.fs, ctx.fontSize * scale);
    fonsSetSpacing(ctx.fs, ctx.letterSpacing * scale);
    fonsSetBlur(ctx.fs, ctx.fontBlur * scale);
    fonsSetAlign(ctx.fs, ctx.textAlign);
    fonsSetFont(ctx.fs, ctx.fontId);

    breakRowWidth *= scale;

    fonsTextIterInit(ctx.fs, &iter, 0, 0, string, end, FONS_GLYPH_BITMAP_OPTIONAL);
    prevIter = iter;
    while (fonsTextIterNext(ctx.fs, &iter, &q))
    {
        if (iter.prevGlyphIndex < 0 && nvg__allocTextAtlas(ctx))
        { // can not retrieve glyph?
            iter = prevIter;
            fonsTextIterNext(ctx.fs, &iter, &q); // try again
        }
        prevIter = iter;
        switch (iter.codepoint)
        {
        case 9: // \t
        case 11: // \v
        case 12: // \f
        case 32: // space
        case 0x00a0: // NBSP
            type = NVG_SPACE;
            break;
        case 10: // \n
            type = pcodepoint == 13 ? NVG_SPACE : NVG_NEWLINE;
            break;
        case 13: // \r
            type = pcodepoint == 10 ? NVG_SPACE : NVG_NEWLINE;
            break;
        case 0x0085: // NEL
            type = NVG_NEWLINE;
            break;
        default:
            if ((iter.codepoint >= 0x4E00 && iter.codepoint <= 0x9FFF) || (iter.codepoint >= 0x3000 && iter.codepoint <= 0x30FF) || (iter.codepoint >= 0xFF00 && iter.codepoint <= 0xFFEF) || (iter.codepoint >= 0x1100 && iter.codepoint <= 0x11FF) || (iter.codepoint >= 0x3130 && iter.codepoint <= 0x318F) || (iter.codepoint >= 0xAC00 && iter.codepoint <= 0xD7AF))
                type = NVG_CJK_CHAR;
            else
                type = NVG_CHAR;
            break;
        }

        if (type == NVG_NEWLINE)
        {
            // Always handle new lines.
            rows[nrows].start = rowStart != NULL ? rowStart : iter.str;
            rows[nrows].end = rowEnd != NULL ? rowEnd : iter.str;
            rows[nrows].width = rowWidth * invscale;
            rows[nrows].minx = rowMinX * invscale;
            rows[nrows].maxx = rowMaxX * invscale;
            rows[nrows].next = iter.next;
            nrows++;
            if (nrows >= maxRows)
                return nrows;
            // Set null break point
            breakEnd = rowStart;
            breakWidth = 0.0;
            breakMaxX = 0.0;
            // Indicate to skip the white space at the beginning of the row.
            rowStart = NULL;
            rowEnd = NULL;
            rowWidth = 0;
            rowMinX = rowMaxX = 0;
        }
        else
        {
            if (rowStart == NULL)
            {
                // Skip white space until the beginning of the line
                if (type == NVG_CHAR || type == NVG_CJK_CHAR)
                {
                    // The current char is the row so far
                    rowStartX = iter.x;
                    rowStart = iter.str;
                    rowEnd = iter.next;
                    rowWidth = iter.nextx - rowStartX;
                    rowMinX = q.x0 - rowStartX;
                    rowMaxX = q.x1 - rowStartX;
                    wordStart = iter.str;
                    wordStartX = iter.x;
                    wordMinX = q.x0 - rowStartX;
                    // Set null break point
                    breakEnd = rowStart;
                    breakWidth = 0.0;
                    breakMaxX = 0.0;
                }
            }
            else
            {
                float nextWidth = iter.nextx - rowStartX;

                // track last non-white space character
                if (type == NVG_CHAR || type == NVG_CJK_CHAR)
                {
                    rowEnd = iter.next;
                    rowWidth = iter.nextx - rowStartX;
                    rowMaxX = q.x1 - rowStartX;
                }
                // track last end of a word
                if (((ptype == NVG_CHAR || ptype == NVG_CJK_CHAR) && type == NVG_SPACE) || type == NVG_CJK_CHAR)
                {
                    breakEnd = iter.str;
                    breakWidth = rowWidth;
                    breakMaxX = rowMaxX;
                }
                // track last beginning of a word
                if ((ptype == NVG_SPACE && (type == NVG_CHAR || type == NVG_CJK_CHAR)) || type == NVG_CJK_CHAR)
                {
                    wordStart = iter.str;
                    wordStartX = iter.x;
                    wordMinX = q.x0;
                }

                // Break to new line when a character is beyond break width.
                if ((type == NVG_CHAR || type == NVG_CJK_CHAR) && nextWidth > breakRowWidth)
                {
                    // The run length is too long, need to break to new line.
                    if (breakEnd == rowStart)
                    {
                        // The current word is longer than the row length, just break it from here.
                        rows[nrows].start = rowStart;
                        rows[nrows].end = iter.str;
                        rows[nrows].width = rowWidth * invscale;
                        rows[nrows].minx = rowMinX * invscale;
                        rows[nrows].maxx = rowMaxX * invscale;
                        rows[nrows].next = iter.str;
                        nrows++;
                        if (nrows >= maxRows)
                            return nrows;
                        rowStartX = iter.x;
                        rowStart = iter.str;
                        rowEnd = iter.next;
                        rowWidth = iter.nextx - rowStartX;
                        rowMinX = q.x0 - rowStartX;
                        rowMaxX = q.x1 - rowStartX;
                        wordStart = iter.str;
                        wordStartX = iter.x;
                        wordMinX = q.x0 - rowStartX;
                    }
                    else
                    {
                        // Break the line from the end of the last word, and start new line from the beginning of the new.
                        rows[nrows].start = rowStart;
                        rows[nrows].end = breakEnd;
                        rows[nrows].width = breakWidth * invscale;
                        rows[nrows].minx = rowMinX * invscale;
                        rows[nrows].maxx = breakMaxX * invscale;
                        rows[nrows].next = wordStart;
                        nrows++;
                        if (nrows >= maxRows)
                            return nrows;
                        // Update row
                        rowStartX = wordStartX;
                        rowStart = wordStart;
                        rowEnd = iter.next;
                        rowWidth = iter.nextx - rowStartX;
                        rowMinX = wordMinX - rowStartX;
                        rowMaxX = q.x1 - rowStartX;
                    }
                    // Set null break point
                    breakEnd = rowStart;
                    breakWidth = 0.0;
                    breakMaxX = 0.0;
                }
            }
        }

        pcodepoint = iter.codepoint;
        ptype = type;
    }

    // Break the line from the end of the last word, and start new line from the beginning of the new.
    if (rowStart != NULL)
    {
        rows[nrows].start = rowStart;
        rows[nrows].end = rowEnd;
        rows[nrows].width = rowWidth * invscale;
        rows[nrows].minx = rowMinX * invscale;
        rows[nrows].maxx = rowMaxX * invscale;
        rows[nrows].next = end;
        nrows++;
    }

    return nrows;
}
*/

/*

float nvgTextBounds(FontContext& ctx, float x, float y, const char* string, const char* end, float* bounds)
{
    NVGstate* state = nvg__getState(ctx);
    float scale = nvg__getFontScale(state) * ctx.devicePxRatio;
    float invscale = 1.0f / scale;
    float width;

    if (ctx.fontId == FONS_INVALID)
        return 0;

    fonsSetSize(ctx.fs, ctx.fontSize * scale);
    fonsSetSpacing(ctx.fs, ctx.letterSpacing * scale);
    fonsSetBlur(ctx.fs, ctx.fontBlur * scale);
    fonsSetAlign(ctx.fs, ctx.textAlign);
    fonsSetFont(ctx.fs, ctx.fontId);

    width = fonsTextBounds(ctx.fs, x * scale, y * scale, string, end, bounds);
    if (bounds != NULL)
    {
        // Use line bounds for height.
        fonsLineBounds(ctx.fs, y * scale, &bounds[1], &bounds[3]);
        bounds[0] *= invscale;
        bounds[1] *= invscale;
        bounds[2] *= invscale;
        bounds[3] *= invscale;
    }
    return width * invscale;
}

void nvgTextBoxBounds(FontContext& ctx, float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds)
{
    NVGstate* state = nvg__getState(ctx);
    NVGtextRow rows[2];
    float scale = nvg__getFontScale(state) * ctx.devicePxRatio;
    float invscale = 1.0f / scale;
    int nrows = 0, i;
    int oldAlign = ctx.textAlign;
    int haling = ctx.textAlign & (NVG_ALIGN_LEFT | NVG_ALIGN_CENTER | NVG_ALIGN_RIGHT);
    int valign = ctx.textAlign & (NVG_ALIGN_TOP | NVG_ALIGN_MIDDLE | NVG_ALIGN_BOTTOM | NVG_ALIGN_BASELINE);
    float lineh = 0, rminy = 0, rmaxy = 0;
    float minx, miny, maxx, maxy;

    if (ctx.fontId == FONS_INVALID)
    {
        if (bounds != NULL)
            bounds[0] = bounds[1] = bounds[2] = bounds[3] = 0.0f;
        return;
    }

    nvgTextMetrics(ctx, NULL, NULL, &lineh);

    ctx.textAlign = NVG_ALIGN_LEFT | valign;

    minx = maxx = x;
    miny = maxy = y;

    fonsSetSize(ctx.fs, ctx.fontSize * scale);
    fonsSetSpacing(ctx.fs, ctx.letterSpacing * scale);
    fonsSetBlur(ctx.fs, ctx.fontBlur * scale);
    fonsSetAlign(ctx.fs, ctx.textAlign);
    fonsSetFont(ctx.fs, ctx.fontId);
    fonsLineBounds(ctx.fs, 0, &rminy, &rmaxy);
    rminy *= invscale;
    rmaxy *= invscale;

    while ((nrows = nvgTextBreakLines(ctx, string, end, breakRowWidth, rows, 2)))
    {
        for (i = 0; i < nrows; i++)
        {
            NVGtextRow* row = &rows[i];
            float rminx, rmaxx, dx = 0;
            // Horizontal bounds
            if (haling & NVG_ALIGN_LEFT)
                dx = 0;
            else if (haling & NVG_ALIGN_CENTER)
                dx = breakRowWidth * 0.5f - row->width * 0.5f;
            else if (haling & NVG_ALIGN_RIGHT)
                dx = breakRowWidth - row->width;
            rminx = x + row->minx + dx;
            rmaxx = x + row->maxx + dx;
            minx = nvg__minf(minx, rminx);
            maxx = nvg__maxf(maxx, rmaxx);
            // Vertical bounds.
            miny = nvg__minf(miny, y + rminy);
            maxy = nvg__maxf(maxy, y + rmaxy);

            y += lineh * ctx.lineHeight;
        }
        string = rows[nrows - 1].next;
    }

    ctx.textAlign = oldAlign;

    if (bounds != NULL)
    {
        bounds[0] = minx;
        bounds[1] = miny;
        bounds[2] = maxx;
        bounds[3] = maxy;
    }
}
*/

void fonts_text_metrics(FontContext& ctx, float* ascender, float* descender, float* lineh)
{
    float scale = get_font_scale(ctx) * ctx.devicePxRatio;
    float invscale = 1.0f / scale;

    if (ctx.fontId == FONS_INVALID)
        return;

    fonsSetSize(ctx.fs, ctx.fontSize * scale);
    fonsSetSpacing(ctx.fs, ctx.letterSpacing * scale);
    fonsSetBlur(ctx.fs, ctx.fontBlur * scale);
    fonsSetAlign(ctx.fs, ctx.textAlign);
    fonsSetFont(ctx.fs, ctx.fontId);

    fonsVertMetrics(ctx.fs, ascender, descender, lineh);
    if (ascender != NULL)
        *ascender *= invscale;
    if (descender != NULL)
        *descender *= invscale;
    if (lineh != NULL)
        *lineh *= invscale;
}

} // Nodegraph
