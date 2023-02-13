#pragma once

#include <cassert>
#include <vector>

#include <nodegraph/math_utils.h>

//#include "nodegraph/model/graph.h"

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

enum class CaptureState
{
    None,
    MoveCanvas,
    MoveNode,
    Parameter
};

// Represents the current interaction state with the canvas; used for 
// tracking mouse manipulation
struct CanvasInputState
{
    glm::vec2 mousePos;
    bool buttonDown[MouseButtons::MOUSE_MAX];
    bool buttonClicked[MouseButtons::MOUSE_MAX];
    bool buttonReleased[MouseButtons::MOUSE_MAX];
    glm::vec2 dragDelta;
    glm::vec2 dragDeltaRight;
    glm::vec2 mouseDelta;
    bool slowDrag = false;
    float wheelDelta;
    bool canCapture = false;
    CaptureState captureState = CaptureState::None;
};

class Canvas
{
public:
    Canvas()
    {
    }

    // Conversions between pixel space and view space; since we might pan or zoom the canvas
    const glm::vec2 PixelToView(const glm::vec2& pixel) const;
    virtual glm::vec2 ViewToPixels(const glm::vec2& pos) const;
    virtual NRectf ViewToPixels(const NRectf& rc) const;
    virtual float WorldSizeToViewSizeX(float size) const;
    virtual float WorldSizeToViewSizeY(float size) const;
    virtual glm::vec2 ViewSizeToPixelSize(const glm::vec2& size) const;

    void SetPixelRect(const NRectf& rc);
    NRectf GetPixelRect() const
    {
        return m_pixelRect;
    }

    // Drawing functions; These are all in view space, not canvas space
    virtual void Begin(const glm::vec4& clearColor) = 0;
    virtual void End() = 0;
    virtual void FilledCircle(const glm::vec2& center, float radius, const glm::vec4& color) = 0;
    virtual void FilledGradientCircle(const glm::vec2& center, float radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor) = 0;
    virtual void FillRoundedRect(const NRectf& rc, float radius, const glm::vec4& color) = 0;
    virtual void FillRect(const NRectf& rc, const glm::vec4& color) = 0;
    virtual void FillGradientRoundedRect(const NRectf& rc, float radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor) = 0;
    virtual void FillGradientRoundedRectVarying(const NRectf& rc, const glm::vec4& radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor) = 0;
    virtual void Stroke(const glm::vec2& from, const glm::vec2& to, float width, const glm::vec4& color) = 0;
    virtual void Arc(const glm::vec2& pos, float radius, float width, const glm::vec4& color, float startAngle, float endAngle) = 0;
    virtual void SetAA(bool set) = 0;
    virtual void BeginStroke(const glm::vec2& from, float width, const glm::vec4& color) = 0;
    virtual void BeginPath(const glm::vec2& from, const glm::vec4& color) = 0;
    virtual void MoveTo(const glm::vec2& to) = 0;
    virtual void LineTo(const glm::vec2& to) = 0;
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

    virtual void Text(const glm::vec2& pos, float size, const glm::vec4& color, const char* pszText, const char* pszFace = nullptr, uint32_t align = TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER) = 0;
    
    virtual NRectf TextBounds(const glm::vec2& pos, float size, const char* pszText) const = 0;
    
    virtual void DrawGrid(float viewStep);
    virtual void DrawCubicBezier(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, const glm::vec4& color);

    virtual void HandleMouse();

    virtual float GetViewScale() const
    {
        return m_viewScale;
    }

    virtual glm::vec2 GetViewMousePos() const
    {
        return PixelToView(m_inputState.mousePos);
    }

    CanvasInputState& GetInputState()
    {
        return m_inputState;
    }

    void CubicBezier(std::vector<glm::vec2>& path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level);

protected:
    NRectf m_pixelRect; // Pixel size on screen of canvas

    glm::vec2 m_viewOrigin = glm::vec2(0.0f);
    float m_viewScale = 1.0f;

    CanvasInputState m_inputState;

    std::vector<glm::vec2> pointStorage;
};

} // namespace NodeGraph
