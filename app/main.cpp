
#include <mutils/math/imgui_glm.h>
#include <mutils/math/math_utils.h>
#include <mutils/ui/fbo.h>
#include <mutils/ui/sdl_imgui_starter.h>

#include "config_app.h"
#include <nodegraph/model/graph.h>
#include <nodegraph/view/canvas_imgui.h>
#include <nodegraph/view/canvas_vg.h>
#include <nodegraph/view/graphview.h>
#include <nodegraph/view/node_layout.h>

#include <yoga/Yoga.h>

#include <GL/gl3w.h>

#define USE_VG
#ifdef USE_VG
#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>
#endif

using namespace MUtils;
using namespace NodeGraph;

namespace
{
bool singleView = true;
}
#undef ERROR
class TestNode : public Node
{
public:
    DECLARE_NODE(TestNode, test);

    explicit TestNode(Graph& graph)
        : Node(graph, "UI Test")
    {
        m_flags = NodeFlags::OwnerDraw;

        pSum = AddOutput("Sumf", .0f, ParameterAttributes(ParameterUI::Knob, 0.0f, 1.0f));
        pSum->GetAttributes().flags |= ParameterFlags::ReadOnly;

        pValue2 = AddInput("0-1000f", 5.0f, ParameterAttributes(ParameterUI::Knob, 0.01f, 1000.0f));
        pValue2->GetAttributes().taper = 2;

        pValue10 = AddInput("2048-4800", (int64_t)48000, ParameterAttributes(ParameterUI::Knob, (int64_t)2048, (int64_t)48000));
        pValue10->GetAttributes().taper = 4.6f;
        pValue10->GetAttributes().postFix = "Hz";

        pValue3 = AddInput("-1->+1f", .001f, ParameterAttributes(ParameterUI::Knob, -1.0f, 1.0f));

        pValue4 = AddInput("-10->+1f", .001f, ParameterAttributes(ParameterUI::Knob, -10.0f, 1.0f));

        pValue5 = AddInput("Small", (int64_t)-10, ParameterAttributes(ParameterUI::Knob, (int64_t)-10, (int64_t)10));

        pValue6 = AddInput("-10->10is", (int64_t)-10, ParameterAttributes(ParameterUI::Knob, (int64_t)-10, (int64_t)10));
        pValue6->GetAttributes().step = (int64_t)4;

        pValue7 = AddInput("0->1000ie", (int64_t)0, ParameterAttributes(ParameterUI::Knob, (int64_t)0, (int64_t)1000));
        pValue7->GetAttributes().postFix = "dB";

        pValue8 = AddInput("0->1%", 0.0f, ParameterAttributes(ParameterUI::Knob, (float)0.0f, (float)1.0f));
        pValue8->GetAttributes().displayType = ParameterDisplayType::Percentage;

        pSlider = AddInput("Variable A", 0.5f);
        pButton = AddInput("Button", (int64_t)0);

        pIntSlider = AddInput("Foobar 1,3,3", (int64_t)0);
        pValue1 = AddInput("0-111f", .5f, ParameterAttributes(ParameterUI::Knob, 0.01f, 1.0f));
        pValue9 = AddInput("0-1f", .5f, ParameterAttributes(ParameterUI::Knob, 0.01f, 1.0f));

        ParameterAttributes sliderAttrib(ParameterUI::Slider, 0.0f, 1.0f);
        sliderAttrib.step = 0.25f;
        sliderAttrib.thumb = 0.25f;
        pSlider->SetAttributes(sliderAttrib);

        ParameterAttributes sliderIntAttrib(ParameterUI::Slider, (int64_t)0, (int64_t)3);
        sliderIntAttrib.step = (int64_t)1;
        sliderIntAttrib.thumb = 1 / 4.0f;
        pIntSlider->SetAttributes(sliderIntAttrib);

        ParameterAttributes buttonAttrib(ParameterUI::Button, -1ll, 3ll);
        buttonAttrib.labels = { "A", "B", "C" };
        pButton->SetAttributes(buttonAttrib);

        auto pDecorator = AddDecorator(new NodeDecorator(DecoratorType::Label, "Label"));
        pDecorator->gridLocation = NRectf(6, 1, 1, 1);

        const NVec2f KnobWidgetSize(70.0f, 90.0f);

        auto pHLayout1 = new MUtils::HLayout();
        pHLayout1->SetMargin(NVec4f(10.0f)); // Margin between controls
        GetLayout().spContents->AddItem(pHLayout1);
        pHLayout1->AddItem(pValue1, KnobWidgetSize);
        pHLayout1->AddItem(pValue2, KnobWidgetSize);
        pHLayout1->AddItem(pValue3, KnobWidgetSize);
        pHLayout1->AddItem(pValue4, KnobWidgetSize);
        pHLayout1->AddItem(pValue5, KnobWidgetSize);
        pHLayout1->AddItem(pValue6, KnobWidgetSize);

        auto pHLayout2 = new MUtils::HLayout();
        GetLayout().spContents->AddItem(pHLayout2);
        pHLayout2->AddItem(pValue7, KnobWidgetSize); // Set the height
        pHLayout2->AddItem(pValue8, KnobWidgetSize);
        pHLayout2->AddItem(pValue9, KnobWidgetSize);

        auto pVLayout3 = new MUtils::VLayout();
        pVLayout3->AddItem(pValue10, NVec2f(KnobWidgetSize.x, 0.0f));
        pVLayout3->AddItem(pSum, NVec2f(KnobWidgetSize.x, 0.0f));

        pHLayout2->AddItem(pVLayout3, KnobWidgetSize);
        pHLayout2->AddItem(pButton, NVec2f(0.0f));

        GetLayout().spRoot->UpdateLayout();
    }

