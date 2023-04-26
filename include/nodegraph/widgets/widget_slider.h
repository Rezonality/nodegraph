#pragma once
#include <functional>
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

class Canvas;
class Slider;

enum class SliderOp
{
    Get,
    Set
};

enum class SliderType
{
    Mark,
    Magnitude
};

struct SliderValue : WidgetValue
{
    SliderType type = SliderType::Magnitude;
};

struct ISliderCB
{
    virtual void UpdateSlider(Slider* pSlider, SliderOp op, SliderValue& val) = 0;
};

class Slider : public Widget
{
public:
    Slider(const std::string& label, ISliderCB* pCB = nullptr);
    virtual void Draw(Canvas& canvas) override;
    virtual Widget* MouseDown(CanvasInputState& input) override;
    virtual void MouseUp(CanvasInputState& input) override;
    virtual bool MouseMove(CanvasInputState& input) override;

    // Internal
    virtual void ClampNormalized(SliderValue& value);
    virtual void Update(CanvasInputState& input);
    virtual float ThumbWorldSize(Canvas& canvas, float width) const;

    virtual const NRectf& GetSliderRangeArea() const;

private:
    void UpdateSlider(Slider* pSlider, SliderOp op, SliderValue& val);

private:
    ISliderCB* m_pCB = nullptr;
    SliderValue m_value;
    NRectf m_sliderRangeArea;
};

}
