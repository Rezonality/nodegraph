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

struct SizeHint
{
    glm::vec2 hint = glm::vec2(0.0f);
};

class Layout : public Widget
{
public:
    Layout(LayoutType type);
    virtual void Update();
    virtual void AddChild(std::shared_ptr<Widget> spWidget);

    virtual void MoveChildToFront(std::shared_ptr<Widget> pWidget);
    virtual void MoveChildToBack(std::shared_ptr<Widget> pWidget);

    virtual const WidgetList& GetFrontToBack() const;
    virtual const WidgetList& GetBackToFront() const;

    virtual void SortWidgets();

    const WidgetList& GetChildren() const;

    virtual void SetRect(const NRectf& sz) override;
    virtual NRectf GetRectWithPad() const override;
    virtual void SetRectWithPad(const NRectf& rc) override;

    virtual void Draw(Canvas& canvas) override;

    virtual Layout* GetLayout() override;

    virtual void SetContentsMargins(const glm::vec4& contentsMargins);
    virtual const glm::vec4& GetContentsMargins() const;

    virtual LayoutType GetLayoutType() const;

    virtual void SetSpacing(float val);

    virtual void GetChildrenSizeHint(SizeHint& hint) const;

    virtual glm::vec4 GetChildrenMinMaxSize() const;

    //virtual void Resize(const glm::vec2& size);

protected:
    enum class Axis
    {
        Major,
        Minor
    };

    enum class AxisOp
    {
        Set,
        Include
    };

    virtual void SetAxis(Axis axis, AxisOp op, NRectf& val, float update);
    virtual float GetAxis(Axis axis, const NRectf& val) const;
    virtual int GetAxisIndex(Axis axis) const;

private:
    LayoutType m_layoutType = LayoutType::Horizontal;
    WidgetList m_children;
    WidgetList m_frontToBack;
    NRectf m_innerRect;
    float m_spacing = 8.0f;
    glm::vec4 m_contentsMargins = glm::vec4(2.0f);
};

}