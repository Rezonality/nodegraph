#include <map>

#include <fmt/format.h>

#include <nanovg.h>

#define DECLARE_NODE_COLORS
#define DECLARE_NODE_STYLES

#include <mutils/logger/logger.h>
#include <mutils/math/math.h>
#include <mutils/ui/colors.h>
#include <mutils/ui/dpi.h>
#include <mutils/ui/style.h>
#include <mutils/ui/theme.h>

#include <nodegraph/view/graphview.h>
#include <nodegraph/view/viewnode.h>

using namespace MUtils;
using namespace MUtils::Theme;
using namespace MUtils::Style;

namespace {

float widget_fontHeight = 28.0f;
float widget_fontHeightSmall = 16.0f;

float knobWidget_miniSize = 75.0f;

// A little daylight
float node_textGap = 1.0f;
float node_shadowSize = 2.0f;
float node_borderPad = 2.0f;
float node_buttonPad = 2.0f;
float node_pinPad = 4.0f;
float node_gridScale = 125.0f;
float node_labelPad = 6.0f;
} // namespace

namespace NodeGraph {

GraphView::GraphView(Graph* pGraph, std::shared_ptr<Canvas> spCanvas)
    : m_pGraph(pGraph)
    , m_spCanvas(spCanvas)
{
    m_spViewData = std::make_shared<GraphViewData>();

    m_spViewData->pendingUpdate = true;

    m_spViewData->connections.push_back(pGraph->sigBeginModify.connect([=](Graph* pGraph) {
        m_spViewData->disabled = true;

        // TODO: Fix Z Order stuff
        //assert(!"This is broken!");
        //m_spViewData->mapWorldToView.clear();
        //m_spViewData->nodeZOrder.clear();
    }));

    m_spViewData->connections.push_back(pGraph->sigEndModify.connect([=](Graph* pGraph) {
        m_spViewData->disabled = false;
        m_spViewData->pendingUpdate = true;
    }));
}

void GraphView::Init()
{
    InitColors();
    InitStyles();
}

void GraphView::InitColors()
{
    auto& theme = ThemeManager::Instance();

    theme.Set(color_nodeBackground, NVec4f(.3f, .3f, .3f, 1.0f));
    theme.Set(color_nodeHoverBackground, NVec4f(.31f, .31f, .31f, 1.0f));
    theme.Set(color_nodeActiveBackground, NVec4f(.32f, .32f, .32f, 1.0f));

    theme.Set(color_nodeTitleColor, NVec4f(1.0f, 1.0f, 1.0f, 1.0f));
    theme.Set(color_nodeTitleBGColor, NVec4f(0.4f, .4f, 0.4f, 1.0f));
    theme.Set(color_nodeButtonTextColor, NVec4f(0.15f, .15f, 0.15f, 1.0f));
    //theme.Set(color_nodeButtonTextColor, NVec4f(.9f, .9f, .9f, 1.0f));
    theme.Set(color_nodeHLColor, NVec4f(0.98f, 0.48f, 0.28f, 1.0f));
    theme.Set(color_nodeShadowColor, NVec4f(0.1f, 0.1f, 0.1f, .75f));

    theme.Set(color_controlShadowColor, NVec4f(0.1f, 0.1f, 0.1f, 1.0f)); // Border/shadow
    theme.Set(color_controlFillColor, NVec4f(.55f, .55f, 0.55f, 1.0f)); // Knob color, button background, slider background
    theme.Set(color_controlFillColorHL, NVec4f(.65f, .65f, 0.65f, 1.0f)); // Highlighted version of above
    theme.Set(color_controlKeyColor1, NVec4f(0.98f, 0.48f, 0.18f, 1.0f)); // Alternate color for slider thumb, knob surround, etc.

    theme.Set(color_flowData, NVec4f(0.8f, 0.6f, 0.3f, 1.0f));
    theme.Set(color_flowControl, theme.Get(color_controlKeyColor1));
}

void GraphView::InitStyles()
{
    auto& style = StyleManager::Instance();

    // For connectors around side
    style.Set(style_nodeOuter, 20.0f);

    float margin = 2.0f;

    // Title and padding
    style.Set(style_nodeTitleHeight, 30.0f);
    style.Set(style_nodePadSize, 24.0f);
    style.Set(style_nodeTitleFontSize, 26.0f);
    style.Set(style_nodeLayoutMargin, NVec4f(margin));
    style.Set(style_nodeBorderRadius, 4.0f);
    style.Set(style_nodeShadowSize, 4.0f);

    style.Set(style_controlTextMargin, 2.0f);
    style.Set(style_controlShadowSize, 2.0f);
}

bool GraphView::ShouldShowNode(Canvas& canvas, const Node* pNode) const
{
    if (pNode->Flags() & NodeFlags::Hidden)
        return false;

    //if (pNode->Flags() & NodeFlags::OwnerDraw)

    // Check for things to show
    /*
    for (auto& in : pNode->GetInputs())
    {
        if (!in->GetViewCells().Empty())
        {
            return true;
        }
    }
    for (auto& in : pNode->GetOutputs())
    {
        if (!in->GetViewCells().Empty())
        {
            return true;
        }
    }
    */

    /*
    for (auto& decorator : pNode->GetDecorators())
    {
        if (!decorator->gridLocation.Empty())
            return true;
    }

    if (!pNode->GetCustomViewCells().Empty())
        return true;
        */

    return true;
}

// Building pending view nodes
void GraphView::BuildNodes()
{
    if (!m_spViewData->pendingUpdate)
    {
        return;
    }

    m_spViewData->pendingUpdate = false;

    // Get all the nodes and add them to our draw map
    const auto& ins = m_pGraph->GetDisplayNodes();
    for (auto& pNode : ins)
    {
        if (m_spViewData->mapNodeToViewNode.find(pNode) == m_spViewData->mapNodeToViewNode.end())
        {
            if (ShouldShowNode(*m_spCanvas, pNode))
            {
                auto spViewNode = std::make_shared<ViewNode>(pNode);

                m_spViewData->mapNodeToViewNode[pNode] = spViewNode;
                m_spViewData->nodeZOrder.push_back(pNode);
                pNode->GetLayout().spRoot->UpdateLayout();
            }
        }
    }
}

bool GraphView::CheckCapture(ViewNode& viewNode, Pin& param, const NRectf& region, bool& hover)
{
    auto pos = m_spCanvas->GetViewMousePos();
    bool overParam = region.Contains(NVec2f(pos.x, pos.y));
    hover = overParam;

    auto const& state = m_spCanvas->GetInputState();
    if (!state.canCapture)
    {
        m_pCaptureNode = nullptr;
        m_pCaptureParam = nullptr;
        return false;
    }

    if (!state.buttonDown[MOUSE_LEFT] && !state.buttonDown[MOUSE_RIGHT])
    {
        m_pCaptureNode = nullptr;
        m_pCaptureParam = nullptr;
        m_spCanvas->GetInputState().captureState = CaptureState::None;
        return false;
    }

    if (m_spCanvas->GetInputState().captureState == CaptureState::MoveNode || m_spCanvas->GetInputState().captureState == CaptureState::MoveCanvas)
    {
        m_pCaptureParam = nullptr;
        return false;
    }

    if (m_pCaptureParam == nullptr)
    {
        // Capturing a new parameter with a click
        if (state.buttonClicked[MOUSE_LEFT])
        {
            if (overParam)
            {
                // Make this the new start state
                m_pCaptureParam = &param;
                m_mouseStart = NVec2f(pos.x, pos.y);
                m_pStartValue = m_pCaptureParam;
            }
        }
    }

    // Already capturing or hovering
    if ((m_pCaptureParam == &param) || (overParam && m_pCaptureParam == nullptr))
    {
        hover = true;
        if (param.GetAttributes().flags & ParameterFlags::ReadOnly)
        {
            m_spCanvas->GetInputState().captureState = CaptureState::Parameter;
            m_pCaptureParam = nullptr;
        }
    }
    else
    {
        hover = false;
    }

    if (m_pCaptureParam)
    {
        m_spCanvas->GetInputState().captureState = CaptureState::Parameter;
    }
    m_hideCursor = m_pCaptureParam != nullptr;

    return m_pCaptureParam == &param;
}

void GraphView::DrawDecorator(NodeDecorator& decorator, const NRectf& rc)
{
    auto theme = Theme::ThemeManager::Instance();
    static const NVec4f colorLabel(0.20f, 0.20f, 0.20f, 1.0f);
    static const NVec4f fontColor(.8f, .8f, .8f, 1.0f);

    if (decorator.type == DecoratorType::Label)
    {
        float fontSize = 28.0f;

        m_spCanvas->Text(rc.Center(), fontSize, fontColor, decorator.strName.c_str());
    }
    else if (decorator.type == DecoratorType::Line)
    {
        auto center = rc.Center();
        m_spCanvas->Stroke(NVec2f(rc.Left(), center.y), NVec2f(rc.Right(), center.y), 2.0f, theme.Get(color_nodeShadowColor));
    }
}

void GraphView::DrawLabel(Parameter& param, const LabelInfo& info)
{
    NVec4f colorLabel(0.25f, 0.25f, 0.25f, 0.95f);
    NVec4f fontColor(.95f, .95f, .95f, 1.0f);
    std::string val;

    if (param.GetType() == ParameterType::Float || param.GetType() == ParameterType::Double)
    {
        // Convert to 100% if necessary
        float fVal = param.To<float>();
        if (param.GetAttributes().displayType == ParameterDisplayType::Percentage && param.GetAttributes().max.To<float>() <= 1.0f)
        {
            fVal *= 100.0f;
            val = std::to_string((int)fVal);
        }
        else
        {
            val = fmt::format("{:.{}f}", fVal, 3);
        }
    }
    else
    {
        val = std::to_string(param.To<int64_t>());
    }

    switch (param.GetAttributes().displayType)
    {
    case ParameterDisplayType::Percentage:
        val += "%";
        break;
    case ParameterDisplayType::Custom:
        val += param.GetAttributes().postFix;
        break;
    case ParameterDisplayType::None:
        return;
    default:
        break;
    }

    if (!info.prefix.empty())
    {
        val = info.prefix + val;
    }

    float fontSize = 28.0f;
    NRectf rcFont = m_spCanvas->TextBounds(info.pos, fontSize, val.c_str());
    rcFont.Adjust(-rcFont.Width() * .5f, -rcFont.Height() * .5f);
    rcFont.Adjust(0, -node_labelPad);

    NRectf rcBounds = rcFont;
    rcBounds.Adjust(-node_labelPad, -node_labelPad, node_labelPad, node_labelPad);
    rcFont.Adjust(-node_labelPad, -node_labelPad, node_labelPad, node_labelPad);

    auto rcShadow = rcBounds;
    rcShadow.Adjust(-node_shadowSize, -node_shadowSize, node_shadowSize, node_shadowSize);

    //m_spCanvas->FillRect(rcShadow, node_shadowColor);
    m_spCanvas->FillRect(rcBounds, colorLabel);

    // Text is centered
    m_spCanvas->Text(rcFont.topLeftPx + rcFont.Size() * .5f, fontSize, fontColor, val.c_str());
}

void GraphView::DrawPin(ViewNode& viewNode, Pin& pin)
{
    auto& attrib = pin.GetAttributes();

    auto rc = pin.GetViewRect();
    rc.Adjust(viewNode.pModelNode->GetPos());

    if (pin.GetAttributes().ui == ParameterUI::Knob)
    {
        DrawKnob(viewNode, pin, rc, false);
    }
    else if (pin.GetAttributes().ui == ParameterUI::Slider)
    {
        DrawSlider(viewNode, pin, rc);
    }
    else if (pin.GetAttributes().ui == ParameterUI::Button)
    {
        DrawButton(viewNode, pin, rc);
    }
}

bool GraphView::DrawKnob(ViewNode& viewNode, Pin& param, NRectf rect, bool miniKnob)
{
    NVec4f colorLabel(0.35f, 0.35f, 0.35f, 1.0f);
    NVec4f channelColor(0.38f, 0.38f, 0.38f, 1.0f);
    NVec4f shadowColor(0.1f, 0.1f, 0.1f, .5f);
    NVec4f channelHLColor(0.98f, 0.48f, 0.28f, 1.0f);
    NVec4f channelHighColor(0.98f, 0.10f, 0.10f, 1.0f);
    NVec4f markColor(.9f, .9f, 0.9f, 1.0f);
    NVec4f markHLColor(1.0f, 1.0f, 1.0f, 1.0f);
    NVec4f fontColor(.95f, .95f, .95f, 1.0f);
    float channelGap = 4;

    auto& style = StyleManager::Instance();
    auto& theme = ThemeManager::Instance();
    auto fontHeight = widget_fontHeight;
    if (rect.Height() < (fontHeight * 2.0f))
    {
        miniKnob = true;
    }
    float channelWidth = miniKnob ? 8.0f : 4.0f;

    // First, make the knob as big as it can be
    auto knobSize = std::min(rect.Width(), rect.Height());

    auto labelFit = fontHeight - channelWidth - channelGap + node_textGap;

    // Then make it smaller to fit the label
    if (!miniKnob && (knobSize + labelFit) > rect.Height())
    {
        // Reduce the knob by the font size.
        knobSize = rect.Height() - labelFit;
    }

    float knobSizeScale = std::max(1.0f, rect.Width() / 100.0f);

    // Radius not diameter
    knobSize *= .5f;

    channelWidth *= knobSizeScale;

    auto& attrib = param.GetAttributes();

    // Normalized value 0->1
    float fCurrentVal = (float)param.Normalized();

    // Knob is always normalized 0->1
    float fMin = 0.0f;
    float fMax = 1.0f;
    float fOrigin = (float)param.NormalizedOrigin();

    // TODO: Figure out delta amount based on canvas size?
    float rangePerDelta = -(1.0f / 300.0f);

    if (param.GetType() == ParameterType::Int64)
    {
        // Make integer steps a bit more fluid
        rangePerDelta /= 2.0f;
    }

    std::string label;
    if (!attrib.labels.empty())
    {
        label = attrib.labels[0];
    }
    else
    {
        label = param.GetName();
    }

    auto knobRegion = NRectf(rect.Center().x - knobSize, !miniKnob ? rect.Top() : rect.Center().y - knobSize, knobSize * 2.0f, knobSize * 2.0f);

    bool hover = false;
    bool captured = false;
    if (param.GetSource() == nullptr)
    {
        captured = CheckCapture(viewNode, param, knobRegion, hover);
        if (captured)
        {
            if (m_pStartValue)
            {
                viewNode.active = true;
                viewNode.hovered = true;

                const auto& attrib = param.GetAttributes();
                auto startValue = m_pStartValue->Normalized();
                auto const& state = m_spCanvas->GetInputState();

                NVec2f d = state.dragDelta;
                if (state.slowDrag)
                {
                    d *= .5f;
                }
                double delta = double(d.y);
                double fStep = param.NormalizedStep();

                if (param.GetType() == ParameterType::Int64)
                {
                    if (fStep == 0.0)
                    {
                        fStep = std::abs(1.0 / (attrib.max.To<double>() - attrib.min.To<double>()));
                        fStep += std::numeric_limits<double>::epsilon();
                    }
                }

                if (std::fabs(delta) > 0)
                {
                    auto fNew = startValue + (delta * rangePerDelta);

                    if (fStep != 0.0)
                    {
                        fNew = std::floor(fNew / fStep) * fStep;
                    }

                    if (fNew != startValue)
                    {
                        param.SetFromNormalized(fNew);
                    }
                }
            }
        }
    }

    // How far the marker notch is from the center of the button
    float markerInset = knobSize * .25f;

    float arcOffset = -270.0f;
    // The degree ranges corrected for NVG origin which is +90 degrees from top
    float startArc = 55.0f;
    float endArc = 360 - 55.0f;

    // The full range
    float arcRange = (endArc - startArc);

    // Figure out where the position is on the arc
    float ratioPos = fabs((std::clamp(fCurrentVal, 0.0f, 1.0f) - fMin) / (fMax - fMin));
    auto posArc = startArc + arcRange * ratioPos;

    // Figure out where the origin is on the arc
    float ratioOrigin = fabs((fOrigin - fMin) / (fMax - fMin));
    auto posArcBegin = startArc + arcRange * ratioOrigin;

    if (m_debugVisuals)
    {
        m_spCanvas->FillRect(rect, NVec4f(.2f, .2f, .4f, 1.0f));
        m_spCanvas->FillRect(knobRegion, NVec4f(.4f, .2f, .2f, 1.0f));
    }

    auto innerSize = knobSize - channelWidth - channelGap;

    // Knob surrounding shadow; a filled circle behind it
    m_spCanvas->FilledCircle(knobRegion.Center(), innerSize, shadowColor);
    innerSize -= node_shadowSize;

    auto color = theme.Get(color_controlFillColor);
    auto colorHL = theme.Get(color_controlFillColorHL);
    if (param.GetAttributes().flags & ParameterFlags::ReadOnly)
    {
        color.w = .6f;
        colorHL.w = .6f;
        markColor.w = .6f;
        channelHLColor.w = .6f;
    }
    else if (hover || captured)
    {
        markColor = markHLColor;
        color = theme.Get(color_controlFillColorHL);
    }

    // Only draw the actual knob if big enough
    if (!miniKnob)
    {
        m_spCanvas->FilledGradientCircle(knobRegion.Center(), innerSize, NRectf(knobRegion.Center().x, knobRegion.Center().y - innerSize, 0, innerSize * 1.5f), colorHL, color);

        // the notch on the button/indicator
        auto markerAngle = degToRad(posArc + arcOffset);
        auto markVector = NVec2f(std::cos(markerAngle), std::sin(markerAngle));
        m_spCanvas->Stroke(knobRegion.Center() + markVector * (markerInset - node_shadowSize), knobRegion.Center() + markVector * (innerSize - node_shadowSize), channelWidth, shadowColor);
        m_spCanvas->Stroke(knobRegion.Center() + markVector * markerInset, knobRegion.Center() + markVector * (innerSize - node_shadowSize * 2), channelWidth - node_shadowSize, markColor);
    }
    else
    {
        float size = knobSize - channelWidth;
        m_spCanvas->FilledGradientCircle(knobRegion.Center(), size, NRectf(knobRegion.Center().x, knobRegion.Center().y - size, 0, size * 1.5f), colorHL, color);
    }

    m_spCanvas->Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelColor, startArc + arcOffset, endArc + arcOffset);

