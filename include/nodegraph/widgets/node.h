#pragma once
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

class Canvas;
class Node : public Widget 
{
public:
    virtual void Draw(Canvas& canvas) override;
    virtual void MouseDown(const CanvasInputState& input) override;
    virtual void MouseUp(const CanvasInputState& input) override;
    virtual bool MouseMove(const CanvasInputState& input) override;
};

}
