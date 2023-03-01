#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/slider.h>

namespace NodeGraph {

Slider::Slider(const std::string& label)
    : Widget(label)
{

}

void Slider::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    Widget::Draw(canvas);

    auto rc = GetWorldRect();


    // Border rectangle
    canvas.FillRoundedRect(rc, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_nodeBorderColor));

    // Center color
    auto borderSize = theme.GetFloat(s_nodeBorderSize) * 2.0f;
    rc.Adjust(borderSize, borderSize, -borderSize, -borderSize);
    canvas.FillRoundedRect(rc, theme.GetFloat(s_nodeBorderRadius), theme.GetVec4f(c_nodeBackground));

    auto fontSize = rc.Height() - theme.GetFloat(s_nodeTitleFontPad);
    auto titlePanelRect = rc;


    rc.SetSize(glm::vec2(20.0f, rc.Size().y));

    canvas.FillRoundedRect(rc, theme.GetFloat(s_nodeBorderRadius), glm::vec4(0.8f, 0.8f, 0.2f, 1.0f));

    // Text
    canvas.Text(glm::vec2(titlePanelRect.Left(), titlePanelRect.Center().y), fontSize, glm::vec4(.1f, 0.1f, 0.1f, 1.0f), m_label.c_str(), nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_LEFT);

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
