#pragma once
#include <functional>
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

enum class SliderParams
{
    Step,
    Value,
    Name,
    Tip
};

enum class SliderOp
{
    Get,
    Set
};

struct SliderValue
{
    float f = 0.0f;
    std::string s;
};

using SliderCB = std::function<void(SliderParams, SliderOp, SliderValue& val)>;
class Canvas;
class Slider : public Widget 
{
public:
    Slider(const std::string& label, const SliderCB& cb);
    virtual void Draw(Canvas& canvas) override;
    virtual void MouseDown(const CanvasInputState& input) override;
    virtual void MouseUp(const CanvasInputState& input) override;
    virtual bool MouseMove(const CanvasInputState& input) override;

private:
    SliderCB m_callback;
};

}
