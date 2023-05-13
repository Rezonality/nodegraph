#include <algorithm>
#include <fmt/format.h>

#include <zest/logger/logger.h>

#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/layout.h>
#include <nodegraph/widgets/widget_text.h>

namespace NodeGraph {

TextLabel::TextLabel(const std::string& label, const std::string& font)
    : Widget(label)
    , m_font(font)
{
}

void TextLabel::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    Widget::Draw(canvas);

    auto rc = GetWorldRect();

    // Draw the background area
    rc = DrawSlab(canvas,
        rc,
        theme.GetFloat(s_sliderBorderRadius),
        theme.GetFloat(s_sliderShadowSize),
        theme.GetVec4f(c_sliderShadowColor),
        theme.GetFloat(s_sliderBorderSize),
        theme.GetVec4f(c_sliderBorderColor),
        theme.GetVec4f(c_sliderCenterColor),
        m_label.c_str(),
        2.0f,
        glm::vec4(1.0f), 
        0.0f,
        m_font.empty() ? nullptr : m_font.c_str());

    for (auto& child : GetLayout()->GetBackToFront())
    {
        child->Draw(canvas);
    }
}

}
