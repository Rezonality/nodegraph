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
    Widget();
    virtual const NRectf& GetRect() const;
    virtual void SetRect(const NRectf& sz);
    virtual void Draw(Canvas& canvas);
    virtual Widget* GetParent() const;
    virtual void SetParent(Widget* pParent);

    virtual void AddChild(std::shared_ptr<Widget> spWidget);

    virtual void MouseDown(const CanvasInputState& input);
    virtual void MouseUp(const CanvasInputState& input);
    virtual bool MouseMove(const CanvasInputState& input);
    virtual void SetCapture(bool capture);
    virtual bool GetCapture() const;

    virtual void MoveChildToFront(std::shared_ptr<Widget> pWidget);
    virtual void MoveChildToBack(std::shared_ptr<Widget> pWidget);

    virtual const WidgetList& GetFrontToBack() const;
    virtual const WidgetList& GetBackToFront() const;

    virtual NRectf ToWorldRect(const NRectf& rc);

protected:
    void SortWidgets();

protected:
    NRectf m_rect;
    WidgetList m_children;
    WidgetList m_frontToBack;
    Widget* m_pParent = nullptr;
    bool m_capture = false;
};

}