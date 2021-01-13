#include <map>

#include <fmt/format.h>

#include <nanovg.h>

#define DECLARE_NODE_STYLES

#include <mutils/logger/logger.h>
#include <mutils/math/math.h>
#include <mutils/ui/colors.h>
#include <mutils/ui/dpi.h>

#include <nodegraph/view/graphview.h>
#include <nodegraph/view/viewnode.h>
#include <nodegraph/view/style.h>

using namespace MUtils;
using namespace MUtils::Theme;
using namespace MUtils::Style;

namespace
{
NVec4f node_Color(.3f, .3f, .3f, 1.0f);
NVec4f node_TitleColor(1.0f, 1.0f, 1.0f, 1.0f);
NVec4f node_TitleBGColor(0.4f, .4f, 0.4f, 1.0f);
NVec4f node_buttonTextColor(0.15f, .15f, 0.15f, 1.0f);
NVec4f node_HLColor(0.98f, 0.48f, 0.28f, 1.0f);
NVec4f node_shadowColor(0.1f, 0.1f, 0.1f, 1.0f);

float widget_fontHeight = 28.0f;
float widget_fontHeightSmall = 16.0f;

float knobWidget_miniSize = 75.0f;

// A little daylight
float node_textGap = 1.0f;
float node_shadowSize = 2.0f;
float node_borderRadius = 7.0f;
float node_borderPad = 2.0f;
float node_buttonPad = 2.0f;
float node_pinPad = 4.0f;
float node_titleFontSize = 17.0f * 1.5f;
float node_titleHeight = node_titleFontSize + node_borderPad * 4.0f;
float node_gridScale = 125.0f;
float node_titleBorder = node_borderPad * 2.0f;
float node_labelPad = 6.0f;
} // namespace

namespace NodeGraph
{

GraphView::GraphView(Graph* pGraph, std::shared_ptr<Canvas> spCanvas)
    : m_pGraph(pGraph)
    , m_spCanvas(spCanvas)
{
    m_spViewData = std::make_shared<GraphViewData>();

    m_spViewData->pendingUpdate = true;

    m_spViewData->connections.push_back(pGraph->sigBeginModify.connect([=](Graph* pGraph) {
        m_spViewData->disabled = true;

        m_spViewData->mapWorldToView.clear();
        m_spViewData->mapNodeCreateOrder.clear();
    }));

    m_spViewData->connections.push_back(pGraph->sigEndModify.connect([=](Graph* pGraph) {
        m_spViewData->disabled = false;
        m_spViewData->pendingUpdate = true;
    }));
}

void GraphView::InitStyles()
{
    auto& style = StyleManager::Instance();
    
    // For connectors around side
    style.Set(style_nodeOuter, 20.0f); 

    // Title and padding
    style.Set(style_nodeTitleHeight, 30.0f);
    style.Set(style_nodeTitlePad, 4.0f);

    style.Set(style_nodeContentsPad, 2.0f);
}

bool GraphView::ShouldShowNode(Canvas& canvas, const Node* pNode) const
{
    if (pNode->Flags() & NodeFlags::Hidden)
        return false;

    if (pNode->Flags() & NodeFlags::OwnerDraw)
        return true;

    // Check for things to show
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

    for (auto& decorator : pNode->GetDecorators())
    {
        if (!decorator->gridLocation.Empty())
            return true;
    }

    if (!pNode->GetCustomViewCells().Empty())
        return true;

    return false;
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
        if (m_spViewData->mapWorldToView.find(pNode) == m_spViewData->mapWorldToView.end())
        {
            if (ShouldShowNode(*m_spCanvas, pNode))
            {
                auto spViewNode = std::make_shared<ViewNode>(pNode);

                m_spViewData->mapWorldToView[pNode] = spViewNode;
                m_spViewData->mapNodeCreateOrder[pNode->GetId()] = pNode;
            }
        }
    }
}

