#pragma once

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

class Widget;
using WidgetList = std::vector<std::shared_ptr<Widget>>;

class Widget
{
public:
    Widget(const std::string& label = std::string());
    virtual const NRectf& GetRect() const;
    virtual void SetRect(const NRectf& sz);
    virtual void Draw(Canvas& canvas);
    virtual Widget* GetParent() const;
    virtual void SetParent(Widget* pParent);

    virtual void AddChild(std::shared_ptr<Widget> spWidget);

    virtual Widget* MouseDown(CanvasInputState& input);
    virtual void MouseUp(CanvasInputState& input);
    virtual bool MouseMove(CanvasInputState& input);

    virtual void MoveChildToFront(std::shared_ptr<Widget> pWidget);
    virtual void MoveChildToBack(std::shared_ptr<Widget> pWidget);

    virtual const WidgetList& GetFrontToBack() const;
    virtual const WidgetList& GetBackToFront() const;

    virtual NRectf ToWorldRect(const NRectf& rc) const;
    virtual NRectf GetWorldRect() const;

    virtual const std::string& GetLabel() const;

    // Draw helpers
    NRectf DrawSlab(Canvas& canvas, const NRectf& rect, float borderRadius, float shadowSize, const glm::vec4& shadowColor, float borderSize, const glm::vec4& borderColor, const glm::vec4& centerColor, const char* pszText = nullptr, float fontPad = 2.0f, const glm::vec4& textColor = glm::vec4(1.0f));

protected:
    void SortWidgets();

protected:
    NRectf m_rect;
    WidgetList m_children;
    WidgetList m_frontToBack;
    Widget* m_pParent = nullptr;
    std::string m_label;
};

}