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
    GraphView(Graph* pGraph, std::shared_ptr<Canvas> spCanvas);

    void BuildNodes();

    void Show(const MUtils::NVec2i& displaySize, const MUtils::NVec4f& clearColor);
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
        bool pendingUpdate = true;
        bool disabled = false;
        std::map<Node*, std::shared_ptr<ViewNode>> mapWorldToView;
        std::vector<nod::connection> connections;
        std::map<uint64_t, Node*> mapNodeCreateOrder;
    };

    Graph* GetGraph() const;
    Canvas* GetCanvas() const;

private:
    enum class InputDirection
    {
        X,
        Y
    };

    void EvaluateDragDelta(Canvas& canvas, Pin& pin, float delta, InputDirection dir);
    void CheckInput(Canvas& canvas, Pin& param, const MUtils::NRectf& region, float rangePerDelta, bool& hover, bool& captured, InputDirection dir);

private:
    Graph* m_pGraph;
    std::shared_ptr<Canvas> m_spCanvas;
    std::shared_ptr<GraphViewData> m_spViewData;

    Parameter* m_pCaptureParam = nullptr;
    MUtils::NVec2f m_mouseStart;
    std::shared_ptr<Parameter> m_pStartValue;

    bool m_hideCursor = false;
    uint32_t m_currentInputIndex = 0;

    std::map<Parameter*, LabelInfo> m_drawLabels;
};

}; // namespace NodeGraph