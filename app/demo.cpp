#include <filesystem>
#include <memory>

#include <nodegraph/canvas.h>
#include <nodegraph/canvas_imgui.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/node.h>
#include <nodegraph/widgets/slider.h>
#include <nodegraph/widgets/widget.h>

#include <config_app.h>
using namespace NodeGraph;
namespace fs = std::filesystem;

namespace {
std::unique_ptr<CanvasImGui> spCanvas;
const glm::vec2 worldCenter = glm::vec2(0.0f);

}

void demo_resize(const glm::vec2& size, IFontTexture* pFontTexture)
{
    if (!spCanvas)
    {
        spCanvas = std::make_unique<CanvasImGui>(pFontTexture, 1.0f, glm::vec2(0.1f, 20.0f));
        spCanvas->SetPixelRegionSize(size);
        spCanvas->SetWorldAtCenter(worldCenter);

        // Node 1
        {
            auto spWidget = std::make_shared<Node>("Node 1");
            spWidget->SetRect(NRectf(0.0f, -350.0f, 300.0f, 150.0f));
            spCanvas->GetRootWidget()->AddChild(spWidget);

            // Child
            auto spNodeChild = std::make_shared<Node>("Child");
            spNodeChild->SetRect(NRectf(10.0f, 40.0f, 170.0f, 90.0f));
            spWidget->AddChild(spNodeChild);
        }

        // Node 2
        {
            auto spWidget = std::make_shared<Node>("Node 2");
            spWidget->SetRect(NRectf(100.0f, -450.0f, 300.0f, 150.0f));
            spCanvas->GetRootWidget()->AddChild(spWidget);

            auto spSlider = std::make_shared<Slider>("Amplitude",
                [&](auto param, auto op, SliderValue& val) {
                    switch (param)
                    {
                    case SliderParams::Step:
                        val.f = (op == SliderOp::Get) ? .33f : 0.0f;
                        break;
                    default:
                        break;
                    }
                });
            spSlider->SetRect(NRectf(20.0f, 60.0f, 140.0f, 40.0f));
            spWidget->AddChild(spSlider);
        }

        ThemeManager::Instance().Load(fs::path(NODEGRAPH_ROOT) / "theme.toml");
    }
    spCanvas->SetPixelRegionSize(size);
}

void demo_theme_editor()
{
    if (ImGui::Begin("Theme"))
    {
        auto& theme = ThemeManager::Instance();
        for (auto& [mstr, val] : theme.m_themes[theme.m_currentTheme])
        {
            auto name = mstr.ToString();
            auto prefix = name.substr(0, 2);
            if (prefix == "c_")
            {
                glm::vec4 v = val.ToVec4f();
                if (ImGui::ColorEdit4(name.c_str(), &v[0]))
                {
                    val.f4 = v;
                }
            }
            else if (prefix == "s_")
            {
                float f = 0.0f;
                switch (val.type)
                {
                case ThemeType::Float:
                    ImGui::DragFloat(name.c_str(), &val.f);
                    break;
                case ThemeType::Vec2f:
                    ImGui::DragFloat2(name.c_str(), &val.f);
                    break;
                case ThemeType::Vec3f:
                    ImGui::DragFloat3(name.c_str(), &val.f);
                    break;
                case ThemeType::Vec4f:
                    ImGui::DragFloat4(name.c_str(), &val.f);
                    break;
                }
            }
        }
    }
}

void demo_draw()
{
    canvas_imgui_update_state(*spCanvas, spCanvas->GetPixelRegionSize(), true);

    demo_theme_editor();

    ImGui::End();

    spCanvas->Begin(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    spCanvas->DrawGrid(100.0f);

    spCanvas->SetLineCap(LineCap::ROUND);
    spCanvas->DrawLine(glm::vec2(-10.0f, 0.0f), glm::vec2(10.0f, 0.0f), glm::vec4(1.0f, 0.2f, 0.2f, 1.0f), 1.0f);
    spCanvas->DrawLine(glm::vec2(0.0f, -10.0f), glm::vec2(0.0f, 10.0f), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f), 1.0f);

    spCanvas->FillRoundedRect(NRectf(-200.0f, -200.0f, 100.0f, 100.0f), 1.0f, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    spCanvas->FilledCircle(glm::vec2(150.0f, 150.0f), 50.0f, glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));

    spCanvas->DrawCubicBezier(glm::vec2(-200.0f, 150.0f), glm::vec2(-150.0f, 120.0f), glm::vec2(-100.0f, 150.0f), glm::vec2(-50.0f, 120.0f), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f), 1.0f);

    spCanvas->Text(glm::vec2(-150.0f, -150.0f), 30.0f, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f), "Text");

    spCanvas->TextBox(glm::vec2(150.0f, -150.0f), 30.0f, 200.0f, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f), "This is text that has been split and aligned into a box, so that it sits within it.");

    spCanvas->Draw();

    spCanvas->End();

    spCanvas->HandleMouse();
}

void demo_cleanup()
{
    ThemeManager::Instance().Save(fs::path(NODEGRAPH_ROOT) / "theme.toml");
    spCanvas.reset();
}