    // Cover the shortest arc between the 2 points
    if (posArcBegin > posArc)
    {
        std::swap(posArcBegin, posArc);
    }

    m_spCanvas->Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHLColor, posArcBegin + arcOffset, posArc + arcOffset);

    if (fCurrentVal > (fMax + std::numeric_limits<float>::epsilon()))
    {
        m_spCanvas->Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHighColor, endArc - 10 + arcOffset, endArc + arcOffset);
    }
    else if (fCurrentVal < (fMin - std::numeric_limits<float>::epsilon()))
    {
        m_spCanvas->Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHighColor, startArc + arcOffset, startArc + 10 + arcOffset);
    }

    // Scale the font to fit and center it
    float bottomOffset = knobRegion.Bottom() + labelFit - fontHeight * .5f;
    auto rcText = m_spCanvas->TextBounds(NVec2f(0.0f, 0.0f), fontHeight, label.c_str());
    if (rcText.Width() > rect.Width())
    {
        auto fontDecrease = (rcText.Width() / rect.Width()) - 1.0f;
        fontDecrease *= fontHeight * .5f;
        auto oldHeight = fontHeight;
        fontHeight -= fontDecrease;
        fontHeight = std::floor(fontHeight);
        bottomOffset += (oldHeight - fontHeight) * .25f;
    }

    auto textPos = NVec2f(rect.Center().x, bottomOffset);
    rcText = m_spCanvas->TextBounds(textPos, fontHeight, label.c_str());

    if (!miniKnob)
    {
        // Text
        if (false && m_debugVisuals)
        {
            auto rcArea = rcText;
            rcArea.Adjust(-rcText.Width() * .5f, -rcText.Height() * .5f);
            m_spCanvas->FillRect(rcArea, NVec4f(.2f, .4f, .2f, 1.0f));
        }
        m_spCanvas->Text(textPos, fontHeight, fontColor, label.c_str());
    }

    if ((captured || hover) && (param.GetAttributes().displayType != ParameterDisplayType::None))
    {
        std::string prefix;
        float offset = (style.GetFloat(style_nodeTitleFontSize) * .5f) + node_labelPad + node_shadowSize;
        if (miniKnob)
        {
            auto pPin = dynamic_cast<Pin*>(&param);
            if (pPin)
            {
                prefix = pPin->GetName() + ": ";
            }
        }

        m_drawLabels[&param] = LabelInfo(NVec2f(knobRegion.Center().x, rect.Top() - offset), prefix);
    }
    return m_pCaptureParam == &param;
}

