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

struct SliderValue
{
    //float step = 0.25f;
    float step = 0.01f;
    float value = 0.0f;
    std::string name;
    std::string tip;
    SliderType type = SliderType::Magnitude;
};

struct ISliderCB
{
    virtual void UpdateSlider(Slider* pSlider, SliderOp op, SliderValue& val) = 0;
};

class Slider : public Widget, public ISliderCB
{
public:
    Slider(const std::string& label, ISliderCB* pCB = nullptr);
    virtual void Draw(Canvas& canvas) override;
    virtual Widget* MouseDown(CanvasInputState& input) override;
    virtual void MouseUp(CanvasInputState& input) override;
    virtual bool MouseMove(CanvasInputState& input) override;

    // ISliderCB
    virtual void UpdateSlider(Slider* pSlider, SliderOp op, SliderValue& val) override;

    // Internal
    virtual void ClampNormalized(SliderValue& value);
    virtual void Update(CanvasInputState& input);
    virtual float ThumbWorldSize(Canvas& canvas, float width) const;

private:
    ISliderCB* m_pCB = nullptr;
    SliderValue m_value;
    NRectf m_sliderRangeArea;
};

}
