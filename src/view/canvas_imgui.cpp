#include "nodegraph/view/canvas_imgui.h"
#include "mutils/logger/logger.h"

using namespace MUtils;

namespace NodeGraph
{

void CanvasImGui::Begin(const NVec2f& displaySize, const NVec4f& clearColor)
{
    origin = ImGui::GetCursorScreenPos();
    auto size = ImGui::GetContentRegionAvail();
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(origin.x, origin.y), ImVec2(origin.x + size.x, origin.y + size.y), ToImColor(clearColor));
}

void CanvasImGui::End()
{
}

void CanvasImGui::FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color)
{
    auto viewCenter = ViewToPixels(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    viewCenter += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddCircleFilled(viewCenter, viewRadius, ToImColor(color), 30);
}

void CanvasImGui::FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const NVec4f& endColor)
{
    auto viewCenter = ViewToPixels(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);
    viewCenter += NVec2f(origin);

    // TODO: Should be gradient
    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddCircleFilled(viewCenter, viewRadius, ToImColor(startColor), 30);
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
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(color), radius);
}

void CanvasImGui::FillGradientRoundedRect(const NRectf& rc, float radius, const NRectf& gradientRange, const NVec4f& startColor, const NVec4f& endColor)
{
    auto viewRect = ViewToPixels(rc);
    auto viewSize = WorldSizeToViewSizeX(radius);
    auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);
    viewRect.Adjust(origin.x, origin.y);

    auto pDraw = ImGui::GetWindowDrawList();
    //pDraw->AddRectFilledMultiColor(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(startColor), ToImColor(startColor), ToImColor(startColor), ToImColor(endColor));
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(startColor), radius);
}

void CanvasImGui::FillGradientRoundedRectVarying(const NRectf& rc, const NVec4f& radius, const NRectf& gradientRange, const NVec4f& startColor, const NVec4f& endColor)
{
    auto viewRect = ViewToPixels(rc);
    auto viewSize0 = WorldSizeToViewSizeX(radius.x);
    auto viewSize1 = WorldSizeToViewSizeX(radius.y);
    auto viewSize2 = WorldSizeToViewSizeX(radius.z);
    auto viewSize3 = WorldSizeToViewSizeX(radius.w);
    auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);

    viewRect.Adjust(origin.x, origin.y);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(startColor), radius.x);
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
    auto viewPos = pos;
    float bounds[4];
    /*
    nvgTextAlign(vg, NImGui_ALIGN_MIDDLE | NImGui_ALIGN_CENTER);
    nvgFontSize(vg, size);
    nvgTextBounds(vg, viewPos.x, viewPos.y, pszText, nullptr, &bounds[0]);
    */

    //ImGui::CalcTextSize(pszText, )
    return NRectf(bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);
}

void CanvasImGui::Text(const NVec2f& pos, float size, const NVec4f& color, const char* pszText, const char* pszFace, uint32_t align)
{
    auto viewPos = ViewToPixels(pos);

    viewPos += NVec2f(origin);

    auto pDraw = ImGui::GetWindowDrawList();

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

    pDraw->AddText(ImGui::GetFont(), fontSize.y, viewPos, ToImColor(color), pszText);
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
    pDraw->PathArcTo(viewPos, viewRadius, degToRad(startAngle), degToRad(endAngle), 20);
    pDraw->PathStroke(ToImColor(color), false, viewWidth);
}


void CanvasImGui::SetAA(bool set)
{
    auto pDraw = ImGui::GetWindowDrawList();
    //nvgShapeAntiAlias(vg, set ? 1 : 0);
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
