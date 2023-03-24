#pragma once
#include <functional>
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

class Canvas;
class Knob;

enum class KnobOp
{
    Get,
    Set
};

enum class KnobType
{
    Standard,
    Mini
};

struct KnobValue
{
    //float step = 0.25f;
    float step = 0.01f;
    float value = 0.0f;
    std::string name;
    std::string tip;
    KnobType type = KnobType::Standard;
};

struct IKnobCB
{
    virtual void UpdateKnob(Knob* pKnob, KnobOp op, KnobValue& val) = 0;
};

class Knob : public Widget, public IKnobCB
{
public:
    Knob(const std::string& label, IKnobCB* pCB = nullptr);
    virtual void Draw(Canvas& canvas) override;
    virtual Widget* MouseDown(CanvasInputState& input) override;
    virtual void MouseUp(CanvasInputState& input) override;
    virtual bool MouseMove(CanvasInputState& input) override;

    // IKnobCB
    virtual void UpdateKnob(Knob* pSlider, KnobOp op, KnobValue& val) override;

    // Internal
    virtual void ClampNormalized(KnobValue& value);
    virtual void Update(CanvasInputState& input);
    //virtual float ThumbWorldSize(Canvas& canvas, float width) const;

private:
    IKnobCB* m_pCB = nullptr;
    KnobValue m_value;
};

}