NRectf GraphView::GetShadowRegion(const NRectf& region) const
{
    auto& style = StyleManager::Instance();
    auto shadowSize = style.GetFloat(style_controlShadowSize);
    NRectf newRegion = region;
    newRegion.Adjust(shadowSize, shadowSize, 0.0f, 0.0f);
    return newRegion;
}

NRectf GraphView::GetInnerRegion(const NRectf& region) const
{
    auto& style = StyleManager::Instance();
    auto shadowSize = style.GetFloat(style_controlShadowSize);
    NRectf newRegion = region;
    newRegion.Adjust(0.0f, 0.0f, -shadowSize, -shadowSize);
    return newRegion;
}

void GraphView::SplitRegionAddPad(const NRectf& region, NRectf& remainRegion, NRectf& padRegion) const
{
    auto& style = StyleManager::Instance();
    auto padSize = style.GetFloat(style_nodePadSize);
    float padPad = 4.0f;
    remainRegion = region;
    remainRegion.Adjust(0.0f, 0.0f, -padSize - padPad, 0.0f);

    padSize = std::max(padSize, remainRegion.Height());
    padRegion = NRectf(remainRegion.Right() + padPad, remainRegion.Top(), padSize, remainRegion.Height());
}

void GraphView::DrawSlab(const NRectf& region, const NVec4f& color)
{
    auto& style = StyleManager::Instance();
    auto& theme = ThemeManager::Instance();

    auto shadowSize = style.GetFloat(style_controlShadowSize);

    // Draw the shadow
    m_spCanvas->FillRoundedRect(GetShadowRegion(region), style.GetFloat(style_nodeBorderRadius), theme.Get(color_controlShadowColor));

    // Draw the interior
    m_spCanvas->FillRoundedRect(GetInnerRegion(region), style.GetFloat(style_nodeBorderRadius), color);
}