bool GraphView::CheckCapture(Canvas& canvas, Parameter& param, const NRectf& region, bool& hover)
{
    auto pos = canvas.GetViewMousePos();
    bool overParam = region.Contains(NVec2f(pos.x, pos.y));
    auto const& state = canvas.GetInputState();

    if (state.buttonReleased[MOUSE_LEFT])
    {
        m_pCaptureParam = nullptr;
    }

    if (m_pCaptureParam == nullptr)
    {
        if (state.buttonClicked[MOUSE_LEFT])
        {
            if (overParam)
            {
                m_pCaptureParam = &param;
                m_mouseStart = NVec2f(pos.x, pos.y);
                m_pStartValue = std::make_shared<Parameter>(*m_pCaptureParam);
            }
        }
    }

    if ((m_pCaptureParam == &param) || (overParam && m_pCaptureParam == nullptr))
    {
        hover = true;
        if (param.GetAttributes().flags & ParameterFlags::ReadOnly)
        {
            m_pCaptureParam = nullptr;
        }
    }
    else
    {
        hover = false;
    }

    canvas.Capture(m_pCaptureParam != nullptr);

    m_hideCursor = m_pCaptureParam != nullptr;
    return m_pCaptureParam == &param;
}

void GraphView::EvaluateDragDelta(Canvas& canvas, Pin& param, float rangePerDelta, InputDirection dir)
{
    if (!m_pStartValue)
    {
        return;
    }

    const auto& attrib = param.GetAttributes();
    auto startValue = m_pStartValue->Normalized();
    auto const& state = canvas.GetInputState();

    NVec2f d = state.dragDelta;
    if (state.slowDrag)
    {
        d *= .5f;
    }
    double delta = double(dir == InputDirection::X ? d.x : d.y);
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

void GraphView::CheckInput(Canvas& canvas, Pin& param, const NRectf& region, float rangePerDelta, bool& hover, bool& captured, InputDirection dir)
{
    if (param.GetSource() == nullptr)
    {
        captured = CheckCapture(canvas, param, region, hover);
        if (captured)
        {
            EvaluateDragDelta(canvas, param, rangePerDelta, dir);
        }
    }
}

void GraphView::DrawDecorator(Canvas& canvas, NodeDecorator& decorator, const NRectf& rc)
{
    static const NVec4f colorLabel(0.20f, 0.20f, 0.20f, 1.0f);
    static const NVec4f fontColor(.8f, .8f, .8f, 1.0f);

    if (decorator.type == DecoratorType::Label)
    {
        float fontSize = 28.0f;

        canvas.Text(rc.Center(), fontSize, fontColor, decorator.strName.c_str());
    }
    else if (decorator.type == DecoratorType::Line)
    {
        auto center = rc.Center();
        canvas.Stroke(NVec2f(rc.Left(), center.y), NVec2f(rc.Right(), center.y), 2.0f, node_shadowColor);
    }
}

void GraphView::DrawLabel(Canvas& canvas, Parameter& param, const LabelInfo& info)
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
    NRectf rcFont = canvas.TextBounds(info.pos, fontSize, val.c_str());
    rcFont.Adjust(-rcFont.Width() * .5f, -rcFont.Height() * .5f);
    rcFont.Adjust(0, -node_labelPad);

    NRectf rcBounds = rcFont;
    rcBounds.Adjust(-node_labelPad, -node_labelPad, node_labelPad, node_labelPad);
    rcFont.Adjust(-node_labelPad, -node_labelPad, node_labelPad, node_labelPad);

    auto rcShadow = rcBounds;
    rcShadow.Adjust(-node_shadowSize, -node_shadowSize, node_shadowSize, node_shadowSize);

    //canvas.FillRect(rcShadow, node_shadowColor);
    canvas.FillRect(rcBounds, colorLabel);

    // Text is centered
    canvas.Text(rcFont.topLeftPx + rcFont.Size() * .5f, fontSize, fontColor, val.c_str());
}

void GraphView::DrawPin(Pin& pin, ViewNode& viewNode)
{
    auto& attrib = pin.GetAttributes();

    auto rc = pin.GetViewRect();
    rc.Adjust(viewNode.pModelNode->GetPos());

    DrawKnob(*GetCanvas(), rc, false, pin);
}

