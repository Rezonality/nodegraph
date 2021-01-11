#pragma once

#include <mutils/math/math.h>

namespace MUtils
{

class LayoutControl
{
public:
    virtual NVec2f GetPreferredSize() { return m_preferredSize; }
    virtual void SetPreferredSize(const NVec2f& preferred) { m_preferredSize = preferred; }

    virtual NRectf GetViewRect() { return m_viewRect; }
    virtual void SetViewRect(const NRectf& rc) { m_viewRect = rc; }

    virtual NVec4f GetPadding() const { return m_padding; };
    virtual void SetPadding(const NVec4f& padding) { m_padding = padding; }

protected:
    NVec2f m_preferredSize;
    NRectf m_viewRect;
    NVec4f m_padding = NVec4f(0.0f);//4.0f, 4.0f, 4.0f, 4.0f);
};

}

