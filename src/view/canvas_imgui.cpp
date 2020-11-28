#include "nodegraph/view/canvas_imgui.h"
#include "mutils/logger/logger.h"
#include "mutils/common.h"

using namespace MUtils;

namespace
{
const int CircleSegments = 40;
const int ArcSegments = 40;
}
namespace NodeGraph
{

void CanvasImGui::Begin(const NVec2f& displaySize, const NVec4f& clearColor)
{
    origin = ImGui::GetCursorScreenPos();
    auto size = ImGui::GetContentRegionAvail();

    auto bottomRight = ImVec2(origin.x + size.x, origin.y + size.y);
    ImGui::GetWindowDrawList()->PushClipRect(origin, bottomRight);
    ImGui::GetWindowDrawList()->AddRectFilled(origin, bottomRight, ToImColor(clearColor));
}

void CanvasImGui::End()
{
    ImGui::GetWindowDrawList()->PopClipRect();
}

void CanvasImGui::FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color)
{
    auto viewCenter = ViewToPixels(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    viewCenter += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddCircleFilled(viewCenter, viewRadius, ToImColor(color), CircleSegments);
}

void CanvasImGui::FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const NVec4f& endColor)
{
    auto viewCenter = ViewToPixels(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    //auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    //auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);
    viewCenter += NVec2f(origin);

    // TODO: Should be gradient
    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddCircleFilled(viewCenter, viewRadius, ToImColor(startColor), CircleSegments);
}

void CanvasImGui::Stroke(const NVec2f& from, const NVec2f& to, float width, const NVec4f& color)
{
    auto viewFrom = ViewToPixels(from);
    auto viewTo = ViewToPixels(to);
    auto viewWidth = WorldSizeToViewSizeX(width);
    
    viewFrom += NVec2f(origin);
    viewTo += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddLine(viewFrom, viewTo, ToImColor(color), viewWidth);
}

void CanvasImGui::FillRoundedRect(const NRectf& rc, float radius, const NVec4f& color)
{
    auto viewRect = ViewToPixels(rc);
    auto viewSize = WorldSizeToViewSizeX(radius);
    viewRect.Adjust(origin.x, origin.y);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(color), viewSize);
}

void CanvasImGui::FillGradientRoundedRect(const NRectf& rc, float radius, const NRectf& gradientRange, const NVec4f& startColor, const NVec4f& endColor)
{
    auto viewRect = ViewToPixels(rc);
    auto viewSize = WorldSizeToViewSizeX(radius);
    //auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    //auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);
    viewRect.Adjust(origin.x, origin.y);

    auto pDraw = ImGui::GetWindowDrawList();
    //pDraw->AddRectFilledMultiColor(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(startColor), ToImColor(startColor), ToImColor(startColor), ToImColor(endColor));
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(startColor), viewSize);
}

void CanvasImGui::FillGradientRoundedRectVarying(const NRectf& rc, const NVec4f& radius, const NRectf& gradientRange, const NVec4f& startColor, const NVec4f& endColor)
{
    auto viewRect = ViewToPixels(rc);
    float viewSize0 = 0.0f;
    //auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    //auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);

    auto flags = 0;
    if (radius.x > 0.0f)
    {
        flags |= ImDrawCornerFlags_TopLeft;
        viewSize0 = WorldSizeToViewSizeX(radius.x);
    }
    if (radius.y > 0.0f)
    {
        flags |= ImDrawCornerFlags_TopRight;
        viewSize0 = WorldSizeToViewSizeX(radius.y);
    }
    if (radius.z > 0.0f)
    {
        flags |= ImDrawCornerFlags_BotRight;
        viewSize0 = WorldSizeToViewSizeX(radius.z);
    }
    if (radius.w > 0.0f)
    {
        flags |= ImDrawCornerFlags_BotLeft;
        viewSize0 = WorldSizeToViewSizeX(radius.w);
    }

    viewRect.Adjust(origin.x, origin.y);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(startColor), viewSize0, flags);
}

void CanvasImGui::FillRect(const NRectf& rc, const NVec4f& color)
{
    auto viewRect = ViewToPixels(rc);
    auto pDraw = ImGui::GetWindowDrawList();
    
    viewRect.Adjust(origin.x, origin.y);
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(color));
}

