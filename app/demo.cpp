#include <nodegraph/canvas.h>
#include <nodegraph/canvas_imgui.h>

using namespace NodeGraph; 

namespace 
{
std::unique_ptr<CanvasImGui> spCanvas;
const glm::vec2 worldCenter = glm::vec2(0.0f);
}


void demo_resize(const glm::vec2& size)
{
    if (!spCanvas)
    {
        spCanvas = std::make_unique<CanvasImGui>(1.0f, glm::vec2(0.1f, 20.0f));
        spCanvas->SetPixelRegionSize(size);
        spCanvas->SetWorldAtCenter(worldCenter);
    }
    spCanvas->SetPixelRegionSize(size);
}

void demo_draw()
{
    canvas_imgui_update_state(*spCanvas, spCanvas->GetPixelRegionSize(), true);

    spCanvas->Begin(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    spCanvas->DrawGrid(100.0f);

    spCanvas->SetLineCap(LineCap::ROUND);
    spCanvas->DrawLine(glm::vec2(-10.0f, 0.0f), glm::vec2(10.0f, 0.0f), glm::vec4(1.0f, 0.2f, 0.2f, 1.0f), 1.0f);
    spCanvas->DrawLine(glm::vec2(0.0f, -10.0f), glm::vec2(0.0f, 10.0f), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f), 1.0f);

    spCanvas->FillRoundedRect(NRectf(-200.0f, -200.0f, 100.0f, 100.0f), 1.0f, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    spCanvas->FilledCircle(glm::vec2(150.0f, 150.0f), 50.0f, glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));

    spCanvas->SetAA(true);
    spCanvas->DrawCubicBezier(glm::vec2(-200.0f, 150.0f), glm::vec2(-150.0f, 120.0f), glm::vec2(-100.0f, 150.0f), glm::vec2(-50.0f, 120.0f), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f), 1.0f);
    spCanvas->SetAA(false);

    spCanvas->End();
        
    spCanvas->HandleMouse();
}