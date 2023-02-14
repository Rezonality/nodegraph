#include <nodegraph/nodegraph.h>
#include <nodegraph/canvas_imgui.h>

namespace
{
const int CircleSegments = 40;
const int ArcSegments = 40;
}

namespace NodeGraph
{

void CanvasImGui::Begin(const glm::vec4& clearColor)
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

void CanvasImGui::FilledCircle(const glm::vec2& center, float radius, const glm::vec4& color)
{
    auto viewCenter = ViewToPixels(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    viewCenter += glm::vec2(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddCircleFilled(viewCenter, viewRadius, ToImColor(color), CircleSegments);
}

void CanvasImGui::FilledGradientCircle(const glm::vec2& center, float radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor)
{
    auto viewCenter = ViewToPixels(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    //auto viewGradientBegin = ViewToPixels(gradientRange.topLeftPx);
    //auto viewGradientEnd = ViewToPixels(gradientRange.bottomRightPx);
    viewCenter += glm::vec2(origin);

    // TODO: Should be gradient but can't do it on ImGui yet
    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddCircleFilled(viewCenter, viewRadius, ToImColor(startColor), CircleSegments);
}

void CanvasImGui::Stroke(const glm::vec2& from, const glm::vec2& to, float width, const glm::vec4& color)
{
    auto viewFrom = ViewToPixels(from);
    auto viewTo = ViewToPixels(to);
    auto viewWidth = WorldSizeToViewSizeX(width);
    
    viewFrom += glm::vec2(origin);
    viewTo += glm::vec2(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddLine(viewFrom, viewTo, ToImColor(color), viewWidth);
}

void CanvasImGui::FillRoundedRect(const NRectf& rc, float radius, const glm::vec4& color)
{
    auto viewRect = ViewToPixels(rc);
    auto viewSize = WorldSizeToViewSizeX(radius);
    viewRect.Adjust(origin.x, origin.y);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(color), viewSize);
}

void CanvasImGui::FillGradientRoundedRect(const NRectf& rc, float radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor)
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

void CanvasImGui::FillGradientRoundedRectVarying(const NRectf& rc, const glm::vec4& radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor)
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

void CanvasImGui::FillRect(const NRectf& rc, const glm::vec4& color)
{
    auto viewRect = ViewToPixels(rc);
    auto pDraw = ImGui::GetWindowDrawList();
    
    viewRect.Adjust(origin.x, origin.y);
    pDraw->AddRectFilled(viewRect.topLeftPx, viewRect.bottomRightPx, ToImColor(color));
}

NRectf CanvasImGui::TextBounds(const glm::vec2& pos, float size, const char* pszText) const
{
    // Return everything in World space, since we scale every draw call
    auto textSize = ImGui::CalcTextSize(pszText);
    float scale = size / ImGui::GetFontSize();
    return NRectf(pos.x, pos.y, textSize.x * scale, textSize.y * scale);
}

void CanvasImGui::Text(const glm::vec2& pos, float size, const glm::vec4& color, const char* pszText, const char* pszFace, uint32_t align)
{
    auto viewPos = ViewToPixels(pos);

    viewPos += glm::vec2(origin);

    auto pDraw = ImGui::GetWindowDrawList();

    ImGui::PushFont(m_pFont);
    float scale = size / ImGui::GetFontSize();

    auto fontSize = ImGui::CalcTextSize(pszText);
    fontSize.x = WorldSizeToViewSizeX(fontSize.x * scale);
    fontSize.y = WorldSizeToViewSizeY(fontSize.y * scale);
    if (align & TEXT_ALIGN_CENTER)
    {
        viewPos.x -= fontSize.x / 2.0f;
    }
    if (align & TEXT_ALIGN_MIDDLE)
    {
        viewPos.y -= fontSize.y / 2.0f;
    }

    pDraw->AddText(m_pFont, fontSize.y, viewPos, ToImColor(color), pszText);
    ImGui::PopFont();
}

void CanvasImGui::Arc(const glm::vec2& pos, float radius, float width, const glm::vec4& color, float startAngle, float endAngle)
{
    auto viewRadius = WorldSizeToViewSizeX(radius);
    auto viewPos = ViewToPixels(pos);
    auto viewWidth = WorldSizeToViewSizeX(width);

    viewPos += glm::vec2(origin);

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

void CanvasImGui::BeginStroke(const glm::vec2& from, float width, const glm::vec4& color)
{
    auto viewPos = ViewToPixels(from);
    auto size = WorldSizeToViewSizeX(width);
    
    viewPos += glm::vec2(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathClear();
    pDraw->PathLineTo(viewPos);
    
    m_pathWidth = size;
    m_pathColor = ToImColor(color);
    m_closePath = false;
}

void CanvasImGui::BeginPath(const glm::vec2& from, const glm::vec4& color)
{
    auto viewPos = ViewToPixels(from);
    viewPos += glm::vec2(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathClear();
    pDraw->PathLineTo(viewPos);
    m_pathColor = ToImColor(color);
    m_closePath = false;
}

void CanvasImGui::LineTo(const glm::vec2& to)
{
    auto viewPos = ViewToPixels(to);
    viewPos += glm::vec2(origin);

    auto pDraw = ImGui::GetWindowDrawList();
    pDraw->PathLineTo(viewPos);
}

void CanvasImGui::MoveTo(const glm::vec2& to)
{
    auto viewPos = ViewToPixels(to);
    viewPos += glm::vec2(origin);
    
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
