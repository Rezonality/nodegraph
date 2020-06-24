#pragma once

#include <map>

#include "nodegraph/model/graph.h"
#include "nodegraph/view/canvas.h"
#include "nodegraph/view/viewnode.h"

struct NVGcontext;

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
    {}

    MUtils::NVec2f pos = MUtils::NVec2f(0.0f);
    std::string prefix;
};

class GraphView
{
public:
    GraphView(Graph& m_graph, Canvas& canvas)
        : m_graph(m_graph)
        , m_canvas(canvas)
    {
        vg = static_cast<CanvasVG&>(canvas).GetVG();
    }

    void BuildNodes();
    void Show(const MUtils::NVec2i& displaySize);
    bool ShowNode(const Node* pNode) const;

    bool DrawKnob(MUtils::NVec2f pos, float knobSize, Pin& pin);
    SliderData DrawSlider(MUtils::NRectf pos, Pin& pin);
    void DrawButton(MUtils::NRectf pos, Pin& pin);

    MUtils::NRectf DrawNode(const MUtils::NRectf& pos, Node* pNode);

    void DrawLabel(Parameter& param, const LabelInfo& pos);
    void DrawDecorator(NodeDecorator& decorator, const MUtils::NRectf& rc);

    bool CheckCapture(Parameter& param, const MUtils::NRectf& region, bool& hover);
    bool HideCursor() const
    {
        return m_hideCursor;
    }

    Canvas& GetCanvas() const
    {
        return m_canvas;
    }

private:
    enum class InputDirection
    {
        X,
        Y
    };

    void EvaluateDragDelta(Pin& pin, float delta, InputDirection dir);
    void CheckInput(Pin& param, const MUtils::NRectf& region, float rangePerDelta, bool& hover, bool& captured, InputDirection dir);

private:
    Graph& m_graph;
    std::map<Node*, std::shared_ptr<ViewNode>> mapWorldToView;
    std::map<uint32_t, Node*> mapInputOrder;

    NVGcontext* vg = nullptr;

    Parameter* m_pCaptureParam = nullptr;
    MUtils::NVec2f m_mouseStart;
    std::shared_ptr<Parameter> m_pStartValue;

    bool m_hideCursor = false;
    uint32_t m_currentInputIndex = 0;

    std::map<Parameter*, LabelInfo> m_drawLabels;
    Canvas& m_canvas;
};

}; // namespace NodeGraph