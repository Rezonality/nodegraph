#pragma once

#include <cassert>
#include <mutils/math/math.h>

#include "nodegraph/model/graph.h"

namespace NodeGraph
{

enum MouseButtons
{
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_MIDDLE,
    MOUSE_MAX
};

enum class LineCap
{
    ROUND,
    BUTT
};

// Represents the current interaction state with the canvas; used for 
// tracking mouse manipulation
struct CanvasInputState
{
    MUtils::NVec2f mousePos;
    bool buttonDown[MouseButtons::MOUSE_MAX];
    bool buttonClicked[MouseButtons::MOUSE_MAX];
    bool buttonReleased[MouseButtons::MOUSE_MAX];
    MUtils::NVec2f dragDelta;
    MUtils::NVec2f dragDeltaRight;
    MUtils::NVec2f mouseDelta;
    bool slowDrag = false;
    float wheelDelta;
    bool canCapture = false;
    bool captured = false;
};

class Canvas
{
public:
    Canvas()
    {
    }

    // Conversions between pixel space and view space; since we might pan or zoom the canvas
    const MUtils::NVec2f PixelToView(const MUtils::NVec2f& pixel) const;
    virtual MUtils::NVec2f ViewToPixels(const MUtils::NVec2f& pos) const;
    virtual MUtils::NRectf ViewToPixels(const MUtils::NRectf& rc) const;
    virtual float WorldSizeToViewSizeX(float size) const;
    virtual float WorldSizeToViewSizeY(float size) const;
    virtual MUtils::NVec2f ViewSizeToPixelSize(const MUtils::NVec2f& size) const;

    void SetPixelRect(const MUtils::NRectf& rc);
    MUtils::NRectf GetPixelRect() const
    {
        return m_pixelRect;
    }

    // Drawing functions; These are all in view space, not canvas space
    virtual void Begin(const MUtils::NVec4f& clearColor) = 0;
    virtual void End() = 0;
    virtual void FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color) = 0;
    virtual void FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) = 0;
    virtual void FillRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NVec4f& color) = 0;
    virtual void FillRect(const MUtils::NRectf& rc, const MUtils::NVec4f& color) = 0;
    virtual void FillGradientRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) = 0;
    virtual void FillGradientRoundedRectVarying(const MUtils::NRectf& rc, const MUtils::NVec4f& radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) = 0;
    virtual void Stroke(const MUtils::NVec2f& from, const MUtils::NVec2f& to, float width, const MUtils::NVec4f& color) = 0;
    virtual void Arc(const MUtils::NVec2f& pos, float radius, float width, const MUtils::NVec4f& color, float startAngle, float endAngle) = 0;
    virtual void SetAA(bool set) = 0;
    virtual void BeginStroke(const MUtils::NVec2f& from, float width, const MUtils::NVec4f& color) = 0;
    virtual void BeginPath(const MUtils::NVec2f& from, const MUtils::NVec4f& color) = 0;
    virtual void MoveTo(const MUtils::NVec2f& to) = 0;
    virtual void LineTo(const MUtils::NVec2f& to) = 0;
    virtual void SetLineCap(LineCap cap) = 0;
    virtual void ClosePath() = 0;
    virtual void EndPath() = 0;
    virtual void EndStroke() = 0;
    virtual bool HasGradientVarying() const
    {
        return true;
    }

    enum TextAlign
    {
        TEXT_ALIGN_MIDDLE = 1,
        TEXT_ALIGN_CENTER = 2,
        TEXT_ALIGN_TOP = 4,
        TEXT_ALIGN_LEFT = 8,
    };

    virtual void Text(const MUtils::NVec2f& pos, float size, const MUtils::NVec4f& color, const char* pszText, const char* pszFace = nullptr, uint32_t align = TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER) = 0;
    
    virtual MUtils::NRectf TextBounds(const MUtils::NVec2f& pos, float size, const char* pszText) const = 0;
    
    virtual void DrawGrid(float viewStep);

    virtual void HandleMouse();

    virtual float GetViewScale() const
    {
        return m_viewScale;
    }

    virtual MUtils::NVec2f GetViewMousePos() const
    {
        return PixelToView(m_inputState.mousePos);
    }

    CanvasInputState& GetInputState()
    {
        return m_inputState;
    }

    void Capture(bool cap)
    {
        m_inputState.captured = cap;
    }


protected:
    MUtils::NRectf m_pixelRect; // Pixel size on screen of canvas

    MUtils::NVec2f m_viewOrigin;
    float m_viewScale = 1.0f;
    bool m_capturedMouse = false;

    CanvasInputState m_inputState;
};

} // namespace NodeGraph
