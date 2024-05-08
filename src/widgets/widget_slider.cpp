#include <algorithm>
#include <fmt/format.h>

#include <zest/logger/logger.h>

#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/layout.h>
#include <nodegraph/widgets/widget_slider.h>

namespace NodeGraph {

Slider::Slider(const std::string& label, ISliderCB* pCB)
    : Widget(label)
    , m_pCB(pCB)
{
    m_value.name = label;
    m_value.step = 0.01f;
}

float Slider::ThumbWorldSize(Canvas& canvas, float width) const
{
    auto& settings = Zest::GlobalSettingsManager::Instance();
    auto theme = settings.GetCurrentTheme();

    auto thumbPad = settings.GetFloat(theme, s_sliderThumbPad);
    width = std::max(width, 4.0f);
    auto pixelSize = canvas.WorldSizeToPixelSize(std::max(1.0f, width));
    if (pixelSize < 4.0f)
    {
        width = canvas.PixelSizeToWorldSize(4.0f);
    }
    return width;
}

void Slider::Draw(Canvas& canvas)
{
    auto& settings = Zest::GlobalSettingsManager::Instance();
    auto theme = settings.GetCurrentTheme();

    auto rc = GetWorldRect();

    // Draw the background area
    rc = DrawSlab(canvas,
        rc,
        settings.GetFloat(theme, s_sliderBorderRadius),
        settings.GetFloat(theme, s_sliderShadowSize),
        settings.GetVec4f(theme, c_sliderShadowColor),
        settings.GetFloat(theme, s_sliderBorderSize),
        settings.GetVec4f(theme, c_sliderBorderColor),
        settings.GetVec4f(theme, c_sliderCenterColor));

    //canvas.FillRect(rc, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    auto thumbPad = settings.GetFloat(theme, s_sliderThumbPad);
    auto fontSize = rc.Height() - settings.GetFloat(theme, s_sliderFontPad) * 2.0f - thumbPad * 2.0f;
    auto titlePanelRect = rc;

    // Our inside track is inside the thumb pad
    titlePanelRect.Adjust(thumbPad, thumbPad, -thumbPad, -thumbPad);

    SliderValue val;
    if (m_pCB)
    {
        m_pCB->UpdateSlider(this, SliderOp::Get, val);
    }
    else
    {
        UpdateSlider(this, SliderOp::Get, val);
    }

    ClampNormalized(val);

    auto thumbWorldSize = ThumbWorldSize(canvas, val.step * m_sliderRangeArea.Width());
    m_sliderRangeArea = titlePanelRect;
    m_sliderRangeArea.SetSize(m_sliderRangeArea.Width() - thumbWorldSize, m_sliderRangeArea.Height());

    // Thumb
    auto thumbRect = m_sliderRangeArea;

    if (val.type == SliderType::Mark)
    {
        thumbRect.SetSize(glm::vec2(thumbWorldSize, thumbRect.Height())); // Width
        thumbRect.Adjust(val.value * m_sliderRangeArea.Width(), 0.0f); // pos
    }
    else // Mag
    {
        thumbRect.SetSize(glm::vec2(std::max(thumbWorldSize, val.value * m_sliderRangeArea.Width() + thumbWorldSize), thumbRect.Height())); // Width
    }

    DrawSlab(canvas,
        thumbRect,
        settings.GetFloat(theme, s_sliderThumbRadius),
        settings.GetFloat(theme, s_sliderThumbShadowSize),
        settings.GetVec4f(theme, c_sliderThumbShadowColor),
        0.0f,
        glm::vec4(0.0f),
        settings.GetVec4f(theme, c_sliderThumbColor));

    if (val.valueFlags & WidgetValueFlags::ShowText)
    {
        canvas.Text(glm::vec2(titlePanelRect.Left(), titlePanelRect.Center().y), fontSize, TextColorForBackground(settings.GetVec4f(theme, c_sliderCenterColor)), val.name.c_str(), nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_LEFT);

        DrawTip(canvas, glm::vec2(titlePanelRect.Center().x, titlePanelRect.Top()), val);
    }

    for (auto& child : GetLayout()->GetBackToFront())
    {
        child->Draw(canvas);
    }

    PostDraw(canvas, ToLocalRect(titlePanelRect));
}

void Slider::ClampNormalized(SliderValue& value)
{
    value.value = std::max(0.0f, value.value);
    value.value = std::min(1.0f, value.value);

    value.step = std::max(0.001f, value.step);
    value.step = std::min(1.0f, value.step);
}

Widget* Slider::MouseDown(CanvasInputState& input)
{
    if (input.buttonClicked[0])
    {
        Update(input);
        return this;
    }
    return nullptr;
}

void Slider::MouseUp(CanvasInputState& input)
{
}

void Slider::Update(CanvasInputState& input)
{
    SliderValue val;
    if (m_pCB)
    {
        m_pCB->UpdateSlider(this, SliderOp::Get, val);
    }
    else
    {
        UpdateSlider(this, SliderOp::Get, val);
    }

    ClampNormalized(val);

    val.value = (input.worldMousePos.x - m_sliderRangeArea.Left()) / m_sliderRangeArea.Width();

    // Quantize
    if (!(val.valueFlags & WidgetValueFlags::NoQuantization))
    {
        val.value = int(val.value / val.step) * val.step;
    }
    else
    {
        val.value -= (val.step * 0.5f);
    }

    ClampNormalized(val);

    if (m_pCB)
    {
        m_pCB->UpdateSlider(this, SliderOp::Set, val);
    }
    else
    {
        UpdateSlider(this, SliderOp::Set, val);
    }
}

bool Slider::MouseMove(CanvasInputState& input)
{
    // Only move top level
    if (input.m_pMouseCapture == this)
    {
        Update(input);
        return true;
    }
    return false;
}

void Slider::UpdateSlider(Slider* pSlider, SliderOp op, SliderValue& val)
{
    if (op == SliderOp::Set)
    {
        m_value = val;
    }
    else
    {
        m_value.valueText = fmt::format("{:1.2f}", m_value.value);
        val = m_value;
    }
}

const NRectf& Slider::GetSliderRangeArea() const
{
    return m_sliderRangeArea;
}

}