    virtual void Draw(GraphView& view, Canvas& canvas, ViewNode& viewNode) override
    {
        view.SetDebugVisuals(false);
        view.DrawNode(viewNode);
    }

    virtual void Compute() override
    {
        if (pSum)
            pSum->Set(pValue1->To<float>() + pValue9->To<float>());
    }

    Pin* pSum = nullptr;
    Pin* pValue1 = nullptr;
    Pin* pValue2 = nullptr;
    Pin* pValue3 = nullptr;
    Pin* pValue4 = nullptr;
    Pin* pValue5 = nullptr;
    Pin* pValue6 = nullptr;
    Pin* pValue7 = nullptr;
    Pin* pValue8 = nullptr;
    Pin* pValue9 = nullptr;
    Pin* pValue10 = nullptr;
    Pin* pButton = nullptr;
    Pin* pSlider = nullptr;
    Pin* pIntSlider = nullptr;
};

class TestDrawNode : public Node
{
public:
    DECLARE_NODE(TestDrawNode, test);

    explicit TestDrawNode(Graph& graph)
        : Node(graph, "Test Draw")
    {
        m_flags |= NodeFlags::OwnerDraw;

        pSum = AddOutput("Sumf", .5f, ParameterAttributes(ParameterUI::Knob, 0.0f, 1.0f));

        pValue1 = AddInput("0-1000f", 5.0f, ParameterAttributes(ParameterUI::Knob, 0.01f, 1000.0f));
        pValue1->GetAttributes().taper = 2;

        pValue2 = AddInput("Foobar1", 0.5f, ParameterAttributes(ParameterUI::Slider, 0.0f, 1.0f));
        pValue2->GetAttributes().step = 0.25f;

        pValue3 = AddInput("Amp", 0.5f, ParameterAttributes(ParameterUI::Slider, 0.0f, 1.0f));
        pValue3->GetAttributes().step = 0.01f;
        pValue3->GetAttributes().taper = 4;

        pValue4 = AddInput("Noise", 0.5f, ParameterAttributes(ParameterUI::Slider, 0.0f, 1.0f));
        pValue4->GetAttributes().step = 0.25f;

        pValue5 = AddInput("Slider", 0.5f, ParameterAttributes(ParameterUI::Slider, 0.0f, 1.0f));
        pValue5->GetAttributes().step = 0.25f;
        //ParameterAttributes sliderAttrib(ParameterUI::Slider, 0.0f, 1.0f);
        pValue6 = AddInput("Slider", 0.5f, ParameterAttributes(ParameterUI::Slider, 0.0f, 1.0f));
        pValue6->GetAttributes().step = 0.25f;
        //sliderAttrib.step = 0.25f;
        //sliderAttrib.thumb = 0.25f;
        //pValue2->SetAttributes(sliderAttrib);

        auto pLayout = new MUtils::HLayout();
        GetLayout().spContents->AddItem(pLayout);

        pLayout->AddItem(pSum, NVec2f(50.0f, 50.0f));
        pLayout->AddItem(pValue1, NVec2f(100.0f, 100.0f));
        //pLayout->AddItem(pValue2, NVec2f(200.0f, 200.0f));

        auto pSliderLayout = new MUtils::VLayout();
        pSliderLayout->SetMargin(NVec4f(1.0f));
        pLayout->AddItem(pSliderLayout);
        pSliderLayout->AddItem(pValue3, NVec2f(200.0f, 30.0f));
        pSliderLayout->AddItem(pValue4, NVec2f(200.0f, 30.0f));
        pSliderLayout->AddItem(pValue5, NVec2f(200.0f, 30.0f));
        pSliderLayout->AddItem(pValue6, NVec2f(200.0f, 30.0f));

        GetLayout().spRoot->UpdateLayout();
    }

