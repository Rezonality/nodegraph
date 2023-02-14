#include <algorithm>
#include "nodegraph/canvas.h"
//#include "mutils/logger/logger.h"

// Note:
// View space is the virtual area on which everything is positioned.
// Pixel space is the screen area.
// The view area may be larger or smaller on screen depending on zoom, and its origin is not necessarily 0
// due to panning.

namespace NodeGraph
{

// Get the current mouse position in view space
glm::vec2 Canvas::GetViewMousePos() const
{
    return PixelToView(m_inputState.mousePos);
}

float Canvas::GetViewScale() const
{
    return m_viewScale;
}

// This is the visible pixel rect, origin 0
void Canvas::SetPixelRegionSize(const glm::vec2& sz)
{
    m_pixelSize = sz;
}

glm::vec2 Canvas::GetPixelRegionSize() const
{
    return m_pixelSize;
}

glm::vec2 Canvas::ViewToPixels(const glm::vec2& pos) const
{
    auto viewTopLeft = pos - m_viewOrigin;
    return viewTopLeft * m_viewScale;
}

NRectf Canvas::ViewToPixels(const NRectf& rc) const
{
    auto viewTopLeft = (rc.topLeftPx - m_viewOrigin) * m_viewScale;
    auto viewBottomRight = (rc.bottomRightPx - m_viewOrigin) * m_viewScale;
    return NRectf(viewTopLeft, viewBottomRight);
}

// Get the view position from pixel space
const glm::vec2 Canvas::PixelToView(const glm::vec2& pixel) const
{
    return (m_viewOrigin + (pixel / m_viewScale));
}

glm::vec2 Canvas::ViewSizeToPixelSize(const glm::vec2& size) const
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

CanvasInputState& Canvas::GetInputState()
{
    return m_inputState;
}

// Handle the mouse wheel zoom and the right button panning, for manipulating the canvas
void Canvas::HandleMouse()
{
    // We only handle moving canvas here
    if (m_inputState.captureState == CaptureState::Parameter ||
        m_inputState.captureState == CaptureState::MoveNode)
    {
        return;
    }

    auto normalizedRegion = NRectf(0.0f, 0.0f, m_pixelSize.x, m_pixelSize.y);

    bool mouseInView = normalizedRegion.Contains(m_inputState.mousePos);

    // Handle the mouse
    {
        auto viewUnderMouse = GetViewMousePos();

        float wheel = m_inputState.wheelDelta;
        if (wheel != 0.0f && mouseInView)
        {
            m_viewScale += wheel * (std::fabs(m_viewScale) * .1f);
            m_viewScale = std::clamp(m_viewScale, 0.1f, 10.0f);

            auto newView = GetViewMousePos();
            auto diff = newView - viewUnderMouse;
            m_viewOrigin -= diff;
        }
        else if ((mouseInView && m_inputState.buttonClicked[1]) || ((m_inputState.captureState == CaptureState::MoveCanvas) && m_inputState.buttonDown[1]))
        {
            auto viewOrigin = PixelToView(glm::vec2(0.0f, 0.0f));
            auto viewDelta = PixelToView(glm::vec2(m_inputState.mouseDelta.x, m_inputState.mouseDelta.y));
            m_viewOrigin -= (viewDelta - viewOrigin);
            m_inputState.captureState = CaptureState::MoveCanvas;
        }
        else if (m_inputState.buttonDown[1] == 0)
        {
            m_inputState.captureState = CaptureState::None;
        }
    }
}


void Canvas::DrawGrid(float viewStep)
{
    auto startPos = m_viewOrigin;
    startPos.x = std::floor(m_viewOrigin.x / viewStep) * viewStep;
    startPos.y = std::floor(m_viewOrigin.y / viewStep) * viewStep;

    auto size = (glm::vec2(1.0f) / m_viewScale);

    //auto pDraw = ImGui::GetWindowDrawList();

    while (startPos.x < PixelToView(m_pixelSize).x)
    {
        Stroke(glm::vec2(startPos.x, startPos.y), glm::vec2(startPos.x, PixelToView(m_pixelSize).y), size.y, glm::vec4(.9f, .9f, .9f, 0.05f));
        startPos.x += viewStep;
    }

    startPos.x = std::floor(m_viewOrigin.x / viewStep) * viewStep;
    while (startPos.y < PixelToView(m_pixelSize).y)
    {
        Stroke(glm::vec2(startPos.x, startPos.y), glm::vec2(PixelToView(m_pixelSize).x, startPos.y), size.x, glm::vec4(.9f, .9f, .9f, 0.05f));
        startPos.y += viewStep;
    }
}

void Canvas::CubicBezier(std::vector<glm::vec2>& path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = (x2 - x4) * dy - (y2 - y4) * dx;
    float d3 = (x3 - x4) * dy - (y3 - y4) * dx;
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy))
    {
        path.push_back(glm::vec2(x4, y4));
    }
    else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
        float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;
        CubicBezier(path, x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
        CubicBezier(path, x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
    }
}

void Canvas::DrawCubicBezier(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, const glm::vec4& color)
{
    pointStorage.clear();
    pointStorage.push_back(p1);
    CubicBezier(pointStorage, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, 1.f, 0);

    BeginStroke(pointStorage[0], 2.0f, color);
    for (int i = 1; i < pointStorage.size(); i++)
    {
        LineTo(pointStorage[i]);
    }
    EndStroke();

    /*
    BeginStroke(p1, 2.0f, glm::vec4(1.0f));
    LineTo(p4);
    EndStroke();
    */
}

bool Canvas::HasGradientVarying() const
{
    return true;
}

} // namespace NodeGraph
