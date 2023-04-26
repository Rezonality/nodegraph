#pragma once
#include <nodegraph/widgets/widget_slider.h>

namespace NodeGraph {

class WaveSlider : public Slider
{
public:
    enum class WaveType
    {
        Triangle,
        Square,
        PWM,
        Saw
    };

    WaveSlider(const std::string& label, ISliderCB* pCB = nullptr);
    virtual void PostDraw(Canvas& canvas, const NRectf& rc);
    virtual void DrawGeneratedWave(Canvas& canvas, const NRectf& rc);
};

}
