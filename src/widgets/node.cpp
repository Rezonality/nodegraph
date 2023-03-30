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

    auto bottomGap = theme.GetFloat(s_nodeBorderSize) + theme.GetFloat(s_nodeShadowSize);
    
    // Layout in child coordinates
    auto layoutRect = NRectf(titlePanelRect.Left(), titlePanelRect.Bottom(), titlePanelRect.Width(), GetWorldRect().Bottom() - bottomGap - titlePanelRect.Bottom());
    layoutRect.Adjust(-GetWorldRect().Left(), -GetWorldRect().Top());
    GetLayout()->SetRectWithPad(layoutRect);
    GetLayout()->SetConstraints(glm::uvec2(LayoutConstraint::Preferred, LayoutConstraint::Expanding));

    GetLayout()->Draw(canvas);
}

Widget* Node::MouseDown(CanvasInputState& input)
{
    if (auto pCapture = Widget::MouseDown(input))
    {
        return pCapture;
    }
    
    if (input.buttonClicked[0])
    {
        auto rcWorld = GetWorldRect();
        auto moveSize = 50.0f;
        auto sizeRect = NRectf(rcWorld.bottomRightPx.x - moveSize, rcWorld.bottomRightPx.y - moveSize, moveSize, moveSize);
        if (sizeRect.Contains(input.lastWorldMouseClick[0]))
        {
            m_moveType = MoveType::Resize;
        }
        else
        {
            m_moveType = MoveType::Move;
        }

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
        if (m_moveType == MoveType::Resize)
        {
            m_rect.Adjust(0.0f, 0.0f, input.worldMoveDelta.x, input.worldMoveDelta.y);
        }
        else
        {
            m_rect.Adjust(input.worldMoveDelta);
        }
        return true;
    }
    return false;
}

}
