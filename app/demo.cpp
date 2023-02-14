#include <nodegraph/canvas.h>
#include <nodegraph/canvas_imgui.h>

using namespace NodeGraph; 

namespace 
{
 std::unique_ptr<CanvasImGui> spCanvas;
}

void demo_init()
{
    spCanvas = std::make_unique<CanvasImGui>();
}

void demo_resize(const glm::vec2& size)
{
    spCanvas->SetPixelRegionSize(size);
}

void demo_draw()
{
    canvas_imgui_update_state(*spCanvas, spCanvas->GetPixelRegionSize(), true);

    spCanvas->Begin(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    spCanvas->DrawGrid(100.0f);

    spCanvas->End();
        
    spCanvas->HandleMouse();
}