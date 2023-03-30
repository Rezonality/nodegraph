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
    m_value.step = 0.01f;
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

    canvas.FilledCircle(rc.Center(), rc.ShortSide() * .5f - 2.0f, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));

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

    //val.value = (input.worldMousePos.x - m_knobRangeArea.Left()) / m_knobRangeArea.Width();

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
