#include <nodegraph/imgui_glm.h>
#include <nodegraph/canvas.h>

#include <imgui.h>

namespace NodeGraph
{

inline CanvasInputState& canvas_imgui_update_state(Canvas& canvas, const NRectf& region, bool forceCanCapture = false)
{
    auto& state = canvas.GetInputState();

    auto mousePos = ImGui::GetIO().MousePos;

    state.mousePos = glm::vec2(mousePos.x - region.Left(), mousePos.y - region.Top());

    for (uint32_t i = 0; i < MOUSE_MAX; i++)
    {
        state.buttonClicked[i] = ImGui::GetIO().MouseClicked[i];
        state.buttonReleased[i] = ImGui::GetIO().MouseReleased[i];
        state.buttonDown[i] = ImGui::GetIO().MouseDown[i];
    }
    state.canCapture = ImGui::GetIO().WantCaptureMouse || forceCanCapture;
    state.mouseDelta = ImGui::GetIO().MouseDelta;
    state.dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
    state.dragDeltaRight = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
    state.wheelDelta = ImGui::GetIO().MouseWheel;

    if (ImGui::GetIO().KeyCtrl && state.buttonClicked[0] == 1)
    {
        state.slowDrag = true;
    }
    else if (state.buttonDown[0] == 0)
    {
        state.slowDrag = false;
    }

    return state;
}

class CanvasImGui : public Canvas
{
public:
    CanvasImGui(ImFont* pFont = nullptr)
        : Canvas()
        , m_pFont(pFont)
    {
        if (m_pFont == nullptr)
        {
            m_pFont = ImGui::GetFont();
        }
    }

    uint32_t ToImColor(const glm::vec4& val)
    {
        return ImColor(val.x, val.y, val.z, val.w);
    }

    virtual void Begin(const glm::vec4& clearColor) override;
    virtual void End() override;
    virtual void FilledCircle(const glm::vec2& center, float radius, const glm::vec4& color) override;
    virtual void FilledGradientCircle(const glm::vec2& center, float radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor) override;
    virtual void FillRoundedRect(const NRectf& rc, float radius, const glm::vec4& color) override;
    virtual void FillGradientRoundedRect(const NRectf& rc, float radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor) override;
    virtual void FillGradientRoundedRectVarying(const NRectf& rc, const glm::vec4& radius, const NRectf& gradientRange, const glm::vec4& startColor, const glm::vec4& endColor) override;
    virtual void FillRect(const NRectf& rc, const glm::vec4& color) override;

    virtual void SetAA(bool set) override;
    virtual void BeginStroke(const glm::vec2& from, float width, const glm::vec4& color) override;
    virtual void BeginPath(const glm::vec2& from, const glm::vec4& color) override;
    virtual void MoveTo(const glm::vec2& to) override;
    virtual void LineTo(const glm::vec2& to) override;
    virtual void ClosePath() override;
    virtual void EndPath() override;
    virtual void EndStroke() override;

    virtual void Text(const glm::vec2& pos, float size, const glm::vec4& color, const char* pszText, const char* pszFace = nullptr, uint32_t align = TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER) override;
    virtual NRectf TextBounds(const glm::vec2& pos, float size, const char* pszText) const override;

    virtual void Stroke(const glm::vec2& from, const glm::vec2& to, float width, const glm::vec4& color) override;

    virtual void Arc(const glm::vec2& pos, float radius, float width, const glm::vec4& color, float startAngle, float endAngle) override;

    virtual void SetLineCap(LineCap cap) override;

    virtual bool HasGradientVarying() const override
    {
        return true;
    }

private:
    glm::vec2 displaySize;
    ImVec2 origin;
    uint32_t m_pathColor;
    float m_pathWidth;
    bool m_closePath = false;
    ImFont* m_pFont = nullptr;
};
} // namespace NodeGraph
