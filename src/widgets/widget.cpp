#include <functional>
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

const WidgetList& Widget::GetChildren() const
{
    return m_children;
}

void Widget::AddChild(std::shared_ptr<IWidget> spWidget)
{
    m_children.push_back(spWidget);
    SortWidgets();
}

void Widget::MouseDown(const CanvasInputState& input)
{
}

void Widget::MouseUp(const CanvasInputState& input)
{
    std::function<void(IWidget*)> pfnMove;
    pfnMove = [=](IWidget* pWidget) {
        for (auto& pChild : pWidget->GetChildren())
        {
            pChild->MouseMove(input);
            pfnMove(pChild.get());
        }
    };
}

bool Widget::MouseMove(const CanvasInputState& input)
{
    std::function<void(IWidget*)> pfnMove;
    pfnMove = [=](IWidget* pWidget) {
        for (auto& pChild : pWidget->GetChildren())
        {
            pChild->MouseMove(input);
            pfnMove(pChild.get());
        }
    };
    return false;
}

void Widget::SetCapture(bool capture)
{
    m_capture = capture;
}

bool Widget::GetCapture() const
{
    return m_capture;
}

void Widget::SortWidgets()
{
    m_frontToBack = m_children;
    std::reverse(m_frontToBack.begin(), m_frontToBack.end());
}

void Widget::MoveChildToFront(std::shared_ptr<IWidget> pWidget)
{
    auto itr = std::find_if(m_children.begin(),
        m_children.end(),
        [&](const auto& pFound) -> bool {
            return pFound.get() == pWidget.get();
        });

    if (itr != m_children.end())
    {
        m_children.erase(itr);
        m_children.insert(m_children.begin(), pWidget);
    }
    SortWidgets();
}

const WidgetList& Widget::GetFrontToBack() const
{
    return m_frontToBack;
}

void Widget::MoveChildToBack(std::shared_ptr<IWidget> pWidget)
{
    auto itr = std::find_if(m_children.begin(),
        m_children.end(),
        [&](const auto& pFound) -> bool {
            return pFound.get() == pWidget.get();
        });

    if (itr != m_children.end())
    {
        m_children.erase(itr);
        m_children.insert(m_children.end(), pWidget);
    }
    SortWidgets();
}

}