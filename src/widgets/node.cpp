#include <nodegraph/canvas.h>
#include <nodegraph/widgets/node.h>
#include <nodegraph/theme.h>

namespace NodeGraph {

void Node::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    Widget::Draw(canvas);

    // Shadow
    m_rect.Adjust(theme.GetVec2f(s_nodeShadowSize));
    canvas.FillRoundedRect(m_rect, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_nodeShadow));

    m_rect.Adjust(-theme.GetVec2f(s_nodeShadowSize));
    canvas.FillRoundedRect(m_rect, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_nodeBackground));

    auto fontSize = theme.GetFloat(s_nodeTitleFontSize);
    auto titleHeight = fontSize + theme.GetFloat(s_nodeTitleFontPad) * 2.0f;
    auto titleBorder = theme.GetFloat(s_nodeTitleBorder);

    auto titlePanelRect = NRectf(m_rect.Left() + titleBorder, m_rect.Top() + titleBorder, m_rect.Width() - titleBorder * 2.0f, titleHeight);

    // Border curve * 2 to compensate
    canvas.FillRoundedRect(titlePanelRect, theme.GetFloat(s_nodeTitleBorderRadius), theme.GetVec4f(c_nodeTitleBackground));

    canvas.Text(titlePanelRect.Center(), fontSize, glm::vec4(.1f, 0.1f, 0.1f, 1.0f), "Name", nullptr);
}

}
