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

    // Shadow
    rc.Adjust(theme.GetVec2f(s_nodeShadowSize));
    canvas.FillRoundedRect(rc, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_nodeShadow));

    // Border rectangle
    rc.Adjust(-theme.GetVec2f(s_nodeShadowSize));
    canvas.FillRoundedRect(rc, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_nodeBorderColor));

    // Center color
    auto borderSize = theme.GetFloat(s_nodeBorderSize);
    rc.Adjust(borderSize, borderSize, -borderSize, -borderSize);
    canvas.FillRoundedRect(rc, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_nodeBackground));

    auto fontSize = theme.GetFloat(s_nodeTitleFontSize);
    auto titleHeight = fontSize + theme.GetFloat(s_nodeTitleFontPad) * 2.0f;
    auto titleBorder = theme.GetFloat(s_nodeTitleBorder);

    auto titlePanelRect = NRectf(rc.Left() + titleBorder, rc.Top() + titleBorder, rc.Width() - titleBorder * 2.0f, titleHeight);

    // Border curve * 2 to compensate, title background
    canvas.FillRoundedRect(titlePanelRect, theme.GetFloat(s_nodeTitleBorderRadius), theme.GetVec4f(c_nodeTitleBackground));

    // Text
    canvas.Text(titlePanelRect.Center(), fontSize, glm::vec4(.1f, 0.1f, 0.1f, 1.0f), m_label.c_str(), nullptr);

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
