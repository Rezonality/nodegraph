#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/slider.h>

namespace NodeGraph {

Slider::Slider(const std::string& label, const SliderCB& fn)
    : Widget(label)
    , m_callback(fn)
{
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
    m_callback(SliderParams::Step, SliderOp::Get, val);
    rc.SetSize(glm::vec2(val.f * rc.Size().x, rc.Size().y));
    rc.Adjust(thumbPad, thumbPad, -thumbPad, -thumbPad);

    rc = DrawSlab(canvas,
        rc,
        theme.GetFloat(s_sliderThumbRadius),
        theme.GetFloat(s_sliderThumbShadowSize),
        theme.GetVec4f(c_sliderThumbShadowColor),
        0.0f,
        glm::vec4(0.0f),
        theme.GetVec4f(c_sliderThumbColor));


    canvas.FillRoundedRect(rc, theme.GetFloat(s_sliderThumbRadius), theme.GetVec4f(c_sliderThumbColor));

    // Text
    canvas.Text(glm::vec2(titlePanelRect.Left(), titlePanelRect.Center().y), fontSize, glm::vec4(.9f, 0.9f, 0.9f, 1.0f), m_label.c_str(), nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_LEFT);

    for (auto& child : GetBackToFront())
    {
        child->Draw(canvas);
    }
}

void Slider::MouseDown(const CanvasInputState& input)
{
    if (input.buttonClicked[0])
    {
        m_capture = true;
    }
}

void Slider::MouseUp(const CanvasInputState& input)
{
    m_capture = false;
}

bool Slider::MouseMove(const CanvasInputState& input)
{
    // Only move top level
    if (m_capture)
    {
        //        m_rect.Adjust(input.worldMoveDelta);
        return true;
    }
    return false;
}

}
