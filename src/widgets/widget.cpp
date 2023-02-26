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
}

}