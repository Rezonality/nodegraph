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
DECLARE_NODE_STYLE(nodeTitleFontSize);
DECLARE_NODE_STYLE(nodeLayoutMargin);

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

    // Shapes
    void DrawNode(ViewNode& viewNode);
    void DrawPin(ViewNode& viewNode, Pin& pin);

    // Controls
    bool DrawKnob(ViewNode& viewNode, Pin& pin, MUtils::NRectf rect, bool miniKnob);
    SliderData DrawSlider(ViewNode& viewNode, Pin& pin, MUtils::NRectf rect);
    void DrawButton(ViewNode& viewNode, Pin& pin, MUtils::NRectf rect);
   
    bool CheckCapture(ViewNode& viewNode, Parameter& param, const MUtils::NRectf& region, bool& hover);
   
    // Labels/Adornments
    void DrawLabel(Parameter& param, const LabelInfo& pos);
    void DrawDecorator(NodeDecorator& decorator, const MUtils::NRectf& rc);

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
        std::map<Node*, std::shared_ptr<ViewNode>> mapNodeToViewNode;
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
