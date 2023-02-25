#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

Widget::Widget()
{
}
const NRectf& Widget::GetRect() const
{
    return m_rect;
}

void Widget::SetRect(const NRectf& sz)
{
    m_rect = sz;
}

void Widget::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    m_rect.Adjust(theme.GetVec2f(s_nodeShadowSize));
    canvas.FillRoundedRect(m_rect, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_widgetShadow));

    m_rect.Adjust(-theme.GetVec2f(s_nodeShadowSize));
    canvas.FillRoundedRect(m_rect, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_widgetBackground));
}

}