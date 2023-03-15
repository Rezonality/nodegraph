#include <algorithm>
#include <nodegraph/widgets/layout.h>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>

namespace NodeGraph {

Layout::Layout()
{
    m_padding = glm::vec4(4.0f, 4.0f, 4.0f, 4.0f);
}
void Layout::Update()
{
    float totalFixedSize = 0;
    float availableSize = 0;
    float maxSizeMinorAxis = 0;
    float variableCount = 0;

    NRectf layoutRect = GetRect();

    float layoutSize = 0.0f;

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
        NRectf widgetRect = pWidget->GetRect();
        glm::vec4 widgetPad = pWidget->GetPadding();

        switch (m_layoutType)
        {
        case LayoutType::Vertical:
            if (constraints.y == LayoutConstraint::Fixed)
            {
                totalFixedSize += widgetRect.Height() + widgetPad.y + widgetPad.w;
            }
            else
            {
                //totalFixedSize += widgetPad.y + widgetPad.w;
                variableCount++;
            }
            maxSizeMinorAxis = std::max(maxSizeMinorAxis, widgetRect.Width() + widgetPad.x + widgetPad.z);
            break;
        case LayoutType::Horizontal:
            if (constraints.x == LayoutConstraint::Fixed)
            {
                totalFixedSize += widgetRect.Width() + widgetPad.x + widgetPad.z;
            }
            else
            {
                //totalFixedSize += widgetPad.x + widgetPad.z;
                variableCount++;
            }
            maxSizeMinorAxis = std::max(maxSizeMinorAxis, widgetRect.Height() + widgetPad.y + widgetPad.w);
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
    auto contentMargins = GetPadding();

    switch (m_layoutType)
    {
    case LayoutType::Vertical:
        // Set size of owner
        layoutRect.SetSize(maxSizeMinorAxis + contentMargins.x + contentMargins.z, layoutRect.Height());
        availableSize = layoutRect.Height() - totalFixedSize - contentMargins.y - contentMargins.w;
        break;
    case LayoutType::Horizontal:
        layoutRect.SetSize(layoutRect.Width(), maxSizeMinorAxis + contentMargins.y + contentMargins.w);
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

        glm::uvec2 constraints = pWidget->GetConstraints();
        NRectf widgetRect = pWidget->GetRect();
        glm::vec4 widgetPad = pWidget->GetPadding();
        auto space = (pWidget.get() == pLastWidget ? 0.0f : m_spacing);

        switch (m_layoutType)
        {
        case LayoutType::Vertical:
            widgetRect.Move(glm::vec2(layoutRect.Left() + widgetPad.x, layoutRect.Top() + widgetPad.y));
            if (constraints.y == LayoutConstraint::Expanding)
            {
                widgetRect.SetSize(widgetRect.Width(), expandingWidgetSize - widgetPad.y - widgetPad.w);
            }
            layoutRect.SetTop(widgetRect.Bottom() + widgetPad.w + space);
            break;
        case LayoutType::Horizontal:
            widgetRect.Move(glm::vec2(layoutRect.Left() + widgetPad.x, layoutRect.Top() + widgetPad.y));
            if (constraints.x == LayoutConstraint::Expanding)
            {
                widgetRect.SetSize(expandingWidgetSize - widgetPad.x - widgetPad.z, widgetRect.Height());
            }
            layoutRect.SetLeft(widgetRect.Right() + widgetPad.z + space);
            break;
        }
        pWidget->SetRect(widgetRect);
    }
}

void Layout::SetRect(const NRectf& sz)
{
    Widget::SetRect(sz);
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
        canvas.FillRect(ToWorldRect(m_rect), glm::vec4(1.0f, 0.2f, 0.2f, 1.0f));
        canvas.FillRect(ToWorldRect(m_innerRect), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f));
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

}