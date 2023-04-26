#include <algorithm>
#include <fmt/format.h>
#include <nodegraph/canvas.h>
#include <nodegraph/logger/logger.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/layout.h>
#include <nodegraph/widgets/widget_waveslider.h>

namespace NodeGraph {

WaveSlider::WaveSlider(const std::string& label, ISliderCB* pCB)
    : Slider(label, pCB)
{
}

void WaveSlider::PostDraw(Canvas& canvas, const NRectf& rc)
{
    auto sliderArea = ToWorldRect(rc);
    const float NumWaves = 4.0f;
    const float waveWidth = sliderArea.Width() / float(NumWaves);
    const float instep = 10.0f;
    auto types = std::vector<WaveType>{ WaveType::Triangle, WaveType::Square, WaveType::PWM, WaveType::Saw };

    float fVal = 0.0f;
    canvas.SetLineCap(LineCap::ROUND);
    for (uint32_t index = 0; index < types.size(); index++)
    {
        auto waveType = types[index];

        for (int y = 0; y < 2; y++)
        {
            NRectf waveRect = sliderArea;
            waveRect.SetSize(glm::vec2(waveWidth, waveRect.Height()));
            waveRect.Adjust(waveWidth * index, 0, waveWidth * index, 0);
            waveRect.Adjust(instep, instep, -instep, -instep);

            float colorScale = std::max(0.0f, 1.0f - fabs((fVal * 3.0f) - float(index)));
            colorScale = std::min(1.0f, colorScale);

            float width;
            glm::vec4 color;

            auto waveColor = glm::vec4(1.0f * colorScale, 0.5f * colorScale, 0.0f, 1.0f);
            // Shadow
            if (y == 0)
            {
                width = 7.0f;
                color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
                color = ColorForBackground(waveColor);
            }
            else
            {
                width = 6.0f;
                color = waveColor;
            }

            if (waveType == WaveType::Triangle)
            {
                waveRect.Adjust(0.0f, 0.0f, 0.0f, -4.0f);
                auto start = glm::vec2(waveRect.Left(), waveRect.Center().y);
                auto end = glm::vec2(waveRect.Right(), waveRect.Center().y);
                canvas.FilledCircle(start, width * .5f, color);
                canvas.BeginStroke(start, width, color);
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .25f, waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .5f, waveRect.Bottom()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .75f, waveRect.Top()));
                canvas.LineTo(end);
                canvas.EndStroke();
                canvas.FilledCircle(end, width * .5f, color);
            }
            else if (waveType == WaveType::Square)
            {
                canvas.BeginStroke(glm::vec2(waveRect.Left(), waveRect.Center().y), width, color);
                canvas.LineTo(glm::vec2(waveRect.Left(), waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .33f, waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .33f, waveRect.Bottom()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .66f, waveRect.Bottom()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .66f, waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width(), waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width(), waveRect.Center().y));
                canvas.EndStroke();
            }
            else if (waveType == WaveType::PWM)
            {
                auto start = glm::vec2(waveRect.Left() + waveRect.Width() * .1f, waveRect.Center().y);
                auto end = glm::vec2(waveRect.Left() + waveRect.Width() * .8f, waveRect.Center().y);
                canvas.FilledCircle(start, width * .5f, color);
                canvas.BeginStroke(start, width, color);
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .1f, waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .3f, waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .3f, waveRect.Bottom()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .6f, waveRect.Bottom()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .6f, waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .8f, waveRect.Top()));
                canvas.LineTo(end);
                canvas.EndStroke();
                canvas.FilledCircle(end, width * .5f, color);
            }
            else if (waveType == WaveType::Saw)
            {
                waveRect.Adjust(0.0f, 0.0f, 0.0f, -4.0f);

                auto start = glm::vec2(waveRect.Left(), waveRect.Center().y);
                auto end = glm::vec2(waveRect.Left() + waveRect.Width(), waveRect.Center().y);
                canvas.FilledCircle(start, width * .5f, color);
                canvas.BeginStroke(start, width, color);
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .25f, waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .25f, waveRect.Bottom()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .75f, waveRect.Top()));
                canvas.LineTo(glm::vec2(waveRect.Left() + waveRect.Width() * .75f, waveRect.Bottom()));
                canvas.LineTo(end);
                canvas.EndStroke();
                canvas.FilledCircle(end, width * .5f, color);
            }
        }
    }
    canvas.SetLineCap(LineCap::BUTT);
}
    
void WaveSlider::DrawGeneratedWave(Canvas& canvas, const NRectf& rc)
{
    canvas.FillRect(ToWorldRect(rc), glm::vec4(0.5f, 0.5, 0.5f, 1.0f));
}

} // Nodegraph
