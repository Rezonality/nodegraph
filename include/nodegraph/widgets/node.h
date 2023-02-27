#pragma once
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

class Canvas;
class Node : public Widget 
{
public:
    virtual void Draw(Canvas& canvas) override;
    virtual IWidget* MouseDown(const glm::vec2& pos, MouseButton button = MouseButton::Left)
    {
        return nullptr;
    }
    virtual void MouseUp(const glm::vec2& pos, MouseButton button = MouseButton::Left) {}
    virtual void MouseMove(const glm::vec2& pos) {}
};

}
