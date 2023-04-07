#include <algorithm>
#include <fmt/format.h>
#include <nodegraph/logger/logger.h>
#include <nodegraph/canvas.h>
#include <nodegraph/theme.h>
#include <nodegraph/widgets/layout.h>
#include <nodegraph/widgets/knob.h>

namespace NodeGraph {

Knob::Knob(const std::string& label, IKnobCB* pCB)
    : Widget(label)
    , m_pCB(pCB)
{
    m_value.name = label;
    if (!m_pCB)
    {
        m_pCB = this;
    }
    m_value.step = 0.2f;
}

void Knob::Draw(Canvas& canvas)
{
    auto& theme = ThemeManager::Instance();
    Widget::Draw(canvas);

    auto rc = GetWorldRect();

    if (theme.GetBool(b_debugShowLayout))
    {
        DrawSlab(canvas,
            rc,
            0.0f,
            0.0f,
            glm::vec4(0.0f),
            0.0f,
            glm::vec4(0.0f),
            glm::vec4(0.2f, 0.4f, 0.6f, 1.0f));
    }

    KnobValue val;
    m_pCB->UpdateKnob(this, KnobOp::Get, val);

    auto textSize = theme.GetFloat(s_knobTextSize);
    auto pack = theme.GetFloat(s_knobTextInset);

    NRectf remain = rc.Adjusted(glm::vec4(0.0f, 0.0f, 0.0f, -(textSize - pack)));
    NRectf textRect = rc.Adjusted(glm::vec4(0.0f, rc.Height() - textSize, 0.0f, -pack));

    auto knobSize = remain.ShortSide();

    auto knobRegion = NRectf(remain.Center() - glm::vec2(knobSize * .5f), remain.Center() + glm::vec2(knobSize * .5f));
    knobSize *= 0.5f;
    auto markerInset = knobSize * .25f;
    auto arcOffset = -270.0f;

    // Degree ranges for the ImGui origin, which is 90 degrees from top
    auto startArc = 55.0f;
    auto endArc = 360.0f - 55.0f;

    // The full range
    float arcRange = (endArc - startArc);

    // Figure out where the position is on the arc
    float ratioPos = val.value;
    auto posArc = startArc + arcRange * ratioPos;

    // Figure out where the origin is on the arc
    float ratioOrigin = 0.0f;
    auto posArcBegin = startArc + arcRange * ratioOrigin;

    auto channelWidth = theme.GetFloat(s_knobChannelWidth);
    auto channelGap = theme.GetFloat(s_knobChannelGap);

    auto innerSize = knobSize - channelWidth - channelGap;

    auto shadowColor = theme.GetVec4f(c_knobShadowColor);

    // Knob surrounding shadow; a filled circle behind it
    canvas.FilledCircle(knobRegion.Center(), innerSize, shadowColor);

    innerSize -= theme.GetFloat(s_knobShadowSize);

    auto color = theme.GetVec4f(c_knobFillColor);
    auto colorHL = theme.GetVec4f(c_knobFillHLColor);
    auto markColor = theme.GetVec4f(c_knobMarkColor);
    auto markHLColor = theme.GetVec4f(c_knobMarkHLColor);
    auto channelHLColor = theme.GetVec4f(c_knobChannelHLColor);
    auto channelColor = theme.GetVec4f(c_knobChannelColor);
    
    auto shadowSize = theme.GetFloat(s_knobShadowSize);

    if (val.flags & KnobFlags::ReadOnly)
    {
        color.w = .6f;
        colorHL.w = .6f;
        markColor.w = .6f;
        channelHLColor.w = .6f;
    }
    /*
    else if (hover || captured)
    {
        markColor = markHLColor;
        color = theme.Get(color_controlFillColorHL);
    }
    */

    // Only draw the actual knob if big enough
    if (!m_mini)
    {
        canvas.FilledGradientCircle(knobRegion.Center(), innerSize, NRectf(knobRegion.Center().x, knobRegion.Center().y - innerSize, 0, innerSize * 1.5f), colorHL, color);

        // the notch on the button/indicator
        auto markerAngle = degToRad(posArc + arcOffset);
        auto markVector = glm::vec2(std::cos(markerAngle), std::sin(markerAngle));
        canvas.Stroke(knobRegion.Center() + markVector * (markerInset - shadowSize), knobRegion.Center() + markVector * (innerSize - shadowSize), channelWidth, shadowColor);
        canvas.Stroke(knobRegion.Center() + markVector * markerInset, knobRegion.Center() + markVector * (innerSize - shadowSize * 2), channelWidth - shadowSize, markColor);
    }
    else
    {
        float size = knobSize - channelWidth;
        canvas.FilledGradientCircle(knobRegion.Center(), size, NRectf(knobRegion.Center().x, knobRegion.Center().y - size, 0, size * 1.5f), colorHL, color);
    }

    canvas.Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelColor, startArc + arcOffset, endArc + arcOffset);

    // Cover the shortest arc between the 2 points
    if (posArcBegin > posArc)
    {
        std::swap(posArcBegin, posArc);
    }

    canvas.Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHLColor, posArcBegin + arcOffset, posArc + arcOffset);

    canvas.Text(textRect.Center(), textSize, TextColorForBackground(theme.GetVec4f(c_knobFillColor)), val.name.c_str(), nullptr, TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER);
    //canvas.Text()
    /*
    if (fCurrentVal > (fMax + std::numeric_limits<float>::epsilon()))
    {
        m_spCanvas->Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHighColor, endArc - 10 + arcOffset, endArc + arcOffset);
    }
    else if (fCurrentVal < (fMin - std::numeric_limits<float>::epsilon()))
    {
        m_spCanvas->Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHighColor, startArc + arcOffset, startArc + 10 + arcOffset);
    }
    */

    DrawTip(canvas, glm::vec2(knobRegion.Center().x, knobRegion.Top()), val.tip);

    for (auto& child : GetLayout()->GetBackToFront())
    {
        child->Draw(canvas);
    }
}

void Knob::ClampNormalized(KnobValue& value)
{
    value.value = std::max(0.0f, value.value);
    value.value = std::min(1.0f, value.value);

    value.step = std::max(0.001f, value.step);
    value.step = std::min(1.0f, value.step);
}

Widget* Knob::MouseDown(CanvasInputState& input)
{
    if (input.buttonClicked[0])
    {
        m_startValue = m_value.value;
        Update(input);
        return this;
    }
    return nullptr;
}

void Knob::MouseUp(CanvasInputState& input)
{
}

void Knob::Update(CanvasInputState& input)
{
    KnobValue val;
    m_pCB->UpdateKnob(this, KnobOp::Get, val);

    ClampNormalized(val);

    auto dragDistanceWorld = 200.0f;

    val.value = m_startValue + (-input.worldDragDelta.y / dragDistanceWorld);

    // Quantize
    val.value = int(val.value / val.step) * val.step;

    ClampNormalized(val);

    m_pCB->UpdateKnob(this, KnobOp::Set, val);
}

bool Knob::MouseMove(CanvasInputState& input)
{
    // Only move top level
    if (input.m_pMouseCapture == this)
    {
        Update(input);
        return true;
    }
    return false;
}

void Knob::UpdateKnob(Knob* pKnob, KnobOp op, KnobValue& val)
{
    if (op == KnobOp::Set)
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
