#include <algorithm>
#include <fmt/format.h>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/logger/logger.h>
#include <nodegraph/widgets/layout.h>

namespace NodeGraph {

Layout::Layout(LayoutType type)
{
    m_layoutType = type;
}

void Layout::LayoutWidget(Widget* pWidget)
{
    // Walk the children
    glm::vec2 finalSize = glm::vec2(0.0f, 0.0f);
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    for (auto& pWidget : pWidget->GetLayout()->GetChildren())
    {
        if (pWidget->GetFlags() & WidgetFlags::DoNotLayout)
        {
            continue;
        }

        pWidget->SetRect(pWidget->GetRect().Moved(position));

        LayoutWidget(pWidget.get());

        auto rcWidget = pWidget->GetRect();

        switch (m_layoutType)
        {
        case LayoutType::Vertical:
            position.y += rcWidget.Height();
            finalSize.y += rcWidget.Height();
            finalSize.x = std::max(rcWidget.Width(), finalSize.x);
            break;
        case LayoutType::Horizontal:
            position.x += rcWidget.Width();
            finalSize.x += rcWidget.Width();
            finalSize.y = std::max(rcWidget.Height(), finalSize.y);
            break;
        }
    }
    m_rect.SetSize(finalSize);

    LOG(DBG, "Widget: " << GetLabel() << " : " << m_rect);
}


void Layout::Update()
{
    LayoutWidget(this);
}

/*
void Layout::Update()
{
    float totalFixedSize = 0;
    float availableSize = 0;
    float variableCount = 0;

    NRectf layoutRect = GetRect();

    float layoutSize = 0.0f;

    SizeHint hint;
    GetChildrenSizeHint(hint);

    uint32_t totalWidgets = 0;
    Widget* pLastWidget = nullptr;
    for (auto& pWidget : m_children)
    {
        if (pWidget->GetFlags() & WidgetFlags::DoNotLayout)
        {
            continue;
        }
        pLastWidget = pWidget.get();
        totalWidgets++;
        glm::uvec2 constraints = pWidget->GetConstraints();
        NRectf widgetRect = pWidget->GetRectWithPad();

        switch (m_layoutType)
        {
        case LayoutType::Vertical:
            if (constraints.y == LayoutConstraint::Fixed)
            {
                totalFixedSize += widgetRect.Height();
            }
            else
            {
                // totalFixedSize += widgetPad.y + widgetPad.w;
                variableCount++;
            }
            break;
        case LayoutType::Horizontal:
            if (constraints.x == LayoutConstraint::Fixed)
            {
                totalFixedSize += widgetRect.Width();
            }
            else
            {
                // totalFixedSize += widgetPad.x + widgetPad.z;
                variableCount++;
            }
            break;
        }
    }

    // Add space between all widgets
    float spacingSize = 0.0f;
    if (totalWidgets > 0)
    {
        spacingSize += ((totalWidgets - 1) * m_spacing);
    }

    // The rectangle for our layout contains all widgets plus our own padding
    // The layout rectangle grows in the minor axis but not the major one; it is fixed at whatever size it was when it was stacked
    auto contentMargins = GetContentsMargins();

    switch (m_layoutType)
    {
    case LayoutType::Vertical:
        // Set size of owner
        layoutRect.SetSize(m_constraints.x == LayoutConstraint::Expanding ? hint.hint.x + contentMargins.x + contentMargins.z : layoutRect.Width(), layoutRect.Height());
        availableSize = layoutRect.Height() - totalFixedSize - contentMargins.y - contentMargins.w;
        break;
    case LayoutType::Horizontal:
        layoutRect.SetSize(layoutRect.Width(), m_constraints.y == LayoutConstraint::Expanding ? hint.hint.y + contentMargins.y + contentMargins.w : layoutRect.Height());
        availableSize = layoutRect.Width() - totalFixedSize - contentMargins.x - contentMargins.z;
        break;
    }

    // Rect is the max minor axis size + content margins
    // Major axis is whatever it was before
    m_rect = layoutRect;

    // Layout rect is now the inner rect; in child rect coordinates
    layoutRect = NRectf(0.0f, 0.0f, layoutRect.Width(), layoutRect.Height());
    layoutRect.Adjust(contentMargins.x, contentMargins.y, -contentMargins.z, -contentMargins.w);

    // Local World space
    m_innerRect = layoutRect.Adjusted(m_rect.TopLeft());

    float expandingWidgetSize = (availableSize - totalFixedSize - spacingSize) / variableCount;
    for (auto& pWidget : m_children)
    {
        if (pWidget->GetFlags() & WidgetFlags::DoNotLayout)
        {
            continue;
        }

        NRectf widgetRect = pWidget->GetRectWithPad();
        glm::uvec2 constraints = pWidget->GetConstraints();
        auto space = (pWidget.get() == pLastWidget ? 0.0f : m_spacing);

        if (auto pLayout = dynamic_cast<Layout*>(pWidget.get()))
        {
            switch (m_layoutType)
            {
            case LayoutType::Vertical:
                widgetRect.SetSize(glm::vec2(layoutRect.Width(), pLayout->GetRect().Height()));
                break;
            case LayoutType::Horizontal:
                widgetRect.SetSize(glm::vec2(pLayout->GetRect().Width(), layoutRect.Height()));
                break;
            }
        }

        switch (m_layoutType)
        {
        case LayoutType::Vertical:
            widgetRect.Move(glm::vec2(layoutRect.Left(), layoutRect.Top()));
            if (constraints.y == LayoutConstraint::Expanding)
            {
                widgetRect.SetSize(widgetRect.Width(), expandingWidgetSize);
            }
            layoutRect.SetTop(widgetRect.Bottom() + space);
            break;
        case LayoutType::Horizontal:
            widgetRect.Move(glm::vec2(layoutRect.Left(), layoutRect.Top()));
            if (constraints.x == LayoutConstraint::Expanding)
            {
                widgetRect.SetSize(expandingWidgetSize, widgetRect.Height());
            }
            layoutRect.SetLeft(widgetRect.Right() + space);
            break;
        }
        pWidget->SetRectWithPad(widgetRect);
    }

    // Now the widgets might be too far apart; if we are stacking top->bottom
    float lastEdge = 0;
    for (auto& pWidget : m_children)
    {
        auto rc = pWidget->GetRectWithPad();
        if (m_layoutType == LayoutType::Vertical)
        {
            if (rc.Top() > (lastEdge + m_spacing))
            {
                pWidget->SetRectWithPad(rc.Moved(glm::vec2(rc.Left(), lastEdge + m_spacing)));
            }
            lastEdge = rc.Bottom();
        }
        else
        {
            if (rc.Left() > (lastEdge + m_spacing))
            {
                pWidget->SetRectWithPad(rc.Moved(glm::vec2(lastEdge + m_spacing, rc.Top())));
            }
            lastEdge = rc.Right();
        }
    }
}
*/

void Layout::SetRect(const NRectf& sz)
{
    auto rc = sz;

    // Constrain this rectangle to the limits of the child widgets
    /*
    auto sizes = GetChildrenMinMaxSize();
    if (sz.Width() > sizes.z)
    {
        rc.SetWidth(sizes.z);
    }
    if (sz.Height() > sizes.w)
    {
        rc.SetWidth(sizes.w);
    }
    if (sz.Width() < sizes.x)
    {
        rc.SetWidth(sizes.x);
    }
    if (sz.Height() < sizes.y)
    {
        rc.SetHeight(sizes.y);
    }
    */

    Widget::SetRect(rc);

    Update();
}

void Layout::AddChild(std::shared_ptr<Widget> spWidget)
{
    m_children.push_back(spWidget);
    spWidget->SetParent(this);
    SortWidgets();
}

void Layout::MoveChildToBack(std::shared_ptr<Widget> pWidget)
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

void Layout::MoveChildToFront(std::shared_ptr<Widget> pWidget)
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

const WidgetList& Layout::GetFrontToBack() const
{
    return m_frontToBack;
}

const WidgetList& Layout::GetBackToFront() const
{
    return m_children;
}

void Layout::SortWidgets()
{
    m_frontToBack = m_children;
    std::reverse(m_frontToBack.begin(), m_frontToBack.end());
}

const WidgetList& Layout::GetChildren() const
{
    return m_children;
}

void Layout::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    if (theme.GetBool(b_debugShowLayout))
    {
        if (m_layoutType == LayoutType::Horizontal)
        {
            canvas.FillRect(ToWorldRect(m_rect), glm::vec4(1.0f, 0.2f, 0.2f, 1.0f));
            canvas.FillRect(ToWorldRect(m_innerRect), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f));
        }
        else
        {
            canvas.FillRect(ToWorldRect(m_rect), glm::vec4(0.2f, 0.2f, 1.0f, 1.0f));
            canvas.FillRect(ToWorldRect(m_innerRect), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        }
    }
    for (auto& child : GetLayout()->GetBackToFront())
    {
        child->Draw(canvas);
    }
}

