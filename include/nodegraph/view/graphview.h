#pragma once

#include <map>
#include <deque>

#include <mutils/string/string_utils.h>

#include "nodegraph/model/graph.h"
#include "nodegraph/view/canvas.h"
#include "nodegraph/view/viewnode.h"
#include "nodegraph/view/node_layout.h"

namespace NodeGraph
{

#ifdef DECLARE_NODE_STYLES
#define DECLARE_NODE_STYLE(name) MUtils::StringId style_##name(#name);
#else
#define DECLARE_NODE_STYLE(name) extern MUtils::StringId style_##name;
#endif

DECLARE_NODE_STYLE(nodeOuter);
DECLARE_NODE_STYLE(nodeTitleHeight);
DECLARE_NODE_STYLE(nodeTitlePad);
DECLARE_NODE_STYLE(nodeTitleFontSize);
DECLARE_NODE_STYLE(nodeContentsPad);

#ifdef DECLARE_NODE_COLORS
#define DECLARE_NODE_COLOR(name) const MUtils::StringId color_##name(#name);
#else
#define DECLARE_NODE_COLOR(name) extern const MUtils::StringId color_##name;
#endif

DECLARE_NODE_COLOR(nodeBackground);
DECLARE_NODE_COLOR(nodeHoverBackground);
DECLARE_NODE_COLOR(nodeActiveBackground);
DECLARE_NODE_COLOR(nodeTitleColor);
DECLARE_NODE_COLOR(nodeTitleBGColor);
DECLARE_NODE_COLOR(nodeButtonTextColor);
DECLARE_NODE_COLOR(nodeHLColor);
DECLARE_NODE_COLOR(nodeShadowColor);

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

    void HandleInput();
    void Show(const MUtils::NVec4f& clearColor);
    bool ShouldShowNode(Canvas& canvas, const Node* pNode) const;

    void DrawNode(ViewNode& viewNode);

    void DrawPin(Pin& pin, ViewNode& viewNode);

    // For drawing individual controls
    bool DrawKnob(Canvas& canvas, MUtils::NRectf rect, bool miniKnob, Pin& pin);
    SliderData DrawSlider(Canvas& canvas, MUtils::NRectf rect, Pin& pin);
    void DrawButton(Canvas& canvas, MUtils::NRectf rect, Pin& pin);


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
        std::deque<Node*> nodeZOrder;
    };

    Graph* GetGraph() const;
    Canvas* GetCanvas() const;

    void SetDebugVisuals(bool debug)
    {
        m_debugVisuals = debug;
    }

public:
    static void InitStyles();
    static void InitColors();
    static void Init();

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
    Node* m_pCaptureNode = nullptr;

    MUtils::NVec2f m_mouseStart;
    std::shared_ptr<Parameter> m_pStartValue;

    bool m_hideCursor = false;
    uint32_t m_currentInputIndex = 0;
    bool m_debugVisuals = false;

    std::map<Parameter*, LabelInfo> m_drawLabels;
};

}; // namespace NodeGraph
