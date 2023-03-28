#include <functional>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/layout.h>
#include <nodegraph/widgets/widget.h>
#include <nodegraph/logger/logger.h>

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
    if (m_sizeHint.x == 0.0f && m_sizeHint.y == 0.0f)
    {
        m_sizeHint = sz.Size();
    }
    m_rect = sz;
    LOG(DBG, "Widget: " << GetLabel() << ": " << m_rect);
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

void Widget::SetLabel(const char* pszLabel)
{
    m_label = pszLabel;
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
        SetLayout(std::make_shared<Layout>(LayoutType::Vertical));
    }
    return m_spLayout.get();
}

uint64_t Widget::GetFlags() const
{
    return m_flags;
}

void Widget::SetFlags(uint64_t flags)
{
    m_flags = flags;
}

glm::vec4 Widget::GetMinMaxSize() const
{
    const float minSize = 10.0f; // TBD
    const float maxSize = 100000.0f; // Arbitrary large (not max_float!)
    auto ret = glm::vec4(minSize, minSize, maxSize, maxSize);

    if (m_constraints.x & LayoutConstraint::Fixed)
    {
        ret.x = ret.z = m_rect.Width();
    }
    if (m_constraints.y & LayoutConstraint::Fixed)
    {
        ret.y = ret.w = m_rect.Height();
    }
    return ret;
}

glm::vec2 Widget::GetSizeHint() const
{
    return m_sizeHint;
}

NRectf Widget::GetRectWithPad() const
{
    return m_rect.Adjusted(glm::vec4(-m_padding.x, -m_padding.y, m_padding.z, m_padding.w));
}

void Widget::SetRectWithPad(const NRectf& rc)
{
    SetRect(rc.Adjusted(glm::vec4(m_padding.x, m_padding.y, -m_padding.z, -m_padding.w)));
}

}