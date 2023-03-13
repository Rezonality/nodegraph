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
    Layout() {}
    virtual void Update();
    virtual void AddChild(std::shared_ptr<Widget> spWidget);

private:
    LayoutType m_layoutType = LayoutType::Horizontal;
};

}