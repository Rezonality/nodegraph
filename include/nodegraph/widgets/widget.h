#pragma once

#include <vector>
#include <functional>
#include <nodegraph/math_utils.h>

namespace NodeGraph {

class Canvas;
struct CanvasInputState;
class TipTimer;

enum MouseButtons
{
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_MIDDLE,
    MOUSE_MAX
};

namespace LayoutConstraint {
enum
{
    Preferred = (1 << 0),
    Expanding = (1 << 1),
};
}

namespace WidgetFlags {
enum
{
    None = 0,
    DoNotLayout = (1 << 0)
};
};

namespace WidgetValueFlags {
enum
{
    None,
    ShowText,
    NoQuantization
};
};

struct WidgetValue
{
    std::string name;
    std::string valueText;
    std::string units = "Hz";
    float value = 0.0f;
    float step = 0.01f;
    uint32_t valueFlags = WidgetValueFlags::ShowText;
};

enum class TipState
{
    Wait,
    On,
    Decay,
    Off
};

class Widget;
class TipTimer
{
public:
    static std::unordered_map<Widget*, TipTimer*> ActiveTips;

    TipTimer(Widget* pOwner, float secondsIn, float secondsOut)
        : m_in(secondsIn)
        , m_out(secondsOut)
        , m_state(TipState::Off)
        , m_pOwner(pOwner)
    {
        m_decayTime = m_out;
    }

    ~TipTimer()
    {
        ActiveTips.erase(m_pOwner);
    }
    TipState Update()
    {
        // Wait or decay over time
        switch (m_state)
        {
        case TipState::Wait:
        {
            if (timer_get_elapsed_seconds(m_time) > m_in)
            {
                m_state = TipState::On;
            }
        }
        break;
        case TipState::Decay:
        {
            if (timer_get_elapsed_seconds(m_time) > m_in)
            {
                m_state = TipState::Off;
                ActiveTips.erase(m_pOwner);
            }
        }
        break;
        }
        return m_state;
    }

    float Alpha() const
    {
        auto elapsed = float(timer_get_elapsed_seconds(m_time));
        if (m_state == TipState::Decay)
        {
            if (elapsed > m_decayTime)
            {
                return 0.0f;
            }
            return ((m_decayTime - elapsed) / m_out);
        }
        else if (m_state == TipState::Wait || m_state == TipState::On)
        {
            if (elapsed > m_in)
            {
                return 1.0f;
            }
            return 1.0f - ((m_in - elapsed) / m_in);
        }
        return 0.0f;
    }

    void Stop()
    {
        // If waiting, reverse to a decay for the remaining alpha
        if (m_state == TipState::Wait)
        {
            auto a = Alpha();
            m_state = TipState::Decay;
            timer_restart(m_time);
            m_decayTime = m_out - (m_out * a);
        }
        else
        {
            // If on, switch to decay
            if (m_state == TipState::On || m_state == TipState::Decay)
            {
                SetState(TipState::Decay);
            }
            else
            {
                // Othwerise directly off
                SetState(TipState::Off);
            }
        }
    }

    void Start()
    {
        if (m_state != TipState::On)
        {
            SetState(TipState::Wait);
        }
    }

    void SetState(TipState s)
    {
        if (s == m_state)
        {
            return;
        }
        m_state = s;
        m_decayTime = m_out;
        timer_restart(m_time);
        Update();

        if (s != TipState::Off)
        {
            ActiveTips[m_pOwner] = this;
        }
        else
        {
            ActiveTips.erase(m_pOwner);
        }
    }

    TipState GetState()
    {
        Update();
        return m_state;
    }

private:
    float m_in = 0.0f;
    float m_out = 0.0f;
    float m_decayTime = 0.0f;
    bool on = false;
    timer m_time;
    TipState m_state = TipState::Wait;
    Widget* m_pOwner;
};

class Layout;
class Widget;
using WidgetList = std::vector<std::shared_ptr<Widget>>;

using fnPostDraw = std::function<void(Canvas& canvas, const NRectf& hintRect)>;
class Widget
{
public:
    Widget(const std::string& label);
    
    virtual void Draw(Canvas& canvas);
    virtual void PostDraw(Canvas& canvas, const NRectf& hintRect);

    virtual Widget* GetParent() const;
    virtual void SetParent(Widget* pParent);
   
    // Post Draw
    virtual void AddPostDrawCB(const fnPostDraw& fnCB);

    virtual const NRectf& GetRect() const;
    virtual void SetRect(const NRectf& sz);
    virtual const glm::uvec2& GetConstraints() const;
    virtual void SetConstraints(const glm::uvec2& constraints);
    virtual const glm::vec4& GetPadding() const;
    virtual void SetPadding(const glm::vec4& padding);

    virtual Widget* MouseDown(CanvasInputState& input);
    virtual void MouseUp(CanvasInputState& input);
    virtual bool MouseMove(CanvasInputState& input);
    virtual Widget* MouseHover(CanvasInputState& input);

    virtual NRectf ToWorldRect(const NRectf& rc) const;
    virtual NRectf ToLocalRect(const NRectf& rc) const;
    virtual NRectf GetWorldRect() const;

    virtual const std::string& GetLabel() const;
    virtual void SetLabel(const char* pszLabel);

    virtual void SetLayout(std::shared_ptr<Layout> spLayout);
    virtual Layout* GetLayout();

    virtual uint64_t GetFlags() const;
    virtual void SetFlags(uint64_t flags);

    // Min and max size for this widget
    virtual glm::vec4 GetMinMaxSize() const;

    // The size this widget would like to be.
    virtual glm::vec2 GetSizeHint() const;

    virtual NRectf GetRectWithPad() const;
    virtual void SetRectWithPad(const NRectf& rc);

    // Draw helpers 
    NRectf DrawSlab(Canvas& canvas, const NRectf& rect, float borderRadius, float shadowSize, const glm::vec4& shadowColor, float borderSize, const glm::vec4& borderColor, const glm::vec4& centerColor, const char* pszText = nullptr, float fontPad = 2.0f, const glm::vec4& textColor = glm::vec4(1.0f), float fontSize = 0.0f, const char* pszFont = nullptr);

    virtual glm::vec4 TextColorForBackground(const glm::vec4& color);

    virtual void DrawTip(Canvas& canvas, const glm::vec2& widgetTopCenter, const WidgetValue& value);

    virtual bool IsMouseOver(Canvas& canvas);
    virtual bool IsMouseHover(Canvas& canvas);
    virtual bool IsMouseCapture(Canvas& canvas);

    TipTimer& GetTipTimer();

    void Visit(const std::function<void(Widget*)>& fnVisit);

protected:
    NRectf m_rect;
    Widget* m_pParent = nullptr;
    std::string m_label;
    glm::uvec2 m_constraints = glm::uvec2(LayoutConstraint::Expanding, LayoutConstraint::Expanding);
    glm::vec4 m_padding = glm::vec4(0.0f);
    std::shared_ptr<Layout> m_spLayout;
    uint64_t m_flags = 0;
    glm::vec2 m_sizeHint = glm::vec2(0.0f);
    fnPostDraw m_postDrawCB;
    TipTimer m_tipTimer;
};

}