void GraphView::DrawTri(const NRectf& region, const NVec4f& color, Side orient)
{
    auto& style = StyleManager::Instance();
    auto& theme = ThemeManager::Instance();

    auto fill = [=](auto r, auto color) {
        NVec2f points[3];

        switch (orient)
        {
        case Side::Left:
        {
            points[0] = NVec2f(r.Left(), r.Center().y);
            points[1] = r.TopRight();
            points[2] = r.bottomRightPx;
        }
        break;
        case Side::Right:
        {
            points[0] = NVec2f(r.Right(), r.Center().y);
            points[1] = r.topLeftPx;
            points[2] = r.BottomLeft();
        }
        break;
        case Side::Top:
        {
            points[0] = r.BottomLeft();
            points[1] = NVec2f(r.Center().x, r.Top());
            points[2] = r.bottomRightPx;
        }
        break;
        case Side::Bottom:
        {
            points[0] = r.topLeftPx;
            points[1] = NVec2f(r.Center().x, r.Bottom());
            points[2] = r.TopRight();
        }
        break;
        }

        m_spCanvas->BeginPath(points[2], color);
        m_spCanvas->LineTo(points[1]);
        m_spCanvas->LineTo(points[0]);
        m_spCanvas->LineTo(points[2]);
        m_spCanvas->EndPath();
    };
    fill(GetShadowRegion(region), theme.Get(color_controlShadowColor));
    fill(GetInnerRegion(region), color);
}