    virtual void Compute() override
    {
    }

    virtual void Draw(GraphView& view, Canvas& canvas, ViewNode& viewNode) override
    {
        view.DrawNode(viewNode);
    }

    Pin* pSum = nullptr;
    Pin* pValue1 = nullptr;
    Pin* pValue2 = nullptr;
    Pin* pValue3 = nullptr;
    Pin* pValue4 = nullptr;
    Pin* pValue5 = nullptr;
    Pin* pValue6 = nullptr;
    std::shared_ptr<NodeLayout> m_spNodeLayout;
};

std::set<Node*> appNodes;

struct GraphData
{
    GraphData(NodeGraph::Graph* pGraph, std::shared_ptr<Canvas> spCanvas)
        : spGraphView(std::make_shared<GraphView>(pGraph, spCanvas))
    {
    }

    std::shared_ptr<NodeGraph::GraphView> spGraphView;
    MUtils::Fbo fbo;
};

class App : public IAppStarterClient
{
public:
    App()
    {
        m_settings.flags |= AppStarterFlags::DockingEnable;
        m_settings.startSize = NVec2i(1680, 1000);
        m_settings.clearColor = NVec4f(.2f, .2f, .2f, 1.0f);
        m_settings.appName = "NodeGraph Test";
    }

    // Inherited via IAppStarterClient
    virtual fs::path GetRootPath() const override
    {
        return fs::path(NODEGRAPH_ROOT);
    }

