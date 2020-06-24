#include <mutils/logger/logger.h>

#include <mutils/math/imgui_glm.h>
#include <mutils/ui/sdl_imgui_starter.h>

#include "config_app.h"
#include <SDL.h>
#include <nodegraph/model/graph.h>
#include <nodegraph/view/graphview.h>
#include <nodegraph/view/canvas_imgui.h>

#include <GL/gl3w.h>
#include <nanovg/nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg/nanovg_gl.h>

using namespace MUtils;
using namespace NodeGraph;

#undef ERROR
class TestNode : public Node
{
public:
    DECLARE_NODE(TestNode, adder);

    TestNode(Graph& m_graph)
        : Node(m_graph, "UI Test")
    {
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

        pSlider = AddInput("Slider", 0.5f);
        pButton = AddInput("Button", (int64_t)0);

        pIntSlider = AddInput("Slider", (int64_t)0);
        pValue1 = AddInput("0-1f", .5f, ParameterAttributes(ParameterUI::Knob, 0.01f, 1.0f));
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

        if (pValue2)
            pValue2->SetViewCells(NRectf(0, 0, 1, 1));
        if (pValue3)
            pValue3->SetViewCells(NRectf(1, 0, 1, 1));
        if (pValue4)
            pValue4->SetViewCells(NRectf(2, 0, 1, 1));
        if (pValue5)
            pValue5->SetViewCells(NRectf(3, 0, .5, .5));
        if (pValue6)
            pValue6->SetViewCells(NRectf(4, 0, 1, 1));
        if (pValue7)
            pValue7->SetViewCells(NRectf(5, 0, 1, 1));
        if (pValue8)
            pValue8->SetViewCells(NRectf(6, 0, 1, 1));
        if (pValue10)
            pValue10->SetViewCells(NRectf(7, 0, 1, 1));

        // Sum
        if (pValue9)
            pValue9->SetViewCells(NRectf(4, 1, 1, 1));
        if (pValue1)
            pValue1->SetViewCells(NRectf(5, 1, 1, 1));
        if (pSum)
            pSum->SetViewCells(NRectf(3, 1, 1, 1));

        pSlider->SetViewCells(NRectf(.25f, 1, 2.5f, .5f));
        pIntSlider->SetViewCells(NRectf(.25f, 1.5, 2.5f, .5f));
        pButton->SetViewCells(NRectf(.25f, 2.0, 2.5f, .5f));

        auto pDecorator = AddDecorator(new NodeDecorator(DecoratorType::Label, "Label"));
        pDecorator->gridLocation = NRectf(4, 1, 1, 1);
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

std::vector<Node*> appNodes;

class App : public IAppStarterClient
{
public:
    App()
    {
        m_settings.flags |= AppStarterFlags::DockingEnable;
        m_settings.startSize = NVec2i(1680, 1000);
        m_settings.clearColor = NVec4f(.2f, .2f, .2f, 1.0f);
        m_settings.appName = "NodeGraph Test";

        appNodes.push_back(m_graph.CreateNode<EmptyNode>("Empty Node"));
        appNodes.push_back(m_graph.CreateNode<TestNode>());
        appNodes.push_back(m_graph.CreateNode<TestNode>());
        appNodes.push_back(m_graph.CreateNode<TestNode>());
        appNodes.push_back(m_graph.CreateNode<TestNode>());
    }

    // Inherited via IAppStarterClient
    virtual fs::path GetRootPath() const override
    {
        return fs::path(NODEGRAPH_ROOT);
    }

    virtual void Init() override
    {
        vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

        auto path = this->GetRootPath() / "run_tree" / "fonts" / "Roboto-Regular.ttf";
        auto font = nvgCreateFont(vg, "sans", path.string().c_str());

        m_spCanvas = std::make_shared<CanvasVG>(vg);
        m_spGraphView = std::make_shared<GraphView>(m_graph, *m_spCanvas);
        m_spGraphView->BuildNodes();
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
        fbo_destroy(m_fbo);
    }

    virtual void Draw(const NVec2i& displaySize) override
    {
    }

    void DrawGraph(const NVec2i& canvasSize)
    {
        if (m_spGraphView)
        {
            if (m_fbo.fbo == 0)
            {
                m_fbo = fbo_create();
            }
            fbo_resize(m_fbo, canvasSize);

            fbo_bind(m_fbo);

            sdl_imgui_clear(m_settings.clearColor);

            m_spGraphView->Show(canvasSize);
            m_graph.Compute(appNodes, 0);

            fbo_unbind(m_fbo, m_displaySize);
        }
    }

    void BeginCanvas(const NRectf& region)
    {
        static CanvasInputState state;
        m_spCanvas->Update(region.Size(), canvas_imgui_update_state(state, region));
    }

    void EndCanvas()
    {
        ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = (m_spCanvas->GetInputState().captured);
        if (m_spCanvas->GetInputState().resetDrag)
        {
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }
    }

    virtual void DrawGUI(const NVec2i& displaySize) override
    {
        m_displaySize = displaySize;

        ImGui::Begin("Canvas");

        ImVec2 pos = ImGui::GetCursorScreenPos();
        NRectf region = NRectf(pos.x, pos.y, ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
        BeginCanvas(region);

        DrawGraph(region.Size());

        ImGui::Image(*(ImTextureID*)&m_fbo.fboTexture, ImVec2(region.Width(), region.Height()), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();

        EndCanvas();

    }

    virtual AppStarterSettings& GetSettings() override
    {
        return m_settings;
    }

private:
    std::shared_ptr<NodeGraph::GraphView> m_spGraphView;
    std::shared_ptr<NodeGraph::Canvas> m_spCanvas;
    NodeGraph::Graph m_graph;
    AppStarterSettings m_settings;
    NVGcontext* vg = nullptr;
    MUtils::AppFBO m_fbo;
    NVec2i m_displaySize = 0;
    bool m_slowDrag = false;
};

App theApp;

// Main code
int main(int args, char** ppArgs)
{
    return sdl_imgui_start(args, ppArgs, &theApp);
}

/*

        */
