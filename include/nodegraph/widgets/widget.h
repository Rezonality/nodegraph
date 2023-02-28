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

struct IWidget
{
    virtual const NRectf& GetRect() const = 0;
    virtual void SetRect(const NRectf& sz) = 0;
    virtual void Draw(Canvas& canvas) = 0;
    virtual const std::vector<std::shared_ptr<IWidget>>& GetChildren() const = 0;
    virtual void AddChild(std::shared_ptr<IWidget> spWidget) = 0;

    virtual void MouseDown(const CanvasInputState& input) = 0;
    virtual void MouseUp(const CanvasInputState& input) = 0;
    virtual bool MouseMove(const CanvasInputState& input) = 0;
    
    virtual void SetCapture(bool capture) = 0;
    virtual bool GetCapture() const = 0;
    
    virtual void MoveChildToFront(std::shared_ptr<IWidget> pWidget) = 0;
    virtual void MoveChildToBack(std::shared_ptr<IWidget> pWidget) = 0;
};

class Widget : public IWidget
{
public:
    Widget();
    virtual const NRectf& GetRect() const override;
    virtual void SetRect(const NRectf& sz) override;
    virtual void Draw(Canvas& canvas) override;

    virtual const std::vector<std::shared_ptr<IWidget>>& GetChildren() const override;
    virtual void AddChild(std::shared_ptr<IWidget> spWidget) override;

    virtual void MouseDown(const CanvasInputState& input);
    virtual void MouseUp(const CanvasInputState& input);
    virtual bool MouseMove(const CanvasInputState& input);
    virtual void SetCapture(bool capture);
    virtual bool GetCapture() const;

    virtual void MoveChildToFront(std::shared_ptr<IWidget> pWidget) override;
    virtual void MoveChildToBack(std::shared_ptr<IWidget> pWidget) override;

protected:
    NRectf m_rect;
    std::vector<std::shared_ptr<IWidget>> m_children;
    IWidget* m_pParent = nullptr;
    bool m_capture = false;
};

}