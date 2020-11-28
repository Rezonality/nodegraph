#include <mutils/logger/logger.h>
#include <mutils/math/imgui_glm.h>
#include <nodegraph/view/canvas.h>

#include <imgui.h>

using namespace MUtils;

namespace NodeGraph
{

inline CanvasInputState& canvas_imgui_update_state(CanvasInputState& state, const MUtils::NRectf& region)
{
    auto mousePos = ImGui::GetIO().MousePos;

    state.mousePos = MUtils::NVec2f(mousePos.x - region.Left(), mousePos.y - region.Top());

    for (uint32_t i = 0; i < MOUSE_MAX; i++)
    {
        state.buttonClicked[i] = ImGui::GetIO().MouseClicked[i];
        state.buttonReleased[i] = ImGui::GetIO().MouseReleased[i];
        state.buttonDown[i] = ImGui::GetIO().MouseDown[i];
    }
    state.canCapture = ImGui::GetIO().WantCaptureMouse;
    state.mouseDelta = ImGui::GetIO().MouseDelta;
    state.dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
    state.wheelDelta = ImGui::GetIO().MouseWheel;
    state.resetDrag = false;
    state.captured = false;

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

    uint32_t ToImColor(const MUtils::NVec4f& val)
    {
        return ImColor(val.x, val.y, val.z, val.w);
    }

    virtual void Begin(const MUtils::NVec2f& displaySize, const MUtils::NVec4f& clearColor) override;
    virtual void End() override;
    virtual void FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color) override;
    virtual void FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NVec4f& color) override;
    virtual void FillGradientRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillGradientRoundedRectVarying(const MUtils::NRectf& rc, const MUtils::NVec4f& radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillRect(const MUtils::NRectf& rc, const MUtils::NVec4f& color) override;

    virtual void SetAA(bool set) override;
    virtual void BeginStroke(const MUtils::NVec2f& from, float width, const MUtils::NVec4f& color) override;
    virtual void BeginPath(const MUtils::NVec2f& from, const MUtils::NVec4f& color) override;
    virtual void MoveTo(const MUtils::NVec2f& to) override;
    virtual void LineTo(const MUtils::NVec2f& to) override;
    virtual void ClosePath() override;
    virtual void EndPath() override;
    virtual void EndStroke() override;

    virtual void Text(const MUtils::NVec2f& pos, float size, const MUtils::NVec4f& color, const char* pszText, const char* pszFace = nullptr, uint32_t align = TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER) override;
    virtual MUtils::NRectf TextBounds(const MUtils::NVec2f& pos, float size, const char* pszText) const override;

    virtual void Stroke(const MUtils::NVec2f& from, const MUtils::NVec2f& to, float width, const MUtils::NVec4f& color) override;

    virtual void Arc(const MUtils::NVec2f& pos, float radius, float width, const MUtils::NVec4f& color, float startAngle, float endAngle) override;

    virtual void SetLineCap(LineCap cap) override;

    virtual bool HasGradientVarying() const
    {
        return true;
    }

private:
    NVec2f displaySize;
    ImVec2 origin;
    uint32_t m_pathColor;
    float m_pathWidth;
    bool m_closePath = false;
    ImFont* m_pFont = nullptr;
};
} // namespace NodeGraph