SliderData GraphView::DrawSlider(ViewNode& viewNode, Pin& param, NRectf region)
{
    auto& style = StyleManager::Instance();
    auto& theme = ThemeManager::Instance();
    auto& attrib = param.GetAttributes();

    bool hover = false;
    bool captured = false;
    float fMin = 0.0f;
    float fMax = 1.0f;
    float fCurrentVal = (float)param.Normalized();
    float fStep = (float)param.NormalizedStep();
    float fRange = std::max(fMax - fMin, std::numeric_limits<float>::epsilon());
    float fThumb = 1.0f / (fRange / fStep);

    if (attrib.thumb.type != ParameterType::None)
    {
        fThumb = attrib.thumb.To<float>();
    }

    NRectf sliderRegion, padRegion;
    if (param.GetSource() != nullptr)
    {
        SplitRegionAddPad(region, sliderRegion, padRegion);
        param.SetPadRect(padRegion, Side::Left, Side::Right);
    }
    else
    {
        sliderRegion = region;
    }

    auto innerRegion = GetInnerRegion(sliderRegion);
    float fThumbWidth = std::max(innerRegion.Width() * fThumb, 6.0f);
    float fRegionWidthNoThumb = innerRegion.Width() - fThumbWidth;
    auto fVal = std::clamp(fCurrentVal, fMin, fMax);
    float rangePerPixel = fRange / fRegionWidthNoThumb;

    if (param.GetSource() == nullptr)
    {
        // Update the slider if it is being clicked
        captured = CheckCapture(viewNode, param, innerRegion, hover);
        if (captured)
        {
            auto pos = m_spCanvas->GetViewMousePos();
            auto fNewVal = ((pos.x - innerRegion.Left()) * rangePerPixel) - (fThumbWidth * .5f * rangePerPixel);

            auto fQuant = std::floor(fNewVal / fStep) * fStep;

            param.SetFromNormalized(fQuant);
        }
    }

    auto fillColor = theme.Get(color_controlFillColor);
    if (hover || captured)
    {
        fillColor = theme.Get(color_controlFillColorHL);
    }

    if (!padRegion.Empty())
    {
        //DrawSlab(padRegion, theme.Get(color_flowControl));
        DrawTri(padRegion, theme.Get(color_flowControl), Side::Left);
    }
    DrawSlab(sliderRegion, fillColor);

    NRectf thumbRect = NRectf(innerRegion.Left() + fVal * fRegionWidthNoThumb,
        innerRegion.Top(),
        fThumbWidth,
        innerRegion.Height());

    // Draw the thumb
    m_spCanvas->FillRoundedRect(thumbRect, style.GetFloat(style_nodeBorderRadius), theme.Get(color_controlKeyColor1));

    if (!(attrib.flags & ParameterFlags::NoLabel))
    {
        std::string label;
        if (!attrib.labels.empty())
        {
            label = attrib.labels[0];
        }
        else
        {
            label = param.GetName();
        }

        std::string str;
        auto val = param.GetValue<float>();
        if (int(val) == val)
        {
            str = fmt::format("{}: {}", label, int(val));
        }
        else
        {
            str = fmt::format("{}: {:.3f}", label, val);
            str = string_right_trim(str, "0");
        }
        m_spCanvas->Text(NVec2f(region.Left() + style.GetFloat(style_controlTextMargin), region.Center().y - 2.0f), region.Height(), theme.Get(color_nodeButtonTextColor), str.c_str(), nullptr, Canvas::TEXT_ALIGN_MIDDLE | Canvas::TEXT_ALIGN_LEFT);
    }
    else
    {
        // Hover value; since it is not in the label
        auto node_titleFontSize = style.GetFloat(style_nodeTitleFontSize);
        if ((captured || hover) && (param.GetAttributes().displayType != ParameterDisplayType::None))
        {
            m_drawLabels[&param] = LabelInfo(NVec2f(thumbRect.Center().x, thumbRect.Top() - node_titleFontSize));
        }
    }

    SliderData ret;
    ret.channel = region;
    ret.thumb = thumbRect;
    return ret;
}

