#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/node.h>
#include <nodegraph/widgets/layout.h>

namespace NodeGraph {

Node::Node(const std::string& label)
    : Widget(label)
{

}

void Node::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    Widget::Draw(canvas);

    auto rcWorld = GetWorldRect();

    rcWorld = DrawSlab(canvas,
        rcWorld,
        theme.GetFloat(s_nodeBorderRadius),
        theme.GetFloat(s_nodeShadowSize),
        theme.GetVec4f(c_nodeShadowColor),
        theme.GetFloat(s_nodeBorderSize),
        theme.GetVec4f(c_nodeBorderColor),
        theme.GetVec4f(c_nodeCenterColor));

    auto fontSize = theme.GetFloat(s_nodeTitleSize);
    auto titleHeight = fontSize + theme.GetFloat(s_nodeTitleFontPad) * 2.0f;
    auto titlePad = theme.GetFloat(s_nodeTitlePad);

    auto titlePanelRect = NRectf(rcWorld.Left() + titlePad, rcWorld.Top() + titlePad, rcWorld.Width() - titlePad * 2.0f, titleHeight);

    rcWorld = DrawSlab(canvas,
        titlePanelRect,
        theme.GetFloat(s_nodeTitleBorderRadius),
        theme.GetFloat(s_nodeTitleShadowSize),
        theme.GetVec4f(c_nodeTitleShadowColor),
        theme.GetFloat(s_nodeTitleBorderSize),
        theme.GetVec4f(c_nodeTitleBorderColor),
        theme.GetVec4f(c_nodeTitleCenterColor),
        m_label.c_str(),
        theme.GetFloat(s_nodeTitleFontPad));

    // Layout in child coordinates
    auto layoutRect = NRectf(titlePanelRect.Left(), titlePanelRect.Bottom(), titlePanelRect.Width(), rcWorld.Bottom() - titlePanelRect.Bottom());
    layoutRect.Adjust(-GetWorldRect().Left(), -GetWorldRect().Top());
    GetLayout()->SetRect(layoutRect);
    GetLayout()->SetPadding(glm::vec4(8.0f, 8.0f, 8.0f, 8.0f));

    for (auto& child : GetLayout()->GetBackToFront())
    {
        child->Draw(canvas);
    }
}

Widget* Node::MouseDown(CanvasInputState& input)
{
    if (auto pCapture = Widget::MouseDown(input))
    {
        return pCapture;
    }
    
    if (input.buttonClicked[0])
    {
        return this;
    }
    return nullptr;
}

void Node::MouseUp(CanvasInputState& input)
{
}

bool Node::MouseMove(CanvasInputState& input)
{
    // Only move top level
    if (input.m_pMouseCapture == this)
    {
        m_rect.Adjust(input.worldMoveDelta);
        return true;
    }
    return false;
}

}
