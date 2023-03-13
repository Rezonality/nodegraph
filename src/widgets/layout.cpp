#include <algorithm>
#include <nodegraph/widgets/layout.h>

namespace NodeGraph {

void Layout::Update()
{
    float totalFixedSize = 0;
    float availableSize = 0;
    float maxSize = 0;
    float variableCount = 0;

    NRectf layoutRect = GetRect();
    float layoutSize = 0.0f;

    for (auto& pWidget : m_children) 
    {
        glm::uvec2 constraints = pWidget->GetConstraints();
        NRectf widgetRect = pWidget->GetRect();
        glm::vec4 widgetPad = pWidget->GetPadding();
        uint32_t constraint;

        switch (m_layoutType)
        {
        case LayoutType::Vertical:
            constraint = constraints.y;
            totalFixedSize += (constraint == LayoutConstraint::Fixed ? widgetRect.Height() + widgetPad.y + widgetPad.w : 0.0f);
            maxSize = std::max(maxSize, widgetRect.Width());
            layoutSize = layoutRect.Height();
            break;
        case LayoutType::Horizontal:
            constraint = constraints.x;
            totalFixedSize += (constraint == LayoutConstraint::Fixed ? widgetRect.Width() + widgetPad.x + widgetPad.z : 0.0f);
            maxSize = std::max(maxSize, widgetRect.Height());
            layoutSize = layoutRect.Width();
            break;
        }

        if (constraint == LayoutConstraint::Expanding) 
        {
            variableCount++;
        }
    }
    
    availableSize = layoutSize - totalFixedSize;

    /*
    for (auto& obj : objects) {
        if (isVerticalLayout) {
            obj.width = maxSize;
        } else {
            obj.height = maxSize;
        }
    }

    int currentPos = 0;
    for (auto& obj : objects) {
        if (obj.constraint == FixedSize) {
            if (isVerticalLayout) {
                obj.y = currentPos;
                currentPos += obj.height + spacing;
            } else {
                obj.x = currentPos;
                currentPos += obj.width + spacing;
            }
        } else {
            int variableSize = availableSize / variableCount;
            if (isVerticalLayout) {
                obj.height = variableSize;
                obj.y = currentPos;
                currentPos += variableSize + spacing;
            } else {
                obj.width = variableSize;
                obj.x = currentPos;
                currentPos += variableSize + spacing;
            }
            availableSize -= variableSize;
            variableCount--;
        }
    }
    */
}

void Layout::AddChild(std::shared_ptr<Widget> spWidget)
{
    return AddChildInternal(spWidget);
}

}