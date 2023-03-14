#include <functional>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/widget.h>
#include <nodegraph/widgets/layout.h>

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

Widget* Widget::MouseDown(CanvasInputState& input)
{
    for (auto& child : GetLayout()->GetFrontToBack())
    {
        if (child->GetWorldRect().Contains(input.worldMousePos))
        {
            if (auto pCapture = child->MouseDown(input))
            {
                return pCapture;
            }
        }
    }
    return nullptr;
}

void Widget::MouseUp(CanvasInputState& input)
{
    for (auto& child : GetLayout()->GetFrontToBack())
    {
        if (child->GetWorldRect().Contains(input.worldMousePos))
        {
            child->MouseUp(input);
        }
    }
}

bool Widget::MouseMove(CanvasInputState& input)
{
    for (auto& child : GetLayout()->GetFrontToBack())
    {
        if (child->GetWorldRect().Contains(input.worldMousePos))
        {
            return child->MouseMove(input);
        }
    }
    return false;
}

const std::string& Widget::GetLabel() const
{
    return m_label;
}

NRectf Widget::DrawSlab(Canvas& canvas, const NRectf& rect, float borderRadius, float shadowSize, const glm::vec4& shadowColor, float borderSize, const glm::vec4& borderColor, const glm::vec4& centerColor, const char* pszText, float fontPad, const glm::vec4& textColor)
{
    NRectf rc = rect;

    rc.Adjust(shadowSize, shadowSize, 0.0f, 0.0f);
    canvas.FillRoundedRect(rc, borderRadius, shadowColor);

    rc.Adjust(-shadowSize, -shadowSize);

    if (borderSize != 0.0f)
    {
        canvas.FillRoundedRect(rc, borderRadius, borderColor);
        rc.Adjust(borderSize, borderSize, -borderSize, -borderSize);
    }

    canvas.FillRoundedRect(rc, borderRadius, centerColor);

    if (pszText)
    {
        auto fontSize = rc.Height() - fontPad * 2.0f;
        canvas.Text(glm::vec2(rc.Left() + fontPad, rc.Center().y + 1), fontSize, textColor, pszText, nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_LEFT);
    }
    return rc;
}

const glm::uvec2& Widget::GetConstraints() const
{
    return m_constraints;
}

void Widget::SetConstraints(const glm::uvec2& constraints)
{
    m_constraints = constraints;
}

const glm::vec4& Widget::GetPadding() const
{
    return m_padding;
}

void Widget::SetPadding(const glm::vec4& padding)
{
    m_padding = padding;
}
    
void Widget::SetLayout(std::shared_ptr<Layout> spLayout)
{
    m_spLayout = spLayout;
    m_spLayout->SetParent(this);
}
    
Layout* Widget::GetLayout()
{
    if (!m_spLayout)
    {
        m_spLayout = std::make_shared<Layout>();
    }
    return m_spLayout.get();
}

}