#include <functional>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

Widget::Widget(const std::string& label)
    : m_label(label)
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

NRectf Widget::ToWorldRect(const NRectf& rc) const
{
    if (!m_pParent)
    {
        return rc;
    }

    auto rcNew = rc.Adjusted(m_pParent->GetRect().TopLeft());
    return m_pParent->ToWorldRect(rcNew);
}

NRectf Widget::GetWorldRect() const
{
    return ToWorldRect(m_rect);
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
    
const std::string& Widget::GetLabel() const
{
    return m_label;
}

NRectf Widget::DrawSlab(Canvas& canvas, const NRectf& rect, float borderRadius, float shadowSize, const glm::vec4& shadowColor, float borderSize, const glm::vec4& borderColor, const glm::vec4& centerColor, const char* pszText, float fontPad, const glm::vec4& textColor)
{
    NRectf rc = rect;

    rc.Adjust(shadowSize, shadowSize);
    canvas.FillRoundedRect(rc, borderRadius, shadowColor);

    rc.Adjust(-shadowSize, -shadowSize, -shadowSize, -shadowSize);

    if (borderSize != 0.0f)
    {
        canvas.FillRoundedRect(rc, borderRadius, borderColor);
        rc.Adjust(borderSize, borderSize, -borderSize, - borderSize);
    }

    canvas.FillRoundedRect(rc, borderRadius, centerColor);

    if (pszText)
    {
        auto fontSize = rc.Height() - fontPad * 2.0f;
        canvas.Text(glm::vec2(rc.Left() + fontPad, rc.Center().y + 1), fontSize, textColor, pszText, nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_LEFT);
    }
    return rc;
}

}