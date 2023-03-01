#include <functional>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

Widget::Widget()
{
}
    
Widget* Widget::GetParent() const
{
    return m_pParent;
}

void Widget::SetParent(Widget* pParent)
{
    assert(!m_pParent);
    assert(pParent);
    m_pParent = pParent;
}

const NRectf& Widget::GetRect() const
{
    return m_rect;
}

void Widget::SetRect(const NRectf& sz)
{
    m_rect = sz;
}

NRectf Widget::ToWorldRect(const NRectf& rc)
{
    if (!pWidget)
    {
        return rc;
    }

    auto rcNew = rc.Adjusted(pWidget->GetRect().TopLeft());
    return ToWorldRect(pWidget->GetParent(), rcNew);
}

void Widget::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
}

void Widget::AddChild(std::shared_ptr<Widget> spWidget)
{
    m_children.push_back(spWidget);
    spWidget->SetParent(this);
    SortWidgets();
}

void Widget::MouseDown(const CanvasInputState& input)
{
}

void Widget::MouseUp(const CanvasInputState& input)
{
    std::function<void(Widget*)> pfnMove;
    pfnMove = [=](Widget* pWidget) {
        for (auto& pChild : pWidget->GetFrontToBack())
        {
            pChild->MouseMove(input);
            pfnMove(pChild.get());
        }
    };
}

bool Widget::MouseMove(const CanvasInputState& input)
{
    std::function<void(Widget*)> pfnMove;
    pfnMove = [=](Widget* pWidget) {
        for (auto& pChild : pWidget->GetFrontToBack())
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

void Widget::MoveChildToFront(std::shared_ptr<Widget> pWidget)
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

const WidgetList& Widget::GetBackToFront() const
{
    return m_children;
}

void Widget::MoveChildToBack(std::shared_ptr<Widget> pWidget)
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