#pragma once

#include <nodegraph/math_utils.h>

namespace NodeGraph {

class Canvas;

struct IWidget
{
    virtual const NRectf& GetRect() const = 0;
    virtual void SetRect(const NRectf& sz) = 0;
    virtual void Draw(Canvas& canvas) = 0;
};

class Widget : public IWidget
{
public:
    Widget();
    virtual const NRectf& GetRect() const override;
    virtual void SetRect(const NRectf& sz) override;
    virtual void Draw(Canvas& canvas) override;

protected:
    NRectf m_rect;
};

}