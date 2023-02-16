#include <cstdint>
#include <nodegraph/fontstash.h>

namespace NodeGraph {
 
#define NVG_INIT_FONTIMAGE_SIZE 512
#define NVG_MAX_FONTIMAGE_SIZE 2048
#define NVG_MAX_FONTIMAGES 4

struct FontContext
{
    struct FONScontext* fs;
    int fontImages[NVG_MAX_FONTIMAGES];
    int fontImageIdx = 0;
    float fontSize;
    float letterSpacing;
    float lineHeight;
    float fontBlur;
    int textAlign;
    int fontId;
	float xform[6]; //?
    float devicePxRatio = 1.0f;
	void* userPtr = nullptr;
    float alpha = 1.0f;
};

struct NVGglyphPosition
{
    const char* str; // Position of the glyph in the input string.
    float x; // The x-coordinate of the logical glyph position.
    float minx, maxx; // The bounds of the glyph shape.
};
typedef struct NVGglyphPosition NVGglyphPosition;

struct NVGtextRow
{
    const char* start; // Pointer to the input text where the row starts.
    const char* end; // Pointer to the input text where the row ends (one past the last character).
    const char* next; // Pointer to the beginning of the next row.
    float width; // Logical width of the row.
    float minx, maxx; // Actual bounds of the row. Logical with and bounds can differ because of kerning and some parts over extending.
};
typedef struct NVGtextRow NVGtextRow;

void fonts_init(FontContext& ctx);
void fonts_destroy(FontContext& ctx);
int fonts_create(FontContext& ctx, const char* name, const char* filename);
int fonts_create(FontContext& ctx, const char* name, const char* filename, const int fontIndex);
int fonts_create_mem(FontContext& ctx, const char* name, unsigned char* data, int ndata, int freeData);
int fonts_create_mem(FontContext& ctx, const char* name, unsigned char* data, int ndata, int freeData, const int fontIndex);
int fonts_add_fallback(FontContext& ctx, int baseFont, int fallbackFont);
int fonts_add_fallback(FontContext& ctx, const char* baseFont, const char* fallbackFont);
void fonts_reset_fallback(FontContext& ctx, int baseFont);
void fonts_reset_fallback(FontContext& ctx, const char* baseFont);
void fonts_set_face(FontContext& ctx, const char* font);
int fonts_find(FontContext& ctx, const char* name);
void fonts_text_metrics(FontContext& ctx, float* ascender, float* descender, float* lineh);

} // Nodegraph
