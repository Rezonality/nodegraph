#include <algorithm>
#include <nodegraph/widgets/layout.h>
#include <nodegraph/canvas.h>

namespace NodeGraph {

Layout::Layout()
{
    m_padding = glm::vec4(8.0f);
}
void Layout::Update()
{
    float totalFixedSize = 0;
    float availableSize = 0;
    float maxSizeMinorAxis = 0;
    float variableCount = 0;

    NRectf layoutRect = GetRect();

    float layoutSize = 0.0f;
 
    for (auto& pWidget : m_children)
    {
        if (pWidget->GetFlags() & WidgetFlags::DoNotLayout)
        {
            continue; 
        }
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
    m_innerRect = layoutRect.Adjusted(m_rect.TopLeft());
        
    for (auto& pWidget : m_children)
    {
        if (pWidget->GetFlags() & WidgetFlags::DoNotLayout)
        {
            continue; 
        }

        glm::uvec2 constraints = pWidget->GetConstraints();
        NRectf widgetRect = pWidget->GetRect();
        glm::vec4 widgetPad = pWidget->GetPadding();
        float variableSize = (availableSize / variableCount);

        switch (m_layoutType)
        {
        case LayoutType::Vertical:
            widgetRect.Move(glm::vec2(layoutRect.Left() + widgetPad.x, layoutRect.Top() + widgetPad.y));
            if (constraints.y == LayoutConstraint::Expanding)
            {
                widgetRect.SetSize(widgetRect.Width(), variableSize - widgetPad.y - widgetPad.w);
                availableSize -= widgetRect.Height() - widgetPad.y - widgetPad.w;
                variableCount--;
            }
            layoutRect.SetTop(widgetRect.Bottom() + widgetPad.w);
            break;
        case LayoutType::Horizontal:
            widgetRect.Move(glm::vec2(layoutRect.Left() + widgetPad.x, layoutRect.Top() + widgetPad.y));
            if (constraints.x == LayoutConstraint::Expanding)
            {
                widgetRect.SetSize(variableSize - widgetPad.x - widgetPad.z, widgetRect.Height());
                availableSize -= (widgetRect.Width() + widgetPad.x + widgetPad.z);
                variableCount--;
            }
            layoutRect.SetLeft(widgetRect.Right() + widgetPad.z);
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
    canvas.FillRect(ToWorldRect(m_rect), glm::vec4(1.0f, 0.2f, 0.2f, 1.0f));
    canvas.FillRect(ToWorldRect(m_innerRect), glm::vec4(0.2f, 1.0f, 0.2f, 1.0f));
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