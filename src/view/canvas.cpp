#include "nodegraph/view/canvas.h"
#include "mutils/logger/logger.h"

using namespace MUtils;

namespace NodeGraph
{

const NVec2f Canvas::PixelToView(const NVec2f& pixel) const
{
    return (m_viewOrigin + (pixel / m_viewScale));
}

void Canvas::Update(const NVec2f& regionSize, const CanvasInputState& state)
{
    m_inputState = state;

    auto normalizedRegion = NRectf(0, 0, regionSize.x, regionSize.y);
    SetPixelRect(normalizedRegion);

    bool mouseInView = normalizedRegion.Contains(state.mousePos);

    // Handle the mouse
    {
        auto viewUnderMouse = GetViewMousePos();

        float wheel = state.wheelDelta;
        if (wheel != 0.0f && mouseInView)
        {
            m_viewScale += wheel * (std::fabs(m_viewScale) * .1f);
            m_viewScale = std::clamp(m_viewScale, 0.1f, 10.0f);

            auto newView = GetViewMousePos();
            auto diff = newView - viewUnderMouse;
            m_viewOrigin -= diff;
        }
        else if ((mouseInView && state.buttonClicked[1]) || (m_capturedMouse && state.buttonDown[1]))
        {
            auto viewOrigin = PixelToView(NVec2f(0.0f, 0.0f));
            auto viewDelta = PixelToView(NVec2f(state.mouseDelta.x, state.mouseDelta.y));
            m_viewOrigin -= (viewDelta - viewOrigin);
            m_capturedMouse = true;
        }
        else if (state.buttonDown[1] == 0 )
        {
            m_capturedMouse = false;
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

void Canvas::DrawGrid(float viewStep)
{
    auto startPos = m_viewOrigin;
    startPos.x = std::floor(m_viewOrigin.x / viewStep) * viewStep;
    startPos.y = std::floor(m_viewOrigin.y / viewStep) * viewStep;

    auto size = (NVec2f(1.0f) / m_viewScale);

    //auto pDraw = ImGui::GetWindowDrawList();

    while (startPos.x < PixelToView(m_pixelRect.Size()).x)
    {
        Stroke(NVec2f(startPos.x, startPos.y), NVec2f(startPos.x, PixelToView(m_pixelRect.Size()).y), size.y, NVec4f(.9f, .9f, .9f, 0.05f));
        startPos.x += viewStep;
    }

    startPos.x = std::floor(m_viewOrigin.x / viewStep) * viewStep;
    while (startPos.y < PixelToView(m_pixelRect.Size()).y)
    {
        Stroke(NVec2f(startPos.x, startPos.y), NVec2f(PixelToView(m_pixelRect.Size()).x, startPos.y), size.x, NVec4f(.9f, .9f, .9f, 0.05f));
        startPos.y += viewStep;
    }
}

} // namespace NodeGraph
