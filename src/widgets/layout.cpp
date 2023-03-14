#include <algorithm>
#include <nodegraph/widgets/layout.h>

namespace NodeGraph {

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
                totalFixedSize += widgetPad.y + widgetPad.w;
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
                totalFixedSize += widgetPad.x + widgetPad.z;
                variableCount++;
            }
            maxSizeMinorAxis = std::max(maxSizeMinorAxis, widgetRect.Height() + widgetPad.y + widgetPad.w);
            break;
        }
    }

    // The rectangle for our layout contains all widgets plus our own padding
    // The layout rectangle grows in the minor axis but not the major one; it is fixed at whatever size it was when it was stacked
    float currentPos = 0;
    auto contentMargins = GetPadding();
    switch (m_layoutType)
    {
    case LayoutType::Vertical:
        // Set size of owner
        m_rect.SetSize(maxSizeMinorAxis + contentMargins.x + contentMargins.z, m_rect.Height());
        currentPos = contentMargins.y;
        availableSize = layoutRect.Height() - totalFixedSize - contentMargins.y - contentMargins.w;
        break;
    case LayoutType::Horizontal:
        m_rect.SetSize(m_rect.Width(), maxSizeMinorAxis + contentMargins.y + contentMargins.w);
        currentPos = contentMargins.x;
        availableSize = layoutRect.Width() - totalFixedSize - contentMargins.x - contentMargins.z;
        break;
    }
        
    for (auto& pWidget : m_children)
    {
        glm::uvec2 constraints = pWidget->GetConstraints();
        NRectf widgetRect = pWidget->GetRect();
        glm::vec4 widgetPad = pWidget->GetPadding();
        float variableSize = (availableSize / variableCount);

        switch (m_layoutType)
        {
        case LayoutType::Vertical:
            widgetRect.Move(glm::vec2(contentMargins.x + widgetPad.x, currentPos + widgetPad.y));
            if (constraints.y == LayoutConstraint::Expanding)
            {
                widgetRect.SetSize(widgetRect.Width(), variableSize - widgetPad.y - widgetPad.w);
                variableSize -= widgetRect.Height() - widgetPad.y - widgetPad.w;
                variableCount--;
            }
            currentPos = widgetRect.Bottom() + widgetPad.w;
            break;
        case LayoutType::Horizontal:
            widgetRect.Move(glm::vec2(currentPos + widgetPad.x, contentMargins.y + widgetPad.y));
            if (constraints.x == LayoutConstraint::Expanding)
            {
                widgetRect.SetSize(variableSize - widgetPad.x - widgetPad.z, widgetRect.Height());
                variableSize -= widgetRect.Width() - widgetPad.x - widgetPad.z;
                variableCount--;
            }
            currentPos = widgetRect.Right() + widgetPad.z;
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

}