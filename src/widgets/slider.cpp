#include <fmt/format.h>
#include <algorithm>
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
    titlePanelRect.Adjust(thumbPad, 0.0f, -thumbPad, 0.0f);

    SliderValue val;
    m_pCB->UpdateSlider(this, SliderOp::Get, val);

    rc.SetSize(glm::vec2(val.step * rc.Size().x, rc.Size().y));
    rc.Adjust(thumbPad, thumbPad, -thumbPad, -thumbPad);

    rc = DrawSlab(canvas,
        rc,
        theme.GetFloat(s_sliderThumbRadius),
        theme.GetFloat(s_sliderThumbShadowSize),
        theme.GetVec4f(c_sliderThumbShadowColor),
        0.0f,
        glm::vec4(0.0f),
        theme.GetVec4f(c_sliderThumbColor));

    // Text
    canvas.Text(glm::vec2(titlePanelRect.Left(), titlePanelRect.Center().y), fontSize, glm::vec4(.9f, 0.9f, 0.9f, 1.0f), m_label.c_str(), nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_LEFT);

    for (auto& child : GetBackToFront())
    {
        child->Draw(canvas);
    }
}

void Slider::ClampNormalized(SliderValue& value)
{
    value.value = std::max(0.0f, value.value);
    value.value = std::min(1.0f, value.value);
    
    value.step = std::max(0.001f, value.value);
    value.step = std::min(1.0f, value.value);
}

Widget* Slider::MouseDown(CanvasInputState& input)
{
    if (input.buttonClicked[0])
    {
        return this;
    }
    return nullptr;
}

void Slider::MouseUp(CanvasInputState& input)
{
}

bool Slider::MouseMove(CanvasInputState& input)
{
    // Only move top level
    if (input.m_pMouseCapture == this)
    {
        //        m_rect.Adjust(input.worldMoveDelta);
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
        m_value.tip = fmt::format("{}:{}", m_value.name, std::to_string(m_value.value));
        val = m_value;
    }
}

}