bool GraphView::DrawKnob(Canvas& canvas, NRectf rect, bool miniKnob, Pin& param)
{
    NVec4f color(0.45f, 0.45f, 0.45f, 1.0f);
    NVec4f colorLabel(0.35f, 0.35f, 0.35f, 1.0f);
    NVec4f colorHL(0.68f, 0.68f, 0.68f, 1.0f);
    NVec4f channelColor(0.38f, 0.38f, 0.38f, 1.0f);
    NVec4f shadowColor(0.1f, 0.1f, 0.1f, .5f);
    NVec4f channelHLColor(0.98f, 0.48f, 0.28f, 1.0f);
    NVec4f channelHighColor(0.98f, 0.10f, 0.10f, 1.0f);
    NVec4f markColor(.9f, .9f, 0.9f, 1.0f);
    NVec4f markHLColor(1.0f, 1.0f, 1.0f, 1.0f);
    NVec4f fontColor(.95f, .95f, .95f, 1.0f);
    float channelGap = 4;

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
    CheckInput(canvas, param, knobRegion, rangePerDelta, hover, captured, InputDirection::Y);

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
        canvas.FillRect(rect, NVec4f(.2f, .2f, .4f, 1.0f));
        canvas.FillRect(knobRegion, NVec4f(.4f, .2f, .2f, 1.0f));
    }

    auto innerSize = knobSize - channelWidth - channelGap;

    // Knob surrounding shadow; a filled circle behind it
    canvas.FilledCircle(knobRegion.Center(), innerSize, shadowColor);
    innerSize -= node_shadowSize;

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
        color = colorHL;
    }

    // Only draw the actual knob if big enough
    if (!miniKnob)
    {
        canvas.FilledGradientCircle(knobRegion.Center(), innerSize, NRectf(knobRegion.Center().x, knobRegion.Center().y - innerSize, 0, innerSize * 1.5f), colorHL, color);

        // the notch on the button/indicator
        auto markerAngle = degToRad(posArc + arcOffset);
        auto markVector = NVec2f(std::cos(markerAngle), std::sin(markerAngle));
        canvas.Stroke(knobRegion.Center() + markVector * (markerInset - node_shadowSize), knobRegion.Center() + markVector * (innerSize - node_shadowSize), channelWidth, shadowColor);
        canvas.Stroke(knobRegion.Center() + markVector * markerInset, knobRegion.Center() + markVector * (innerSize - node_shadowSize * 2), channelWidth - node_shadowSize, markColor);
    }
    else
    {
        float size = knobSize - channelWidth;
        canvas.FilledGradientCircle(knobRegion.Center(), size, NRectf(knobRegion.Center().x, knobRegion.Center().y - size, 0, size * 1.5f), colorHL, color);
    }

    canvas.Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelColor, startArc + arcOffset, endArc + arcOffset);

    // Cover the shortest arc between the 2 points
    if (posArcBegin > posArc)
    {
        std::swap(posArcBegin, posArc);
    }

    canvas.Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHLColor, posArcBegin + arcOffset, posArc + arcOffset);

    if (fCurrentVal > (fMax + std::numeric_limits<float>::epsilon()))
    {
        canvas.Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHighColor, endArc - 10 + arcOffset, endArc + arcOffset);
    }
    else if (fCurrentVal < (fMin - std::numeric_limits<float>::epsilon()))
    {
        canvas.Arc(knobRegion.Center(), knobSize - channelWidth * .5f, channelWidth, channelHighColor, startArc + arcOffset, startArc + 10 + arcOffset);
    }

    // Scale the font to fit and center it
    float bottomOffset = knobRegion.Bottom() + labelFit - fontHeight * .5f;
    auto rcText = canvas.TextBounds(NVec2f(0.0f, 0.0f), fontHeight, label.c_str());
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
    rcText = canvas.TextBounds(textPos, fontHeight, label.c_str());

    if (!miniKnob)
    {
        // Text
        if (false && m_debugVisuals)
        {
            auto rcArea = rcText;
            rcArea.Adjust(-rcText.Width() * .5f, -rcText.Height() * .5f);
            canvas.FillRect(rcArea, NVec4f(.2f, .4f, .2f, 1.0f));
        }
        canvas.Text(textPos, fontHeight, fontColor, label.c_str());
    }

    if ((captured || hover) && (param.GetAttributes().displayType != ParameterDisplayType::None))
    {
        std::string prefix;
        float offset = (node_titleFontSize * .5f) + node_labelPad + node_shadowSize;
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
    return false;
}

