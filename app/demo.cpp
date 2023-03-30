#include <filesystem>
#include <fmt/format.h>
#include <memory>

#include <nodegraph/canvas.h>
#include <nodegraph/canvas_imgui.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/knob.h>
#include <nodegraph/widgets/layout.h>
#include <nodegraph/widgets/node.h>
#include <nodegraph/widgets/slider.h>
#include <nodegraph/widgets/widget.h>

#include <config_app.h>
using namespace NodeGraph;
namespace fs = std::filesystem;

namespace {
std::unique_ptr<CanvasImGui> spCanvas;
const glm::vec2 worldCenter = glm::vec2(0.0f);

struct Setter : public ISliderCB
{
    SliderValue myVal;
    virtual void UpdateSlider(Slider* pSlider, SliderOp op, SliderValue& val)
    {
        myVal.type = SliderType::Mark;
        myVal.step = 0.25f;
        if (op == SliderOp::Get)
        {
            myVal.tip = fmt::format("My Val: {:1.2f}", myVal.value);
            val = myVal;
        }
        else
        {
            myVal = val;
        }
    }
};
Setter s;
}

void demo_resize(const glm::vec2& size, IFontTexture* pFontTexture)
{
    if (!spCanvas)
    {
        spCanvas = std::make_unique<CanvasImGui>(pFontTexture, 1.0f, glm::vec2(0.1f, 20.0f));
        spCanvas->SetPixelRegionSize(size);
        spCanvas->SetWorldAtCenter(worldCenter);

// Node 1
#if 1
        {
            auto spWidget = std::make_shared<Node>("Node 1");
            spWidget->SetRect(NRectf(0.0f, -350.0f, 400.0f, 300.0f));
            spCanvas->GetRootLayout()->AddChild(spWidget);

            // Child
            auto spNodeChild = std::make_shared<Node>("Child");
            spNodeChild->SetRect(NRectf(10.0f, 40.0f, 270.0f, 190.0f));
            spNodeChild->SetFlags(WidgetFlags::DoNotLayout);
            spWidget->GetLayout()->AddChild(spNodeChild);
        }
#endif

        // Node 2
        {
            auto spWidget = std::make_shared<Node>("Node 2");
            spWidget->SetRect(NRectf(0.0f, 0.0f, 400.0f, 300.0f));
            spCanvas->GetRootLayout()->AddChild(spWidget);

            auto spRootLayout = std::make_shared<Layout>(LayoutType::Vertical);
            spRootLayout->SetLabel("Vertical Node 2");
            spRootLayout->SetSpacing(4.0f);
            spWidget->SetLayout(spRootLayout);

            auto spCustom = std::make_shared<Widget>("Custom");
            spCustom->SetConstraints(glm::uvec2(LayoutConstraint::Expanding, LayoutConstraint::Expanding));
            spCustom->SetRect(NRectf(0.0f, 0.0f, 100.0f, 50.0f));
            spRootLayout->AddChild(spCustom);

            // Sliders
            #if 1
            {
                auto spSliderLayout = std::make_shared<Layout>(LayoutType::Horizontal);
                spSliderLayout->SetLabel("Slider Horizontal Layout");
                spRootLayout->AddChild(spSliderLayout);

                auto spSlider = std::make_shared<Slider>("Amp", &s);
                spSlider->SetRect(NRectf(0.0f, 0.0f, 190.0f, 50.0f));
                spSliderLayout->AddChild(spSlider);

                spSlider = std::make_shared<Slider>("Freq", &s);
                spSlider->SetRect(NRectf(0.0f, 0.0f, 190.0f, 50.0f));
                spSliderLayout->AddChild(spSlider);

                {
                    auto spSubLayout = std::make_shared<Layout>(LayoutType::Horizontal);
                    spSubLayout->SetLabel("Sub Layout");
                    spSliderLayout->AddChild(spSubLayout);

                    spSlider = std::make_shared<Slider>("A");
                    spSlider->SetRect(NRectf(0.0f, 0.0f, 190.0f, 30.0f));
                    spSubLayout->AddChild(spSlider);

                    spSlider = std::make_shared<Slider>("B");
                    spSlider->SetRect(NRectf(0.0f, 0.0f, 190.0f, 30.0f));
                    spSlider->SetPadding(glm::vec4(4.0f));
                    spSubLayout->AddChild(spSlider);
                }
            }
            #endif

            // Knobs
            #if 1
            {
                auto spKnobLayout = std::make_shared<Layout>(LayoutType::Horizontal);
                spKnobLayout->SetLabel("Knob Horizontal Layout");
                spRootLayout->AddChild(spKnobLayout);

                auto spKnob = std::make_shared<Knob>("Attack");
                spKnob->SetRect(NRectf(0.0f, 0.0f, 200.0f, 120.0f));
                spKnob->SetPadding(glm::vec4(4.0f));
                spKnobLayout->AddChild(spKnob);
               
                spKnob = std::make_shared<Knob>("Decay");
                spKnob->SetRect(NRectf(0.0f, 0.0f, 200.0f, 120.0f));
                spKnob->SetPadding(glm::vec4(4.0f));
                spKnobLayout->AddChild(spKnob);
            }
            #endif
        }

        ThemeManager::Instance().Load(fs::path(NODEGRAPH_ROOT) / "theme.toml");
    }
    spCanvas->SetPixelRegionSize(size);
}

