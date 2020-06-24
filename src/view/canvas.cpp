#include "nodegraph/view/canvas.h"

using namespace MUtils;

namespace NodeGraph
{

const NVec2f Canvas::PixelToView(const NVec2f& pixel) const
{
    return (m_viewOrigin + (pixel / m_viewScale));
}

void Canvas::Update(const NVec2f& size, const CanvasInputState& state)
{
    m_inputState = state;

    SetPixelRect(NRectf(0, 0, size.x, size.y));

    // Handle the mouse
    if (state.canCapture)
    {
        auto viewUnderMouse = GetViewMousePos();

        float wheel = state.wheelDelta;
        if (wheel != 0.0f)
        {
            m_viewScale += wheel * (std::fabs(m_viewScale) * .1f);
            m_viewScale = std::clamp(m_viewScale, 0.1f, 10.0f);

            auto newView = GetViewMousePos();
            auto diff = newView - viewUnderMouse;
            m_viewOrigin -= diff;
        }
        else if (state.buttonDown[1] && (state.mouseDelta.x != 0 || state.mouseDelta.y != 0))
        {
            auto viewOrigin = PixelToView(NVec2f(0.0f, 0.0f));
            auto viewDelta = PixelToView(NVec2f(state.mouseDelta.x, state.mouseDelta.y));
            m_viewOrigin -= (viewDelta - viewOrigin);
        }
    }
}

void Canvas::SetPixelRect(const MUtils::NRectf& rc)
{
    m_pixelRect = rc;
}

NVec2f Canvas::ViewToPixels(const MUtils::NVec2f& pos) const
{
    auto viewTopLeft = pos - m_viewOrigin;
    return viewTopLeft * m_viewScale;
}

NRectf Canvas::ViewToPixels(const MUtils::NRectf& rc) const
{
    auto viewTopLeft = (rc.topLeftPx - m_viewOrigin) * m_viewScale;
    auto viewBottomRight = (rc.bottomRightPx - m_viewOrigin) * m_viewScale;
    return NRectf(viewTopLeft, viewBottomRight);
}

MUtils::NVec2f Canvas::ViewSizeToPixelSize(const MUtils::NVec2f& size) const
{
    return (size * m_viewScale);
}

float Canvas::WorldSizeToViewSizeX(float size) const
{
    return (size * m_viewScale);
}

float Canvas::WorldSizeToViewSizeY(float size) const
{
    return (size * m_viewScale);
}

void CanvasVG::FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color)
{
    auto viewCenter = ViewToPixels(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    nvgBeginPath(vg);
    nvgFillColor(vg, ToNVGColor(color));
    nvgCircle(vg, viewCenter.x, viewCenter.y, viewRadius);
    nvgFill(vg);
}

void CanvasVG::FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const NVec4f& endColor)
{
    auto viewCenter = ViewToPixels(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);
    nvgBeginPath(vg);
    nvgCircle(vg, viewCenter.x, viewCenter.y, viewRadius);
    auto bg = nvgLinearGradient(vg, viewGradientBegin.x, viewGradientBegin.y, viewGradientEnd.x, viewGradientEnd.y, ToNVGColor(startColor), ToNVGColor(endColor));
    nvgFillPaint(vg, bg);
    nvgFill(vg);
}

void CanvasVG::Stroke(const NVec2f& from, const NVec2f& to, float width, const NVec4f& color)
{
    auto viewFrom = ViewToPixels(from);
    auto viewTo = ViewToPixels(to);
    auto viewWidth = WorldSizeToViewSizeX(width);
    nvgBeginPath(vg);
    nvgMoveTo(vg, viewFrom.x, viewFrom.y);
    nvgLineTo(vg, viewTo.x, viewTo.y);
    nvgStrokeWidth(vg, viewWidth);
    nvgStrokeColor(vg, ToNVGColor(color));
    nvgStroke(vg);
}

