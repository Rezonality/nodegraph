#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/node.h>

namespace NodeGraph {

Node::Node(const std::string& label)
    : Widget(label)
{

}

void Node::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    Widget::Draw(canvas);

    auto rc = GetWorldRect();

    rc = DrawSlab(canvas,
        rc,
        theme.GetFloat(s_nodeBorderRadius),
        theme.GetFloat(s_nodeShadowSize),
        theme.GetVec4f(c_nodeShadowColor),
        theme.GetFloat(s_nodeBorderSize),
        theme.GetVec4f(c_nodeBorderColor),
        theme.GetVec4f(c_nodeCenterColor));

    auto fontSize = theme.GetFloat(s_nodeTitleSize);
    auto titleHeight = fontSize + theme.GetFloat(s_nodeTitleFontPad) * 2.0f;
    auto titlePad = theme.GetFloat(s_nodeTitlePad);

    auto titlePanelRect = NRectf(rc.Left() + titlePad, rc.Top() + titlePad, rc.Width() - titlePad * 2.0f, titleHeight);

    rc = DrawSlab(canvas,
        titlePanelRect,
        theme.GetFloat(s_nodeTitleBorderRadius),
        theme.GetFloat(s_nodeTitleShadowSize),
        theme.GetVec4f(c_nodeTitleShadowColor),
        theme.GetFloat(s_nodeTitleBorderSize),
        theme.GetVec4f(c_nodeTitleBorderColor),
        theme.GetVec4f(c_nodeTitleCenterColor),
        m_label.c_str(),
        theme.GetFloat(s_nodeTitleFontPad));

    for (auto& child : GetBackToFront())
    {
        child->Draw(canvas);
    }
}

void Node::MouseDown(const CanvasInputState& input)
{
    if (input.buttonClicked[0])
    {
        m_capture = true;
    }
}

void Node::MouseUp(const CanvasInputState& input)
{
    m_capture = false;
}

bool Node::MouseMove(const CanvasInputState& input)
{
    // Only move top level
    if (m_capture)
    {
        m_rect.Adjust(input.worldMoveDelta);
        return true;
    }
    return false;
}

}