void GraphView::DrawButton(ViewNode& viewNode, Pin& param, NRectf region)
{
    auto& attrib = param.GetAttributes();
    auto& theme = ThemeManager::Instance();
    auto& style = StyleManager::Instance();

    // Draw the shadow
    m_spCanvas->FillRoundedRect(region, style.GetFloat(style_nodeBorderRadius), theme.Get(color_controlShadowColor));

    // Now we are at the contents
    region.Adjust(node_shadowSize, node_shadowSize, -node_shadowSize, -node_shadowSize);

    auto currentButton = param.To<int64_t>();
    auto numButtons = attrib.max.To<int64_t>();
    bool canDisable = attrib.min.To<int64_t>() < 0 ? true : false;
    if (attrib.multiSelect)
    {
        canDisable = true;
    }

    float buttonWidth = region.Width() / numButtons;
    buttonWidth -= node_buttonPad;

    auto mousePos = m_spCanvas->GetViewMousePos();

    auto const& state = m_spCanvas->GetInputState();
    for (int i = 0; i < numButtons; i++)
    {
        auto buttonRegion = NRectf(region.Left() + i * (buttonWidth + node_buttonPad), region.Top(), buttonWidth, region.Height());

        bool hover = false;
        CheckCapture(viewNode, param, buttonRegion, hover);

        bool overButton = buttonRegion.Contains(NVec2f(mousePos.x, mousePos.y));
        if (overButton && state.buttonClicked[MOUSE_LEFT])
        {
            if (!attrib.multiSelect)
            {
                if (canDisable && currentButton == i)
                {
                    currentButton = -1;
                }
                else
                {
                    currentButton = i;
                }
            }
            else
            {
                if (currentButton & ((int64_t)1 << i))
                {
                    currentButton &= ~((int64_t)1 << i);
                }
                else
                {
                    currentButton |= ((int64_t)1 << i);
                }
            }
            param.SetFrom<int64_t>(currentButton);
        }

        auto buttonColor = theme.Get(color_controlFillColor);

        if (!attrib.multiSelect)
        {
            if (i == currentButton)
            {
                buttonColor = theme.Get(color_controlKeyColor1);
            }
        }
        else
        {
            if (currentButton & ((int64_t)1 << i))
            {
                buttonColor = theme.Get(color_controlKeyColor1);
            }
        }
        auto buttonHLColor = buttonColor + NVec4f(.05f, .05f, .05f, 0.0f);

        if (overButton)
        {
            buttonColor += NVec4f(.05f, .05f, .05f, 0.0f);
            buttonHLColor += NVec4f(.05f, .05f, .05f, 0.0f);
        }

        if (numButtons == 1)
        {
            buttonRegion.Adjust(0, 0, 1, 0);
            m_spCanvas->FillGradientRoundedRect(buttonRegion, style.GetFloat(style_nodeBorderRadius), buttonRegion, buttonColor, buttonHLColor);
        }
        else
        {
            if (i == 0 && m_spCanvas->HasGradientVarying())
            {
                m_spCanvas->FillGradientRoundedRectVarying(buttonRegion, NVec4f(style.GetFloat(style_nodeBorderRadius), 0.0f, 0.0f, style.GetFloat(style_nodeBorderRadius)), buttonRegion, buttonColor, buttonHLColor);
            }
            else if (i == numButtons - 1 && m_spCanvas->HasGradientVarying())
            {
                buttonRegion.Adjust(0, 0, 1, 0);
                m_spCanvas->FillGradientRoundedRectVarying(buttonRegion, NVec4f(0.0f, style.GetFloat(style_nodeBorderRadius), style.GetFloat(style_nodeBorderRadius), 0.0f), buttonRegion, buttonColor, buttonHLColor);
            }
            else
            {
                m_spCanvas->FillGradientRoundedRect(buttonRegion, 0.0f, buttonRegion, buttonColor, buttonHLColor);
            }
        }

        if (attrib.labels.size() > i)
        {
            m_spCanvas->Text(buttonRegion.Center() + NVec2f(0, 1), buttonRegion.Height() * .5f, theme.Get(color_nodeButtonTextColor), attrib.labels[i].c_str());
        }
    }
}

void GraphView::HandleInput()
{
    auto& state = m_spCanvas->GetInputState();
    if (state.buttonClicked[MouseButtons::MOUSE_RIGHT] || state.buttonReleased[MouseButtons::MOUSE_RIGHT] || state.buttonClicked[MouseButtons::MOUSE_LEFT] || state.buttonReleased[MouseButtons::MOUSE_LEFT])
    {
        // A transition of state, a new thing
        m_pCaptureNode = nullptr;
        m_pCaptureParam = nullptr;
    }

    auto pos = m_spCanvas->GetViewMousePos();
    for (auto& pNode : m_spViewData->nodeZOrder)
    {
        auto pView = m_spViewData->mapNodeToViewNode[pNode];
        auto& layout = pNode->GetLayout();
        if (layout.spRoot == nullptr)
            continue;

        auto nodeRect = layout.spRoot->GetViewRect() + pNode->GetPos();
        auto titleRect = layout.spTitle->GetViewRect() + pNode->GetPos();
        auto footerRect = layout.spFooter->GetViewRect() + pNode->GetPos();
        bool overNode = nodeRect.Contains(NVec2f(pos.x, pos.y));
        bool overTitle = titleRect.Contains(NVec2f(pos.x, pos.y)) || footerRect.Contains(NVec2f(pos.x, pos.y));

        pView->hovered = overNode;
        auto isRightDrag = (overNode && state.buttonClicked[MouseButtons::MOUSE_RIGHT]) || (state.buttonDown[MouseButtons::MOUSE_RIGHT] && m_pCaptureNode == pNode);
        auto isTitleDrag = (overTitle && state.buttonClicked[MouseButtons::MOUSE_LEFT]) || (state.buttonDown[MouseButtons::MOUSE_LEFT] && m_pCaptureNode == pNode);

        if (isRightDrag || isTitleDrag)
        {
            m_pCaptureNode = pNode;
            pView->active = true;
        }
        else
        {
            if (m_pCaptureNode == pNode)
            {
                m_pCaptureNode = nullptr;
            }

            if (m_spCanvas->GetInputState().captureState == CaptureState::None)
            {
                pView->active = false;
            }
        }
    }

    // If the user is dragging this node and not interacting with controls, disable capture for everything else
    if (m_pCaptureNode)
    {
        m_spCanvas->GetInputState().captureState = CaptureState::MoveNode;

        auto scaledPixel = state.mouseDelta * (1.0f / m_spCanvas->GetViewScale());
        m_pCaptureNode->SetPos(m_pCaptureNode->GetPos() + scaledPixel);

        if (m_spViewData->nodeZOrder.empty())
        {
            m_spViewData->nodeZOrder.resize(1);
        }

        if (m_spViewData->nodeZOrder.back() != m_pCaptureNode)
        {
            auto itrFound = std::find(m_spViewData->nodeZOrder.begin(), m_spViewData->nodeZOrder.end(), m_pCaptureNode);
            if (itrFound != m_spViewData->nodeZOrder.end())
            {
                m_spViewData->nodeZOrder.erase(itrFound);
                m_spViewData->nodeZOrder.push_back(m_pCaptureNode);
            }
        }
    }
}

