#pragma once

#include <nodegraph/math_utils.h>

namespace NodeGraph {

class Canvas;
enum class MouseButton
{
    None,
    Left,
    Right
};

struct IWidget
{
    virtual const NRectf& GetRect() const = 0;
    virtual void SetRect(const NRectf& sz) = 0;
    virtual void Draw(Canvas& canvas) = 0;
    virtual const std::vector<std::shared_ptr<IWidget>>& GetChildren() const = 0;
    virtual void AddChild(std::shared_ptr<IWidget> spWidget) = 0;

    virtual IWidget* MouseDown(const glm::vec2& pos, MouseButton button = MouseButton::Left) = 0;
    virtual void MouseUp(const glm::vec2& pos, MouseButton button = MouseButton::Left) = 0;
    virtual void MouseMove(const glm::vec2& pos) = 0;
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

    static IWidget* HandleMouseDown(const glm::vec2& pos, MouseButton button = MouseButton::Left);
    static void HandleMouseUp(const glm::vec2& pos, MouseButton button = MouseButton::Left);
    static void HandleMouseMove(const glm::vec2& pos);

protected:
    NRectf m_rect;
    std::vector<std::shared_ptr<IWidget>> m_children;
    static IWidget* s_pMouseCapture;
    static MouseButton s_buttonDown;
};

}