    virtual void AddFonts(float size_pixels, const ImFontConfig* pConfig, const ImWchar* pRanges) override
    {
        auto fontPath = this->GetRootPath() / "run_tree" / "fonts" / "Roboto-Regular.ttf";
        m_pCanvasFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.string().c_str(), size_pixels, pConfig, pRanges);
    }

    virtual void InitBeforeDraw() override
    {
        GraphView::Init();
    }
    virtual void InitDuringDraw() override
    {

        m_spGraphA = std::make_shared<Graph>();
        m_spGraphB = std::make_shared<Graph>();

#ifdef USE_VG
        auto fontPath = this->GetRootPath() / "run_tree" / "fonts" / "Roboto-Regular.ttf";
        vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
        nvgCreateFont(vg, "sans", fontPath.string().c_str());
        auto spGraphA = std::make_shared<GraphData>(m_spGraphA.get(), std::make_shared<CanvasVG>(vg));
        auto spGraphB = std::make_shared<GraphData>(m_spGraphB.get(), std::make_shared<CanvasVG>(vg));
#else
        auto spGraphA = std::make_shared<GraphData>(m_spGraphA.get(), std::make_shared<CanvasImGui>(m_pCanvasFont));
        auto spGraphB = std::make_shared<GraphData>(m_spGraphB.get(), std::make_shared<CanvasImGui>(m_pCanvasFont));
#endif

        auto fillGraph = [&](std::shared_ptr<GraphData> graphData, const std::string& name) {
            auto pGraph = graphData->spGraphView->GetGraph();
            pGraph->SetName(name);
            auto pTestNode = pGraph->CreateNode<TestNode>();
            auto pDrawNode = pGraph->CreateNode<TestDrawNode>();

            pTestNode->SetPos(NVec2f(50.0f, 10.0f));
            pDrawNode->SetPos(NVec2f(650.0f, 10.0f));

            for (auto pNode : pGraph->GetNodes())
            {
                appNodes.insert(pNode);
            }
            m_graphs.push_back(graphData);
        };

        fillGraph(spGraphA, "Graph A");
        fillGraph(spGraphB, "Graph B");
    }

    virtual void Update(float time, const NVec2i& displaySize) override
    {
        /*
        m_settings.flags &= ~AppStarterFlags::HideCursor;
        if (m_spGraphView)
        {
            if (m_spGraphView->HideCursor())
            {
                m_settings.flags |= AppStarterFlags::HideCursor;
            }
        }
        */
    }

    virtual void Destroy() override
    {
        for (auto& spGraphData : m_graphs)
        {
            fbo_destroy(spGraphData->fbo);
        }
    }

    virtual void Draw(const NVec2i& displaySize) override
    {
    }

    void DrawGraph(GraphData& graphData)
    {
#ifdef USE_VG
        if (graphData.fbo.fbo == 0)
        {
            graphData.fbo = fbo_create();
        }
        fbo_resize(graphData.fbo, graphData.spGraphView->GetCanvas()->GetPixelRect().Size());

        fbo_bind(graphData.fbo);

        //fbo_clear(m_settings.clearColor);
#endif

        graphData.spGraphView->Show(m_settings.clearColor);
        graphData.spGraphView->GetGraph()->Compute(appNodes, 0);

#ifdef USE_VG
        fbo_unbind(graphData.fbo, m_displaySize);
#endif
    }

    void BeginCanvas(Canvas& canvas, const NRectf& region)
    {
        canvas.SetPixelRect(NRectf(0.0f, 0.0f, region.Width(), region.Height()));

        canvas_imgui_update_state(canvas, region, singleView);
    }

    void EndCanvas(Canvas& canvas)
    {
        canvas.HandleMouse();
    }

    virtual void DrawGUI(const NVec2i& displaySize) override
    {
        static bool p_open = true;
        m_displaySize = displaySize;

        static const bool opt_fullscreen = true;
        static const bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        if (!singleView)
        {
            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("DockSpace Demo", &p_open, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // DockSpace
            ImGuiIO& io = ImGui::GetIO();
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        bool captured = false;
        for (auto& spGraphData : m_graphs)
        {
            if (singleView)
            {
                ImVec2 pos = ImGui::GetWindowViewport()->Pos;
                NRectf region = NRectf(0.0f, 0.0f, ImGui::GetWindowViewport()->Size.x, ImGui::GetWindowViewport()->Size.y);

                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(region.topLeftPx.x, region.topLeftPx.y), ImVec2(region.bottomRightPx.x, region.bottomRightPx.y), ImColor(.6f, .2f, .2f, 1.0f));

                BeginCanvas(*spGraphData->spGraphView->GetCanvas(), region);

                DrawGraph(*spGraphData);

                EndCanvas(*spGraphData->spGraphView->GetCanvas());

                if (spGraphData->spGraphView->GetCanvas()->GetInputState().captureState != CaptureState::None)
                {
                    captured = true;
                }

#ifdef USE_VG
                ImGui::GetBackgroundDrawList()->AddImage(*(ImTextureID*)&spGraphData->fbo.texture, ImVec2(region.topLeftPx.x, region.topLeftPx.y), ImVec2(region.bottomRightPx.x, region.bottomRightPx.y), ImVec2(0, 1), ImVec2(1, 0));
#endif
                // Only the first graph
                break;
            }
            else
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
                if (ImGui::Begin(spGraphData->spGraphView->GetGraph()->Name().c_str()))
                {
                    ImVec2 pos = ImGui::GetCursorScreenPos();
                    NRectf region = NRectf(pos.x, pos.y, ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

                    ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + region.Width(), pos.y + region.Height()), ImColor(.2f, .2f, .2f, 1.0f));

                    BeginCanvas(*spGraphData->spGraphView->GetCanvas(), region);

                    DrawGraph(*spGraphData);

                    EndCanvas(*spGraphData->spGraphView->GetCanvas());

                    if (spGraphData->spGraphView->GetCanvas()->GetInputState().captureState != CaptureState::None)
                    {
                        captured = true;
                    }

#ifdef USE_VG
                    ImGui::Image(*(ImTextureID*)&spGraphData->fbo.texture, ImVec2(region.Width(), region.Height()), ImVec2(0, 1), ImVec2(1, 0));
#endif
                }
                ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
                ImGui::PopStyleVar(1);
                ImGui::End();
            }
        }

        if (!singleView)
        {
            ImGui::End(); // Dockspace
        }
    }

    virtual AppStarterSettings&
    GetSettings() override
    {
        return m_settings;
    }

private:
    std::vector<std::shared_ptr<GraphData>> m_graphs;
    std::shared_ptr<Graph> m_spGraphA;
    std::shared_ptr<Graph> m_spGraphB;

    AppStarterSettings m_settings;
    NVGcontext* vg = nullptr;
    NVec2i m_displaySize = 0;
    ImFont* m_pCanvasFont = nullptr;

    CanvasInputState m_canvasInputState;
};

App theApp;

// Main code
int main(int args, char** ppArgs)
{
    return sdl_imgui_start(args, ppArgs, gsl::not_null<IAppStarterClient*>(&theApp));
}