SliderData GraphView::DrawSlider(Canvas& canvas, NRectf region, Pin& param)
{
    NVec4f color(0.30f, 0.30f, 0.30f, 1.0f);
    NVec4f colorHL(0.35f, 0.35f, 0.35f, 1.0f);
    NVec4f channelColor(0.18f, 0.18f, 0.18f, 1.0f);
    NVec4f shadowColor(0.8f, 0.8f, 0.8f, .5f);
    NVec4f channelHLColor(0.98f, 0.48f, 0.28f, 1.0f);
    NVec4f channelHighColor(0.98f, 0.10f, 0.10f, 1.0f);
    NVec4f markColor(.55f, .55f, 0.55f, 1.0f);
    NVec4f markHLColor(1.0f, 1.0f, 1.0f, 1.0f);
    NVec4f fontColor(.8f, .8f, .8f, 1.0f);

    auto& attrib = param.GetAttributes();

    float fMin = 0.0f;
    float fMax = 1.0f;
    float fCurrentVal = (float)param.Normalized();
    float fStep = (float)param.NormalizedStep();

    float fRange = std::max(fMax - fMin, std::numeric_limits<float>::epsilon());

    // Draw the shadow
    canvas.FillRoundedRect(region, node_borderRadius, shadowColor);

    // Now we are at the contents
    region.Adjust(node_shadowSize, node_shadowSize, -node_shadowSize, -node_shadowSize);

    // Draw the interior
    canvas.FillGradientRoundedRect(region, node_borderRadius, region, color, colorHL);

    SliderData ret;

    region.Adjust(node_borderPad, node_borderPad, -node_borderPad, -node_borderPad);
    ret.channel = region;

    float fThumb = 1.0f / (fRange / fStep);
    if (attrib.thumb.type != ParameterType::None)
    {
        fThumb = attrib.thumb.To<float>();
    }

    float fThumbWidth = region.Width() * fThumb;
    float fRegionWidthNoThumb = region.Width() - fThumbWidth;

    // Clamp to sensible range
    auto fVal = std::clamp(fCurrentVal, fMin, fMax);

    NRectf thumbRect = NRectf(region.Left() + fVal * fRegionWidthNoThumb,
        region.Top(),
        fThumbWidth,
        region.Height());

    bool hover = false;
    bool captured = false;
    float rangePerPixel = fRange / fRegionWidthNoThumb;

    if (param.GetSource() == nullptr)
    {
        // Update the slider if it is being clicked
        captured = CheckCapture(canvas, param, region, hover);
        if (captured)
        {
            auto pos = canvas.GetViewMousePos();
            auto fNewVal = ((pos.x - region.Left()) * rangePerPixel) - (fThumbWidth * .5f * rangePerPixel);

            auto fQuant = std::floor(fNewVal / fStep) * fStep;

            param.SetFromNormalized(fQuant);
        }
    }

    if (hover || captured)
    {
        markColor += NVec4f(.1f, .1f, .1f, 0.0f);
    }

    // Draw the thumb
    canvas.FillRoundedRect(thumbRect, node_borderRadius, markColor);

    ret.thumb = thumbRect;

    if ((captured || hover) && (param.GetAttributes().displayType != ParameterDisplayType::None))
    {
        m_drawLabels[&param] = LabelInfo(NVec2f(thumbRect.Center().x, thumbRect.Top() - node_titleFontSize));
    }
    return ret;
};

