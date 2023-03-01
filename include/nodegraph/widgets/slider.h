#pragma once
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

class Canvas;
class Slider : public Widget 
{
public:
    Slider(const std::string& label);
    virtual void Draw(Canvas& canvas) override;
    virtual void MouseDown(const CanvasInputState& input) override;
    virtual void MouseUp(const CanvasInputState& input) override;
    virtual bool MouseMove(const CanvasInputState& input) override;
};

}
