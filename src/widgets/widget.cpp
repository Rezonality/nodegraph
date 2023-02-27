#include <functional>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

IWidget* Widget::s_pMouseCapture = nullptr;
MouseButton Widget::s_buttonDown = MouseButton::None;

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

const std::vector<std::shared_ptr<IWidget>>& Widget::GetChildren() const
{
    return m_children;
}

void Widget::AddChild(std::shared_ptr<IWidget> spWidget)
{
    return m_children.push_back(spWidget);
}

IWidget* Widget::HandleMouseDown(const glm::vec2& pos, MouseButton button)
{
    /*
    for (auto& pWidget : m_children)
    {
        auto pCapture = pWidget->MouseDown(pos, button);
        if (pCapture)
        {
            return pCapture;
        }
    }*/
    return nullptr;
}

void Widget::HandleMouseUp(const glm::vec2& pos, MouseButton button)
{
    if (s_pMouseCapture)
    {
        s_pMouseCapture->MouseUp(pos, button);
        s_pMouseCapture = nullptr;
        return;
    }
}

void Widget::HandleMouseMove(const glm::vec2& pos)
{
    if (s_pMouseCapture)
    {
        s_pMouseCapture->MouseMove(pos);
        s_pMouseCapture = nullptr;
        return;
    }

    std::function<void(IWidget*)> pfnMove;

    pfnMove = [=](IWidget* pWidget) {
        for (auto& pChild : pWidget->GetChildren())
        {
            pChild->MouseMove(pos);
            pfnMove(pChild.get());
        }
    };
}

}