#pragma once

#include <map>

#include "nodegraph/model/graph.h"
#include "nodegraph/view/canvas.h"
#include "nodegraph/view/viewnode.h"

namespace NodeGraph
{

struct SliderData
{
    MUtils::NRectf channel;
    MUtils::NRectf thumb;
};

struct LabelInfo
{
    LabelInfo(const MUtils::NVec2f& p, const std::string& fix = "")
        : pos(p)
        , prefix(fix)
    {
    }

    LabelInfo()
    {
    }

    MUtils::NVec2f pos = MUtils::NVec2f(0.0f);
    std::string prefix;
};

class GraphView
{
public:
    GraphView()
    {
    }

    void AddGraph(Graph* pGraph, std::shared_ptr<CanvasVG> spCanvas);
    void RemoveGraph(Graph* pGraph);
    Graph* GetCurrentGraph() const;
    void SetCurrentGraph(Graph* pGraph);

    void BuildNodes();
    void BuildNodes(Graph* pGraph);

    void Show(Graph* pGraph, const MUtils::NVec2i& displaySize);
    bool ShouldShowNode(Canvas& canvas, const Node* pNode) const;

    bool DrawKnob(Canvas& canvas, MUtils::NVec2f pos, float knobSize, bool miniKnob, Pin& pin);
    SliderData DrawSlider(Canvas& canvas, MUtils::NRectf pos, Pin& pin);
    void DrawButton(Canvas& canvas, MUtils::NRectf pos, Pin& pin);

    MUtils::NRectf DrawNode(Canvas& canvas, const MUtils::NRectf& pos, Node* pNode);

    void DrawLabel(Canvas& canvas, Parameter& param, const LabelInfo& pos);
    void DrawDecorator(Canvas& canvas, NodeDecorator& decorator, const MUtils::NRectf& rc);

    bool CheckCapture(Canvas& canvas, Parameter& param, const MUtils::NRectf& region, bool& hover);
    bool HideCursor() const
    {
        return m_hideCursor;
    }

    bool IsCaptured() const
    {
        return m_pCaptureParam != nullptr;
    }

    struct GraphViewData
    {
        ~GraphViewData()
        {
            for (auto& con : connections)
            {
                con.disconnect();
            }
        }
        std::shared_ptr<CanvasVG> spCanvas;
        bool pendingUpdate = true;
        bool disabled = false;
        std::map<Node*, std::shared_ptr<ViewNode>> mapWorldToView;
        std::vector<nod::connection> connections;
        std::map<uint32_t, Node*> mapNodeCreateOrder;
    };

    const std::map<Graph*, std::shared_ptr<GraphViewData>>& GetGraphs() const
    {
        return m_graphs;
    }

    Canvas* GetCanvas(Graph* pGraph) const;

private:
    enum class InputDirection
    {
        X,
        Y
    };

    void EvaluateDragDelta(Canvas& canvas, Pin& pin, float delta, InputDirection dir);
    void CheckInput(Canvas& canvas, Pin& param, const MUtils::NRectf& region, float rangePerDelta, bool& hover, bool& captured, InputDirection dir);

private:
    std::map<Graph*, std::shared_ptr<GraphViewData>> m_graphs;
    Graph* m_pCurrentGraph = nullptr;

    Parameter* m_pCaptureParam = nullptr;
    MUtils::NVec2f m_mouseStart;
    std::shared_ptr<Parameter> m_pStartValue;

    bool m_hideCursor = false;
    uint32_t m_currentInputIndex = 0;

    std::map<Parameter*, LabelInfo> m_drawLabels;
};

}; // namespace NodeGraph