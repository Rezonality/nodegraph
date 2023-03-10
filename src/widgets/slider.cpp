#include <algorithm>
#include <fmt/format.h>
#include <nodegraph/logger/logger.h>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/slider.h>

namespace NodeGraph {

Slider::Slider(const std::string& label, ISliderCB* pCB)
    : Widget(label)
    , m_pCB(pCB)
{
    m_value.name = label;
    if (!m_pCB)
    {
        m_pCB = this;
    }
    m_value.step = 0.01f;
}
    
float Slider::ThumbWorldSize(Canvas& canvas, float width) const
{
    auto& theme = ThemeManager::Instance();
    auto thumbPad = theme.GetFloat(s_sliderThumbPad);
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
    auto& theme = ThemeManager::Instance();
    Widget::Draw(canvas);

    auto rc = GetWorldRect();

    rc = DrawSlab(canvas,
        rc,
        theme.GetFloat(s_sliderBorderRadius),
        theme.GetFloat(s_sliderShadowSize),
        theme.GetVec4f(c_sliderShadowColor),
        theme.GetFloat(s_sliderBorderSize),
        theme.GetVec4f(c_sliderBorderColor),
        theme.GetVec4f(c_sliderCenterColor));

    auto thumbPad = theme.GetFloat(s_sliderThumbPad);
    auto fontSize = rc.Height() - theme.GetFloat(s_sliderFontPad) * 2.0f - thumbPad * 2.0f;
    auto titlePanelRect = rc;

    // Our inside track is inside the thumb pad
    titlePanelRect.Adjust(thumbPad, thumbPad, -thumbPad, -thumbPad);

    SliderValue val;
    m_pCB->UpdateSlider(this, SliderOp::Get, val);

    ClampNormalized(val);

    auto thumbWorldSize = ThumbWorldSize(canvas, val.step * m_sliderRange.Width());
    m_sliderRange = titlePanelRect;
    m_sliderRange.SetSize(m_sliderRange.Width() - thumbWorldSize, m_sliderRange.Height());

    // Thumb
    auto thumbRect = m_sliderRange;

    if (val.type == SliderType::Mark)
    {
        thumbRect.SetSize(glm::vec2(thumbWorldSize, thumbRect.Height())); // Width
        thumbRect.Adjust(val.value * m_sliderRange.Width(), 0.0f); // pos
    }
    else // Mag 
    {
        thumbRect.SetSize(glm::vec2(std::max(thumbWorldSize, val.value * m_sliderRange.Width() + thumbWorldSize), thumbRect.Height())); // Width
    }

    DrawSlab(canvas,
        thumbRect,
        theme.GetFloat(s_sliderThumbRadius),
        theme.GetFloat(s_sliderThumbShadowSize),
        theme.GetVec4f(c_sliderThumbShadowColor),
        0.0f,
        glm::vec4(0.0f),
        theme.GetVec4f(c_sliderThumbColor));

    // Text
    canvas.Text(glm::vec2(titlePanelRect.Left(), titlePanelRect.Center().y), fontSize, glm::vec4(.9f, 0.9f, 0.9f, 1.0f), m_label.c_str(), nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_LEFT);

    if (canvas.GetInputState().m_pMouseCapture == this)
    {
        titlePanelRect.Adjust(0.0f, -m_rect.Height(), 0.0f, 0.0f);

        auto tipPad = theme.GetFloat(s_sliderTipFontPad);
        auto fontSize = theme.GetFloat(s_sliderTipFontSize);

        auto rcBounds = canvas.TextBounds(titlePanelRect.Center(), fontSize, val.tip.c_str(), nullptr, TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE);

        titlePanelRect.SetSize(rcBounds.Width() + tipPad * 2.0f, fontSize + tipPad * 2.0f);

        float szHalf = titlePanelRect.Width() / 2.0f;
        titlePanelRect.Move(rc.Center().x - szHalf, titlePanelRect.Top());

        rc = DrawSlab(canvas,
            titlePanelRect,
            theme.GetFloat(s_sliderTipBorderRadius),
            theme.GetFloat(s_sliderTipShadowSize),
            theme.GetVec4f(c_sliderTipShadowColor),
            theme.GetFloat(s_sliderTipBorderSize),
            theme.GetVec4f(c_sliderTipBorderColor),
            theme.GetVec4f(c_sliderTipCenterColor));

        canvas.Text(glm::vec2(titlePanelRect.Center().x, titlePanelRect.Center().y), fontSize, theme.GetVec4f(c_sliderTipFontColor), val.tip.c_str(), nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER);
    }

    for (auto& child : GetBackToFront())
    {
        child->Draw(canvas);
    }
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
    m_pCB->UpdateSlider(this, SliderOp::Get, val);

    ClampNormalized(val);

    val.value = (input.worldMousePos.x - m_sliderRange.Left()) / m_sliderRange.Width();

    // Quantize
    val.value = int(val.value / val.step) * val.step;

    ClampNormalized(val);

    m_pCB->UpdateSlider(this, SliderOp::Set, val);
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
        m_value.tip = fmt::format("{}:{:1.2f}", m_value.name, m_value.value);
        val = m_value;
    }
}

}