void demo_hierarchy_editor()
{
    std::function<void(Widget*)> DrawWidgetTree = [&DrawWidgetTree](Widget* widget) {
        ImGui::PushID(widget);
        bool node_open;
       
        if (widget->GetLayout()->GetChildren().empty())
        {
            ImGui::Text(widget->GetLabel().c_str());
            ImGui::PopID();
            return;
        }

        std::string type = widget->GetLayout()->GetLayoutType() == LayoutType::Horizontal ? "Horizontal" : "Vertical";
        node_open = ImGui::TreeNode(widget, fmt::format("{} ({})", widget->GetLabel(), type).c_str());

        if (node_open)
        {
            auto layout = widget->GetLayout();
            if (!layout->GetChildren().empty())
            {
                //std::string type = layout->GetLayoutType() == LayoutType::Horizontal ? "Horizontal" : "Vertical";
                //bool node_open = ImGui::TreeNode(layout, fmt::format("({} Children)", type).c_str());
                //if (node_open)
                {

                    auto children = layout->GetChildren();
                    for (auto child : children)
                    {
                        DrawWidgetTree(child.get());
                    }
                    //ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    };

    if (ImGui::Begin("Hierarchy"))
    {
        DrawWidgetTree((Widget*)(spCanvas->GetRootLayout()));
    }
    ImGui::End();
}

void demo_theme_editor()
{
    if (ImGui::Begin("Theme"))
    {
        auto& theme = ThemeManager::Instance();
        std::vector<StringId> themeNames;

        for (auto& [mstr, val] : theme.m_themes[theme.m_currentSetting])
        {
            themeNames.push_back(mstr);
        }

        std::sort(themeNames.begin(), themeNames.end(), [](auto& lhs, auto& rhs) {
            return lhs.ToString().substr(2) < rhs.ToString().substr(2);
        });

        std::string last;
        auto& themeMap = theme.m_themes[theme.m_currentSetting];
        for (auto& id : themeNames)
        {
            auto name = id.ToString();
            auto& val = themeMap[id];
            auto prefix = name.substr(0, 2);

            if (!last.empty() && (last != name.substr(2, 4)))
            {
                ImGui::NewLine();
            }
            last = name.substr(2, 4);

            if (prefix == "c_")
            {
                glm::vec4 v = val.ToVec4f();
                if (ImGui::ColorEdit4(name.c_str(), &v[0]))
                {
                    val.f4 = v;
                }
            }
            else if (prefix == "b_")
            {
                bool v = val.ToBool();
                if (ImGui::Checkbox(name.c_str(), &v))
                {
                    val.b = v;
                }
            }
            else if (prefix == "s_")
            {
                float f = 0.0f;
                switch (val.type)
                {
                case SettingType::Float:
                    ImGui::DragFloat(name.c_str(), &val.f);
                    break;
                case SettingType::Vec2f:
                    ImGui::DragFloat2(name.c_str(), &val.f);
                    break;
                case SettingType::Vec3f:
                    ImGui::DragFloat3(name.c_str(), &val.f);
                    break;
                case SettingType::Vec4f:
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
    demo_hierarchy_editor();

    ImGui::End();

    spCanvas->Begin(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    spCanvas->DrawGrid(100.0f);

    spCanvas->SetLineCap(LineCap::ROUND);
    spCanvas->DrawLine(glm::vec2(-10.0f, 0.0f), glm::vec2(10.0f, 0.0f), glm::vec4(1.0f, 0.2f, 0.2f, 1.0f), 1.0f);
    spCanvas->DrawLine(glm::vec2(0.0f, -10.0f), glm::vec2(0.0f, 10.0f), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f), 1.0f);

    spCanvas->FillRoundedRect(NRectf(-200.0f, -200.0f, 100.0f, 100.0f), 1.0f, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    spCanvas->FilledCircle(glm::vec2(150.0f, 150.0f), 50.0f, glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));

    spCanvas->DrawCubicBezier(glm::vec2(-200.0f, 150.0f), glm::vec2(-150.0f, 120.0f), glm::vec2(-100.0f, 150.0f), glm::vec2(-50.0f, 120.0f), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f), 1.0f);

    spCanvas->Text(glm::vec2(-150.0f, -150.0f), 30.0f, glm::vec4(1.0f, 1.0f, 0.2f, 1.0f), "Text");

    spCanvas->TextBox(glm::vec2(150.0f, -150.0f), 30.0f, 200.0f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), "This is text that has been split and aligned into a box, so that it sits within it.");

    spCanvas->Draw();

    spCanvas->End();

    spCanvas->HandleMouse();
}

void demo_cleanup()
{
    ThemeManager::Instance().Save(fs::path(NODEGRAPH_ROOT) / "theme.toml");
    spCanvas.reset();
}