#include <imgui/imgui.h>
#include <nodegraph/view/canvas.h>

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

} // namespace NodeGraph