void GraphView::Show(const NVec4f& clearColor)
{
    PROFILE_SCOPE(GraphView_Show);
    BuildNodes();

    HandleInput();

    auto displaySize = m_spCanvas->GetPixelRect().Size();
    m_spCanvas->Begin(clearColor);

    node_gridScale = 75.0f * dpi.scaleFactorXY.x;
    m_spCanvas->DrawGrid(node_gridScale);

    NVec2f currentPos(node_borderPad, node_borderPad);
    NVec4f nodeColor(.5f, .5f, .5f, 1.0f);
    NVec4f pinBGColor(.2f, .2f, .2f, 1.0f);

    float maxHeightNode = 0.0f;

    m_drawLabels.clear();

    for (auto& pNode : m_spViewData->nodeZOrder)
    {
        auto pView = m_spViewData->mapNodeToViewNode[pNode];

        NVec2f gridSize(0);

        pNode->PreDraw();
        pNode->Draw(*this, *m_spCanvas, *pView);
    }

    auto drawConnector = [=](Pin* pPin) {
        if (pPin->GetTargets().empty())
        {
            return;
        }

        /*if ((*pPin->GetTargets().begin())->GetOwnerNode().GetName() != "Add")
        {
            return; 
        }
        */

        auto& theme = ThemeManager::Instance();
        auto& style = StyleManager::Instance();

        for (auto& pTarget : pPin->GetTargets())
        {
            auto srcRect = pPin->GetPadRect();
            auto dstRect = pTarget->GetPadRect();
            auto srcOrient = pPin->GetPadLocation();
            auto dstOrient = pTarget->GetPadLocation();

            NVec2f srcDist;
            switch (srcOrient)
            {
            case Side::Left:
                srcDist = NVec2f(-1, 0);
                break;
            case Side::Right:
                srcDist = NVec2f(1, 0);
                break;
            case Side::Top:
                srcDist = NVec2f(0, -1);
                break;
            case Side::Bottom:
                srcDist = NVec2f(0, 1);
                break;
            }
            
            NVec2f dstDist;
            switch (dstOrient)
            {
            case Side::Left:
                dstDist = NVec2f(-1, 0);
                break;
            case Side::Right:
                dstDist = NVec2f(1, 0);
                break;
            case Side::Top:
                dstDist = NVec2f(0, -1);
                break;
            case Side::Bottom:
                dstDist = NVec2f(0, 1);
                break;
            }

            auto maxDist = std::max(std::abs(srcRect.Center().x - dstRect.Center().x), std::abs(srcRect.Center().y - dstRect.Center().y)) * .3f;
            maxDist = std::max(maxDist, dstRect.Width() * 3.0f);

            dstDist *= maxDist;
            srcDist *= maxDist;

            NVec4f col;
            if (pPin->GetType() == ParameterType::FlowData)
            {
                col = theme.Get(pPin->GetFlowData()->GetFlowType() == FlowType_Data ? color_flowControl : color_flowData);
            }
            else
            {
                col = theme.Get(color_flowData);
            }
            m_spCanvas->DrawCubicBezier(srcRect.Center(), srcRect.Center() + srcDist, dstRect.Center() + dstDist, dstRect.Center(), col);
        }
    };

    for (auto& pNode : m_spViewData->nodeZOrder)
    {
        auto& outputs = pNode->GetFlowControlOutputs();
        for (int i = 0; i < outputs.size(); i++)
        {
            drawConnector(outputs[i]);
        }
    }

    for (auto& [param, info] : m_drawLabels)
    {
        DrawLabel(*param, info);
    }

    m_spCanvas->End();
}

Graph* GraphView::GetGraph() const
{
    return m_pGraph;
}

Canvas* GraphView::GetCanvas() const
{
    return m_spCanvas.get();
}