Layout* Layout::GetLayout()
{
    return this;
}

void Layout::SetContentsMargins(const glm::vec4& contentsMargins)
{
    m_contentsMargins = contentsMargins;
}

const glm::vec4& Layout::GetContentsMargins() const
{
    return m_contentsMargins;
}

LayoutType Layout::GetLayoutType() const
{
    return m_layoutType;
}

void Layout::SetSpacing(float val)
{
    m_spacing = val;
}

void Layout::GetChildrenSizeHint(SizeHint& hint) const
{
    for (auto& spChild : m_children)
    {
        if (auto pLayout = dynamic_cast<Layout*>(spChild.get()))
        {
            pLayout->GetChildrenSizeHint(hint);
        }
        else
        {
            hint.hint = glm::max(spChild->GetSizeHint(), hint.hint);
        }
    }
}

// Find the range of sizes for the child layout
glm::vec4 Layout::GetChildrenMinMaxSize() const
{
    auto minMax = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    for (auto& spChild : m_children)
    {
        glm::vec4 childMinMax;
        if (auto pLayout = dynamic_cast<Layout*>(spChild.get()))
        {
            childMinMax = pLayout->GetChildrenMinMaxSize();
        }
        else
        {
            childMinMax = spChild->GetMinMaxSize();
        }
        minMax += childMinMax;
    }
    return minMax;
}

}