MUtils::NRectf CanvasImGui::TextBounds(const MUtils::NVec2f& pos, float size, const char* pszText) const
{
    // Return everything in World space, since we scale every draw call
    auto textSize = ImGui::CalcTextSize(pszText);
    float scale = size / ImGui::GetFontSize();
    return NRectf(pos.x, pos.y, textSize.x * scale, textSize.y * scale);
}

void CanvasImGui::Text(const NVec2f& pos, float size, const NVec4f& color, const char* pszText, const char* pszFace, uint32_t align)
{
    auto viewPos = ViewToPixels(pos);

    viewPos += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();

    ImGui::PushFont(m_pFont);
    float scale = size / ImGui::GetFontSize();

    auto fontSize = ImGui::CalcTextSize(pszText);
    fontSize.x = WorldSizeToViewSizeX(fontSize.x * scale);
    fontSize.y = WorldSizeToViewSizeY(fontSize.y * scale);
    if (align & Canvas::TEXT_ALIGN_CENTER)
    {
        viewPos.x -= fontSize.x / 2.0f;
    }
    if (align & Canvas::TEXT_ALIGN_MIDDLE)
    {
        viewPos.y -= fontSize.y / 2.0f;
    }

    pDraw->AddText(m_pFont, fontSize.y, viewPos, ToImColor(color), pszText);
    ImGui::PopFont();
}

float degToRad(float deg)
{
	return deg / 180.0f * 3.1415926f;
}

float radToDeg(float rad)
{
	return rad / 3.1415926f * 180.0f;
}

void CanvasImGui::Arc(const NVec2f& pos, float radius, float width, const NVec4f& color, float startAngle, float endAngle)
{
    auto viewRadius = WorldSizeToViewSizeX(radius);
    auto viewPos = ViewToPixels(pos);
    auto viewWidth = WorldSizeToViewSizeX(width);

    viewPos += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathClear();
    pDraw->PathArcTo(viewPos, viewRadius, degToRad(startAngle), degToRad(endAngle), ArcSegments);
    pDraw->PathStroke(ToImColor(color), false, viewWidth);
}


void CanvasImGui::SetAA(bool set)
{
    /* Nothing currently */
    M_UNUSED(set);
}

void CanvasImGui::BeginStroke(const MUtils::NVec2f& from, float width, const MUtils::NVec4f& color)
{
    auto viewPos = ViewToPixels(from);
    auto size = WorldSizeToViewSizeX(width);
    
    viewPos += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathClear();
    pDraw->PathLineTo(viewPos);
    
    m_pathWidth = size;
    m_pathColor = ToImColor(color);
    m_closePath = false;
}

void CanvasImGui::BeginPath(const MUtils::NVec2f& from, const MUtils::NVec4f& color)
{
    auto viewPos = ViewToPixels(from);
    viewPos += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathClear();
    pDraw->PathLineTo(viewPos);
    m_pathColor = ToImColor(color);
    m_closePath = false;
}

void CanvasImGui::LineTo(const MUtils::NVec2f& to)
{
    auto viewPos = ViewToPixels(to);
    viewPos += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathLineTo(viewPos);
}

void CanvasImGui::MoveTo(const MUtils::NVec2f& to)
{
    auto viewPos = ViewToPixels(to);
    viewPos += NVec2f(origin);
    
    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathClear();
    pDraw->PathLineTo(viewPos);
}

void CanvasImGui::EndStroke()
{
    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathStroke(m_pathColor, m_closePath, m_pathWidth);
}

void CanvasImGui::EndPath()
{
    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathFillConvex(m_pathColor);
}

void CanvasImGui::SetLineCap(LineCap cap)
{
    /*
    if (cap == LineCap::BUTT)
    {
        nvgLineCap(vg, NImGui_BUTT);
        nvgLineJoin(vg, NImGui_BUTT);
    }
    else
    {
        nvgLineCap(vg, NImGui_ROUND);
        nvgLineJoin(vg, NImGui_ROUND);
    }
    */
}

void CanvasImGui::ClosePath()
{
    m_closePath = true;
}

} // namespace NodeGraph
