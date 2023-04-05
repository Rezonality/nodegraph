#pragma once

#include <vector>
#include <nodegraph/math_utils.h>

namespace NodeGraph {

class Canvas;
struct CanvasInputState;

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

class Layout;
class Widget;
using WidgetList = std::vector<std::shared_ptr<Widget>>;

class Widget
{
public:
    Widget(const std::string& label = std::string());
    virtual void Draw(Canvas& canvas);
    virtual Widget* GetParent() const;
    virtual void SetParent(Widget* pParent);

    virtual const NRectf& GetRect() const;
    virtual void SetRect(const NRectf& sz);
    virtual const glm::uvec2& GetConstraints() const;
    virtual void SetConstraints(const glm::uvec2& constraints);
    virtual const glm::vec4& GetPadding() const;
    virtual void SetPadding(const glm::vec4& padding);

    virtual Widget* MouseDown(CanvasInputState& input);
    virtual void MouseUp(CanvasInputState& input);
    virtual bool MouseMove(CanvasInputState& input);

    virtual NRectf ToWorldRect(const NRectf& rc) const;
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
    NRectf DrawSlab(Canvas& canvas, const NRectf& rect, float borderRadius, float shadowSize, const glm::vec4& shadowColor, float borderSize, const glm::vec4& borderColor, const glm::vec4& centerColor, const char* pszText = nullptr, float fontPad = 2.0f, const glm::vec4& textColor = glm::vec4(1.0f));

    virtual glm::vec4 TextColorForBackground(const glm::vec4& color);

protected:
    NRectf m_rect;
    Widget* m_pParent = nullptr;
    std::string m_label;
    glm::uvec2 m_constraints = glm::uvec2(LayoutConstraint::Expanding, LayoutConstraint::Expanding);
    glm::vec4 m_padding = glm::vec4(0.0f);
    std::shared_ptr<Layout> m_spLayout;
    uint64_t m_flags = 0;
    glm::vec2 m_sizeHint = glm::vec2(0.0f);
   
};

}