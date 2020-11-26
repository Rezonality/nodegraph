#pragma once

#include <cassert>
#include <mutils/math/math.h>

#include <nanovg.h>

#include "nodegraph/view/canvas.h"
#include "nodegraph/model/graph.h"

namespace NodeGraph
{

class CanvasVG : public Canvas
{
public:
    CanvasVG(NVGcontext* vgContext)
        : Canvas()
        , vg(vgContext)
    {
    }

    virtual void Begin(const MUtils::NVec2f& displaySize, const MUtils::NVec4f& clearColor) override;
    virtual void End() override;

    NVGcontext* GetVG() const
    {
        return vg;
    }
    NVGcolor ToNVGColor(const MUtils::NVec4f& val)
    {
        return nvgRGBAf(val.x, val.y, val.z, val.w);
    }
    virtual void FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color) override;
    virtual void FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NVec4f& color) override;
    virtual void FillGradientRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillGradientRoundedRectVarying(const MUtils::NRectf& rc, const MUtils::NVec4f& radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillRect(const MUtils::NRectf& rc, const MUtils::NVec4f& color) override;

    virtual void SetAA(bool set) override;
    virtual void BeginStroke(const MUtils::NVec2f& from, float width, const MUtils::NVec4f& color) override;
    virtual void BeginPath(const MUtils::NVec2f& from, const MUtils::NVec4f& color) override;
    virtual void MoveTo(const MUtils::NVec2f& to) override;
    virtual void LineTo(const MUtils::NVec2f& to) override;
    virtual void ClosePath() override;
    virtual void EndPath() override;
    virtual void EndStroke() override;

    virtual void Text(const MUtils::NVec2f& pos, float size, const MUtils::NVec4f& color, const char* pszText, const char* pszFace = nullptr, uint32_t align = TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER) override;
    virtual MUtils::NRectf TextBounds(const MUtils::NVec2f& pos, float size, const char* pszText) const override;

    virtual void Stroke(const MUtils::NVec2f& from, const MUtils::NVec2f& to, float width, const MUtils::NVec4f& color) override;

    virtual void Arc(const MUtils::NVec2f& pos, float radius, float width, const MUtils::NVec4f& color, float startAngle, float endAngle) override;

    virtual void SetLineCap(LineCap cap) override;

private:
    NVGcontext* vg = nullptr;
};


} // namespace NodeGraph