void CanvasVG::FillRoundedRect(const NRectf& rc, float radius, const NVec4f& color)
{
    auto viewRect = ViewToPixels(rc);
    auto viewSize = WorldSizeToViewSizeX(radius);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, viewRect.Left(), viewRect.Top(), viewRect.Width(), viewRect.Height(), viewSize);
    nvgFillColor(vg, ToNVGColor(color));
    nvgFill(vg);
}

void CanvasVG::FillGradientRoundedRect(const NRectf& rc, float radius, const NRectf& gradientRange, const NVec4f& startColor, const NVec4f& endColor)
{
    auto viewRect = ViewToPixels(rc);
    auto viewSize = WorldSizeToViewSizeX(radius);
    auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, viewRect.Left(), viewRect.Top(), viewRect.Width(), viewRect.Height(), viewSize);
    auto bg = nvgLinearGradient(vg, viewGradientBegin.x, viewGradientBegin.y, viewGradientEnd.x, viewGradientEnd.y, ToNVGColor(startColor), ToNVGColor(endColor));
    nvgFillPaint(vg, bg);
    nvgFill(vg);
}

void CanvasVG::FillGradientRoundedRectVarying(const NRectf& rc, const NVec4f& radius, const NRectf& gradientRange, const NVec4f& startColor, const NVec4f& endColor)
{
    auto viewRect = ViewToPixels(rc);
    auto viewSize0 = WorldSizeToViewSizeX(radius.x);
    auto viewSize1 = WorldSizeToViewSizeX(radius.y);
    auto viewSize2 = WorldSizeToViewSizeX(radius.z);
    auto viewSize3 = WorldSizeToViewSizeX(radius.w);
    auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);

    nvgBeginPath(vg);
    nvgRoundedRectVarying(vg, viewRect.Left(), viewRect.Top(), viewRect.Width(), viewRect.Height(), viewSize0, viewSize1, viewSize2, viewSize3);
    auto bg = nvgLinearGradient(vg, viewGradientBegin.x, viewGradientBegin.y, viewGradientEnd.x, viewGradientEnd.y, ToNVGColor(startColor), ToNVGColor(endColor));
    nvgFillPaint(vg, bg);
    nvgFill(vg);
}

void CanvasVG::FillRect(const NRectf& rc, const NVec4f& color)
{
    auto viewRect = ViewToPixels(rc);

    nvgBeginPath(vg);
    nvgRect(vg, viewRect.Left(), viewRect.Top(), viewRect.Width(), viewRect.Height());
    nvgFillColor(vg, ToNVGColor(color));
    nvgFill(vg);
}