void GraphView::DrawButton(Canvas& canvas, NRectf region, Pin& param)
{
    NVec4f color(0.30f, 0.30f, 0.30f, 1.0f);
    NVec4f colorHL(0.35f, 0.35f, 0.35f, 1.0f);
    NVec4f channelColor(0.18f, 0.18f, 0.18f, 1.0f);
    NVec4f shadowColor(0.25f, 0.25f, 0.25f, 1.0f);
    NVec4f channelHLColor(0.98f, 0.48f, 0.18f, 1.0f);
    NVec4f channelHighColor(0.98f, 0.10f, 0.10f, 1.0f);
    NVec4f markColor(.55f, .55f, 0.55f, 1.0f);
    NVec4f markHLColor(1.0f, 1.0f, 1.0f, 1.0f);
    NVec4f fontColor(.8f, .8f, .8f, 1.0f);

    auto& attrib = param.GetAttributes();

    // Draw the shadow
    canvas.FillRoundedRect(region, node_borderRadius, shadowColor);

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

    auto mousePos = canvas.GetViewMousePos();

    auto const& state = canvas.GetInputState();
    for (int i = 0; i < numButtons; i++)
    {
        auto buttonRegion = NRectf(region.Left() + i * (buttonWidth + node_buttonPad), region.Top(), buttonWidth, region.Height());

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

        auto buttonColor = markColor;

        if (!attrib.multiSelect)
        {
            if (i == currentButton)
            {
                buttonColor = channelHLColor;
            }
        }
        else
        {
            if (currentButton & ((int64_t)1 << i))
            {
                buttonColor = channelHLColor;
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
            canvas.FillGradientRoundedRect(buttonRegion, node_borderRadius, buttonRegion, buttonColor, buttonHLColor);
        }
        else
        {
            if (i == 0 && canvas.HasGradientVarying())
            {
                canvas.FillGradientRoundedRectVarying(buttonRegion, NVec4f(node_borderRadius, 0.0f, 0.0f, node_borderRadius), buttonRegion, buttonColor, buttonHLColor);
            }
            else if (i == numButtons - 1 && canvas.HasGradientVarying())
            {
                buttonRegion.Adjust(0, 0, 1, 0);
                canvas.FillGradientRoundedRectVarying(buttonRegion, NVec4f(0.0f, node_borderRadius, node_borderRadius, 0.0f), buttonRegion, buttonColor, buttonHLColor);
            }
            else
            {
                canvas.FillGradientRoundedRect(buttonRegion, 0.0f, buttonRegion, buttonColor, buttonHLColor);
            }
        }

        if (attrib.labels.size() > i)
        {
            canvas.Text(buttonRegion.Center() + NVec2f(0, 1), buttonRegion.Height() * .5f, node_buttonTextColor, attrib.labels[i].c_str());
        }
    }
}

NRectf GraphView::DrawNode(Canvas& canvas, const NRectf& pos, Node* pNode)
{
    canvas.FillRoundedRect(pos, node_borderRadius, node_Color);

    canvas.FillRoundedRect(NRectf(pos.Left() + node_titleBorder, pos.Top() + node_titleBorder, pos.Width() - node_titleBorder * 2.0f, node_titleHeight), node_borderRadius, node_TitleBGColor);

    canvas.Text(NVec2f(pos.Center().x, pos.Top() + node_titleBorder + node_titleHeight * .5f), node_titleFontSize, node_TitleColor, pNode->GetName().c_str());

    canvas.Text(NVec2f(pos.Left() + node_titleBorder * 3.0f, pos.Top() + node_titleBorder + node_titleHeight * .5f), node_titleFontSize / 2, NVec4f(.9f), fmt::format("{}", pNode->GetGeneration()).c_str());
    
    auto contentRect = NRectf(pos.Left() + node_borderPad, pos.Top() + node_titleBorder + node_titleHeight + node_borderPad, pos.Width() - (node_borderPad * 2), pos.Height() - node_titleHeight - (node_titleBorder * 2.0f) - node_borderPad);

    return contentRect;
}

void GraphView::Show(const NVec2i& displaySize, const NVec4f& clearColor)
{
    PROFILE_SCOPE(GraphView_Show);
    BuildNodes();

    m_spCanvas->Begin(displaySize, clearColor);

    node_gridScale = 75.0f * dpi.scaleFactorXY.x;
    m_spCanvas->DrawGrid(node_gridScale);

    NVec2f currentPos(node_borderPad, node_borderPad);
    NVec4f nodeColor(.5f, .5f, .5f, 1.0f);
    NVec4f pinBGColor(.2f, .2f, .2f, 1.0f);

    float maxHeightNode = 0.0f;

    m_drawLabels.clear();

    for (auto& [id, pNode] : m_spViewData->mapNodeCreateOrder)
    {
        auto pView = m_spViewData->mapWorldToView[pNode];

        NVec2f gridSize(0);

        pNode->PreDraw();

        if (pNode->Flags() & NodeFlags::OwnerDraw)
        {
            pNode->Draw(*this, *m_spCanvas, *pView);
            continue;
        }

        auto pins = pNode->GetInputs();
        pins.insert(pins.end(), pNode->GetOutputs().begin(), pNode->GetOutputs().end());

        for (auto& pInput : pins)
        {
            if (pInput->GetViewCells().Empty())
                continue;
            gridSize.x = std::max(gridSize.x, pInput->GetViewCells().Right());
            gridSize.y = std::max(gridSize.y, pInput->GetViewCells().Bottom());
        }

        for (auto& pDecorator : pNode->GetDecorators())
        {
            gridSize.x = std::max(gridSize.x, pDecorator->gridLocation.Right());
            gridSize.y = std::max(gridSize.y, pDecorator->gridLocation.Bottom());
        }

        // Account for custom
        auto custom = pNode->GetCustomViewCells();
        gridSize.x = std::max(gridSize.x, custom.Right());
        gridSize.y = std::max(gridSize.y, custom.Bottom());

        gridSize.x = std::max(1.0f, gridSize.x);
        gridSize.y = std::max(1.0f, gridSize.y);

        gridSize.x *= pNode->GetGridScale().x;
        gridSize.y *= pNode->GetGridScale().y;

        NVec2f nodeSize;
        nodeSize.x = gridSize.x * node_gridScale - node_borderPad * 2.0f;
        nodeSize.y = (gridSize.y * node_gridScale) + node_titleHeight + node_titleBorder;

        if (currentPos.x + nodeSize.x > displaySize.x)
        {
            currentPos.x = node_borderPad;
            currentPos.y += maxHeightNode + node_borderPad;
            maxHeightNode = 0.0f;
        }

        auto contentRect = DrawNode(*m_spCanvas, NRectf(currentPos.x, currentPos.y, nodeSize.x, nodeSize.y), pNode);

        auto cellSize = contentRect.Size() / gridSize;

        for (auto& decorator : pNode->GetDecorators())
        {
            auto decoratorGrid = decorator->gridLocation;
            decoratorGrid.topLeftPx.x *= pNode->GetGridScale().x;
            decoratorGrid.topLeftPx.y *= pNode->GetGridScale().y;
            decoratorGrid.bottomRightPx.x *= pNode->GetGridScale().x;
            decoratorGrid.bottomRightPx.y *= pNode->GetGridScale().y;

            auto decoratorCell = NRectf(contentRect.Left() + (decoratorGrid.Left() * cellSize.x),
                contentRect.Top() + (decoratorGrid.Top() * cellSize.y),
                cellSize.x * decoratorGrid.Width(),
                cellSize.y * decoratorGrid.Height());
            decoratorCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, /*-node_borderPad*/ 0.0f);

            DrawDecorator(*m_spCanvas, *decorator, decoratorCell);
        }

        for (auto& pInput : pins)
        {
            if (pInput->GetViewCells().Empty())
                continue;

            auto pinGrid = pInput->GetViewCells();
            pinGrid.topLeftPx.x *= pNode->GetGridScale().x;
            pinGrid.topLeftPx.y *= pNode->GetGridScale().y;
            pinGrid.bottomRightPx.x *= pNode->GetGridScale().x;
            pinGrid.bottomRightPx.y *= pNode->GetGridScale().y;

            auto pinCell = NRectf(contentRect.Left() + (pinGrid.Left() * cellSize.x),
                contentRect.Top() + (pinGrid.Top() * cellSize.y),
                cellSize.x * pinGrid.Width(),
                cellSize.y * pinGrid.Height());
            pinCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, /*-node_borderPad*/ 0.0f);

            if (pInput->GetAttributes().ui == ParameterUI::Knob)
            {
                bool miniKnob = pInput->GetViewCells().Width() < .99f && pInput->GetViewCells().Height() < .99f;
                DrawKnob(*m_spCanvas, pinCell, miniKnob, *pInput);
            }
            else if (pInput->GetAttributes().ui == ParameterUI::Slider)
            {
                pinCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, -node_pinPad);
                DrawSlider(*m_spCanvas, pinCell, *pInput);
            }
            else if (pInput->GetAttributes().ui == ParameterUI::Button)
            {
                pinCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, -node_pinPad);
                DrawButton(*m_spCanvas, pinCell, *pInput);
            }

            else if (pInput->GetAttributes().ui == ParameterUI::Custom)
            {
                pinCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, -node_pinPad);
                pNode->DrawCustomPin(*this, *m_spCanvas, pinCell, *pInput);
            }
        }

        if (!custom.Empty())
        {
            custom.topLeftPx.x *= pNode->GetGridScale().x;
            custom.topLeftPx.y *= pNode->GetGridScale().y;
            custom.bottomRightPx.x *= pNode->GetGridScale().x;
            custom.bottomRightPx.y *= pNode->GetGridScale().y;
            auto cell = NRectf(contentRect.Left() + (custom.Left() * cellSize.x),
                contentRect.Top() + (custom.Top() * cellSize.y),
                cellSize.x * custom.Width(),
                cellSize.y * custom.Height());
            cell.Adjust(node_borderPad, node_borderPad, -node_borderPad, /*-node_borderPad*/ 0.0f);

            m_spCanvas->FillRoundedRect(cell, node_borderRadius, pinBGColor);

            pNode->DrawCustom(*this, *m_spCanvas, cell);
        }

        maxHeightNode = std::max(maxHeightNode, nodeSize.y + node_borderPad * 2.0f);
        currentPos.x += nodeSize.x + node_borderPad * 2.0f;
    }

    for (auto& [param, info] : m_drawLabels)
    {
        DrawLabel(*m_spCanvas, *param, info);
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

void GraphView::DrawNode(NodeLayout& layout, ViewNode& viewNode)
{
    auto& canvas = *m_spCanvas;
    auto& style = StyleManager::Instance();
    auto& theme = ThemeManager::Instance();

    auto nodePos = viewNode.pModelNode->GetPos();

    // First debug
    if (m_debugVisuals)
    {
        uint32_t index = 0;
        layout.spRoot->VisitLayouts([&](Layout* pLayout) {
            auto rc = pLayout->GetViewRect().Expanded(pLayout->GetPadding()) + nodePos;

            auto col = NVec4f(.3f, .05f, .05f, .5f);
            canvas.FillRect(rc, col);

            rc.Expand(-pLayout->GetPadding());
            col = colors_get_default(index++);
            col.w = .25f;
            canvas.FillRect(rc, col);
        });
    }

    // Shell
    auto nodeRect = layout.spRoot->GetViewRect() + nodePos;
    canvas.FillRoundedRect(nodeRect, node_borderRadius, node_Color);

    auto titleRect = layout.spTitle->GetViewRect() + nodePos;
    canvas.FillRoundedRect(titleRect, node_borderRadius, node_TitleBGColor);

    // Connectors
    auto outerConnectorRect = layout.spRoot->GetViewRect() + nodePos;
    auto contentRect = layout.spContents->GetViewRect() + nodePos;
    auto outerRadius = style.GetFloat(style_nodeOuter) * .5f;
    // L
    canvas.FilledCircle(NVec2f(outerConnectorRect.Left() - outerRadius,
        outerConnectorRect.Center().y), outerRadius, theme.GetColor(ThemeColor::AccentColor1));

    // R
    canvas.FilledCircle(NVec2f(outerConnectorRect.Right() + outerRadius,
        outerConnectorRect.Center().y), outerRadius, theme.GetColor(ThemeColor::AccentColor1));

    // T
    canvas.FilledCircle(NVec2f(outerConnectorRect.Center().x,
        outerConnectorRect.Top() - outerRadius), outerRadius, theme.GetColor(ThemeColor::AccentColor1));

    // B
    canvas.FilledCircle(NVec2f(outerConnectorRect.Center().x,
        outerConnectorRect.Bottom() + outerRadius), outerRadius, theme.GetColor(ThemeColor::AccentColor1));


    canvas.Text(NVec2f(titleRect.Center().x, titleRect.Center().y), node_titleFontSize, node_TitleColor, viewNode.pModelNode->GetName().c_str());

    // Inner contents
    layout.spRoot->VisitLayouts([&](Layout* pLayout) {
        for (auto& pControl : pLayout->GetItems())
        {
            auto pPin = dynamic_cast<Pin*>(pControl);
            if (pPin)
            {
                DrawPin(*pPin, viewNode);
            }
        }
    });
}

} // namespace NodeGraph
