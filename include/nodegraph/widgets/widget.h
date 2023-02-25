#pragma once

#include <nodegraph/math_utils.h>

namespace NodeGraph {

struct IWidget
{
    virtual const NRectf& GetSize();
    virtual void SetSize(const NRectf& sz);
    virtual void Draw();
};

}