void GraphView::DrawNode(ViewNode& viewNode)
{
    auto& canvas = *m_spCanvas;
    auto& style = StyleManager::Instance();
    auto& theme = ThemeManager::Instance();
    auto& layout = viewNode.pModelNode->GetLayout();
    auto& node = *viewNode.pModelNode;

    auto nodePos = viewNode.pModelNode->GetPos();
    auto nodeRect = layout.spRoot->GetViewRect() + nodePos;
    auto titleRect = layout.spTitle->GetViewRect() + nodePos;
    auto footerRect = layout.spFooter->GetViewRect() + nodePos;
    auto contentRect = layout.spContents->GetViewRect() + nodePos;

    auto mousePos = m_spCanvas->GetViewMousePos();

    // First debug
    if (m_debugVisuals)
    {
        uint32_t index = 0;
        layout.spRoot->VisitLayouts([&](Layout* pLayout) {
            auto rc = pLayout->GetViewRect().Expanded(pLayout->GetMargin()) + nodePos;

            auto col = NVec4f(.3f, .05f, .05f, .5f);
            m_spCanvas->FillRect(rc, col);

            rc.Expand(-pLayout->GetMargin());
            col = colors_get_default(index++);
            col.w = .25f;
            m_spCanvas->FillRect(rc, col);
        });
    }

    // Shell
    NVec4f nodeColor;
    if (viewNode.active)
    {
        nodeColor = theme.Get(color_nodeActiveBackground);
    }
    else if (viewNode.hovered)
    {
        nodeColor = theme.Get(color_nodeHoverBackground);
    }
    else
    {
        nodeColor = theme.Get(color_nodeBackground);
    }

    nodeRect.Adjust(style.GetFloat(style_nodeShadowSize), style.GetFloat(style_nodeShadowSize));
    m_spCanvas->FillRoundedRect(nodeRect, style.GetFloat(style_nodeBorderRadius), theme.Get(color_nodeShadowColor));
    nodeRect.Adjust(-style.GetFloat(style_nodeShadowSize), -style.GetFloat(style_nodeShadowSize));

    m_spCanvas->FillRoundedRect(nodeRect, style.GetFloat(style_nodeBorderRadius), nodeColor);
    m_spCanvas->FillRoundedRect(titleRect, style.GetFloat(style_nodeBorderRadius), theme.Get(color_nodeTitleBGColor));

    // Connectors
    auto margin = style.GetFloat(style_nodeLayoutMargin);
    auto padSize = style.GetFloat(style_nodePadSize);
    auto padSizeHalf = padSize * .5f;

    NVec2f targetSites[4];
    uint32_t siteCount[4] = { 0, 0, 0, 0 };
    targetSites[int(Side::Left)] = NVec2f(nodeRect.Left() - padSize, nodeRect.Center().y - padSizeHalf);
    targetSites[int(Side::Right)] = NVec2f(nodeRect.Right(), nodeRect.Center().y - padSizeHalf);
    targetSites[int(Side::Top)] = NVec2f(nodeRect.Center().x - padSizeHalf, nodeRect.Top() - padSize);
    targetSites[int(Side::Bottom)] = NVec2f(nodeRect.Center().x - padSizeHalf, nodeRect.Bottom());

    auto drawIO = [=, &siteCount](Pin* pPin, int i, int maxPads, bool in, int side = 0) {
        auto& theme = ThemeManager::Instance();
        auto& style = StyleManager::Instance();
        uint32_t type = pPin->GetFlowData()->GetFlowType();

        NVec2f center = nodeRect.Center();
        NVec2f otherCenter;
        bool input = false;
        // Figure out which side the io pad lives on based on the where it is relative to the other node it is connected to
        if (pPin->GetDirection() == PinDir::Input)
        {
            input = true;
            if (pPin->GetSource())
            {
                otherCenter = pPin->GetSource()->GetOwnerNode().GetCenter();
            }
            else
            {
                otherCenter = nodeRect.Top();
            }
        }
        else
        {
            if (!pPin->GetTargets().empty())
            {
                otherCenter = (*pPin->GetTargets().begin())->GetOwnerNode().GetCenter();
            }
            else
            {
                otherCenter = nodeRect.Bottom();
            }
        }

        auto diff = otherCenter - center;

        Side orient;
        Side location;
        NVec2f sitePos;
        if (fabs(diff.x) > fabs(diff.y))
        {
            if (diff.x >= 0.0f)
            {
                orient = Side::Right;
                location = Side::Right;
                sitePos = targetSites[int(Side::Right)];
                sitePos.x += (siteCount[int(Side::Right)]++ * (padSize + margin));
            }
            else
            {
                orient = Side::Left;
                location = Side::Left;
                sitePos = targetSites[int(Side::Left)];
                sitePos.x -= (siteCount[int(Side::Left)]++ * (padSize + margin));
            }
        }
        else
        {
            if (diff.y >= 0.0f)
            {
                orient = Side::Bottom;
                location = Side::Bottom;
                sitePos = targetSites[int(Side::Bottom)];
                sitePos.y += (siteCount[int(Side::Bottom)]++ * (padSize + margin));
            }
            else
            {
                orient = Side::Top;
                location = Side::Top;
                sitePos = targetSites[int(Side::Top)];
                sitePos.y -= (siteCount[int(Side::Top)]++ * (padSize + margin));
            }
        }

        NRectf rcPad;
        rcPad = NRectf(sitePos.x, sitePos.y, padSize, padSize);
        
        if (input)
        {
            switch (orient)
            {
            case Side::Left:
                orient = Side::Right;
                break;
            case Side::Right:
                orient = Side::Left;
                break;
            case Side::Top:
                orient = Side::Bottom;
                break;
            case Side::Bottom:
                orient = Side::Top;
                break;
            }
        }
        DrawTri(rcPad, theme.Get(type == FlowType_Data ? color_flowControl : color_flowData), orient);
        pPin->SetPadRect(rcPad, orient, location);
    };

    auto& inputs = node.GetFlowControlInputs();
    for (int i = 0; i < inputs.size(); i++)
    {
        drawIO(inputs[i], i, int(inputs.size()), true, 0);
    }

    auto& outputs = node.GetFlowControlOutputs();
    for (int i = 0; i < outputs.size(); i++)
    {
        drawIO(outputs[i], i, int(outputs.size()), false, 0);
    }

    // Title text
    m_spCanvas->Text(NVec2f(titleRect.Center().x, titleRect.Center().y), style.GetFloat(style_nodeTitleFontSize), theme.Get(color_nodeTitleColor), viewNode.pModelNode->GetName().c_str());

    // Inner contents
    layout.spRoot->VisitLayouts([&](Layout* pLayout) {
        for (auto& pControl : pLayout->GetItems())
        {
            auto pPin = dynamic_cast<Pin*>(pControl);
            if (pPin)
            {
                DrawPin(viewNode, *pPin);
            }
        }
    });
}

} // namespace NodeGraph
