#pragma once

#include <memory>
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

class Widget;

enum class LayoutType
{
    Vertical = 0,
    Horizontal
};

class Layout : public Widget
{
public:
    Layout();
    virtual void Update();
    virtual void AddChild(std::shared_ptr<Widget> spWidget);

    virtual void MoveChildToFront(std::shared_ptr<Widget> pWidget);
    virtual void MoveChildToBack(std::shared_ptr<Widget> pWidget);

    virtual const WidgetList& GetFrontToBack() const;
    virtual const WidgetList& GetBackToFront() const;

    virtual void SortWidgets();

    const WidgetList& GetChildren() const;

    virtual void SetRect(const NRectf& sz) override;

    virtual void Draw(Canvas& canvas) override;

    virtual Layout* GetLayout() override;

    virtual void SetContentsMargins(const glm::vec4& contentsMargins);
    virtual const glm::vec4& GetContentsMargins() const;

private:
    LayoutType m_layoutType = LayoutType::Horizontal;
    WidgetList m_children;
    WidgetList m_frontToBack;
    NRectf m_innerRect;
    float m_spacing = 8.0f;
    glm::vec4 m_contentsMargins = glm::vec4(2.0f);
};

}