MUtils::NRectf CanvasVG::TextBounds(const MUtils::NVec2f& pos, float size, const char* pszText) const
{
    // Return everything in World space, since we scale every draw call
    auto viewPos = pos;
    float bounds[4];
    nvgTextAlign(vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
    nvgFontSize(vg, size);
    nvgTextBounds(vg, viewPos.x, viewPos.y, pszText, nullptr, &bounds[0]);

    return NRectf(bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);
}

void CanvasVG::Text(const NVec2f& pos, float size, const NVec4f& color, const char* pszText, const char* pszFace, uint32_t align)
{
    auto viewSize = WorldSizeToViewSizeY(size);
    auto viewPos = ViewToPixels(pos);
    nvgTextAlign(vg, (align & Canvas::TEXT_ALIGN_MIDDLE ? NVG_ALIGN_MIDDLE : NVG_ALIGN_TOP) | (align & Canvas::TEXT_ALIGN_CENTER ? NVG_ALIGN_CENTER : NVG_ALIGN_LEFT));
    nvgFontSize(vg, viewSize);
    if (pszFace == nullptr)
    {
        nvgFontFace(vg, "sans");
    }
    else
    {
        nvgFontFace(vg, pszFace);
    }
    nvgFillColor(vg, ToNVGColor(color));
    nvgText(vg, viewPos.x, viewPos.y, pszText, nullptr);
}

void CanvasVG::Arc(const NVec2f& pos, float radius, float width, const NVec4f& color, float startAngle, float endAngle)
{
    auto viewRadius = WorldSizeToViewSizeX(radius);
    auto viewPos = ViewToPixels(pos);
    auto viewWidth = WorldSizeToViewSizeX(width);
    nvgBeginPath(vg);
    nvgStrokeColor(vg, ToNVGColor(color));
    nvgArc(vg, viewPos.x, viewPos.y, viewRadius, nvgDegToRad(startAngle), nvgDegToRad(endAngle), NVG_CW);
    nvgStrokeWidth(vg, viewWidth);
    nvgStroke(vg);
}

void CanvasVG::DrawGrid(float viewStep)
{
    auto startPos = m_viewOrigin;
    startPos.x = std::floor(m_viewOrigin.x / viewStep) * viewStep;
    startPos.y = std::floor(m_viewOrigin.y / viewStep) * viewStep;

    auto size = (NVec2f(1.0f) / m_viewScale);

    nvgShapeAntiAlias(vg, 0);
    while (startPos.x < PixelToView(m_pixelRect.Size()).x)
    {
        Stroke(NVec2f(startPos.x, startPos.y), NVec2f(startPos.x, PixelToView(m_pixelRect.Size()).y), size.x, NVec4f(.9f, .9f, .9f, 0.05f));
        startPos.x += viewStep;
    }
    
    startPos.x = std::floor(m_viewOrigin.x / viewStep) * viewStep;
    while (startPos.y < PixelToView(m_pixelRect.Size()).y)
    {
        Stroke(NVec2f(startPos.x, startPos.y), NVec2f(PixelToView(m_pixelRect.Size()).x, startPos.y), size.y, NVec4f(.9f, .9f, .9f, 0.05f));
        startPos.y += viewStep;
    }
    nvgShapeAntiAlias(vg, 1);
}
    
void CanvasVG::SetAA(bool set)
{
    nvgShapeAntiAlias(vg, set ? 1 : 0);
}

void CanvasVG::BeginStroke(const MUtils::NVec2f& from, float width, const MUtils::NVec4f& color)
{
    auto viewPos = ViewToPixels(from);
    auto size = WorldSizeToViewSizeX(width);
    nvgBeginPath(vg);
    nvgStrokeColor(vg, ToNVGColor(color));
    nvgStrokeWidth(vg, size);
    nvgMoveTo(vg, viewPos.x, viewPos.y);
}

void CanvasVG::BeginPath(const MUtils::NVec2f& from, const MUtils::NVec4f& color)
{
    auto viewPos = ViewToPixels(from);
    nvgPathWinding(vg, NVGwinding::NVG_CCW);
    nvgBeginPath(vg);
    nvgFillColor(vg, ToNVGColor(color));
    nvgMoveTo(vg, viewPos.x, viewPos.y);
}

void CanvasVG::LineTo(const MUtils::NVec2f& to)
{
    auto viewPos = ViewToPixels(to);
    nvgLineTo(vg, viewPos.x, viewPos.y);
}

void CanvasVG::MoveTo(const MUtils::NVec2f& to)
{
    auto viewPos = ViewToPixels(to);
    nvgMoveTo(vg, viewPos.x, viewPos.y);
}

void CanvasVG::EndStroke()
{
    nvgStroke(vg);
}

void CanvasVG::EndPath()
{
    nvgFill(vg);
}

void CanvasVG::SetLineCap(LineCap cap)
{
    if (cap == LineCap::BUTT)
    {
        nvgLineCap(vg, NVG_BUTT);
        nvgLineJoin(vg, NVG_BUTT);
    }
    else
    {
        nvgLineCap(vg, NVG_ROUND);
        nvgLineJoin(vg, NVG_ROUND);
    }
}

void CanvasVG::ClosePath()
{
    nvgClosePath(vg);
}

} // namespace NodeGraph
