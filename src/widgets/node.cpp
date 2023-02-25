#include <nodegraph/canvas.h>
#include <nodegraph/widgets/node.h>
#include <nodegraph/theme.h>

namespace NodeGraph {

void Node::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    Widget::Draw(canvas);

    auto rc = m_rect;
    auto textBounds = canvas.TextBounds(glm::vec2(0.0f, 0.0f), 30.0f, "Name", nullptr);
    textBounds = NRectf(rc.topLeftPx.x, rc.Top(), rc.Width(), textBounds.Height());
    textBounds.Adjust(2.0f, 2.0f, -2.0f, -2.0f);

    canvas.FillRoundedRect(textBounds, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_nodeTitleBackground));
    canvas.Text(glm::vec2(rc.Center().x, rc.Top() + textBounds.Height() * .5f), 30.0f, glm::vec4(.1f, 0.1f, 0.1f, 1.0f), "Name", nullptr);
}

}
