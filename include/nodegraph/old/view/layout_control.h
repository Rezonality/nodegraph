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

    virtual NVec4f GetMargin() const { return m_margin; };
    virtual void SetMargin(const NVec4f& margin) { m_margin = margin; }

protected:
    NVec2f m_preferredSize;
    NRectf m_viewRect;
    NVec4f m_margin = NVec4f(4.0f, 4.0f, 4.0f, 4.0f);
};

}

