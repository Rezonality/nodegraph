#include <exception>
#include <stdexcept>

#include "mutils/logger/logger.h"
#include "nodegraph/model/node.h"
#include "nodegraph/model/pin.h"

using namespace MUtils;

namespace NodeGraph {

// Pins on the m_graph node
Pin::Pin(Node& o, PinDir pinDir, const std::string& pinName, float val, const ParameterAttributes& attribs)
    : Parameter(val, attribs)
    , m_owner(o)
    , m_direction(pinDir)
    , m_strName(pinName)
{
}

Pin::Pin(Node& o, PinDir pinDir, const std::string& pinName, double val, const ParameterAttributes& attribs)
    : Parameter(val, attribs)
    , m_owner(o)
    , m_direction(pinDir)
    , m_strName(pinName)
{
}

Pin::Pin(Node& o, PinDir pinDir, const std::string& pinName, int64_t val, const ParameterAttributes& attribs)
    : Parameter(val, attribs)
    , m_owner(o)
    , m_direction(pinDir)
    , m_strName(pinName)
{
}

Pin::Pin(Node& o, PinDir pinDir, const std::string& pinName, bool val, const ParameterAttributes& attribs)
    : Parameter(val, attribs)
    , m_owner(o)
    , m_direction(pinDir)
    , m_strName(pinName)
{
}

Pin::Pin(Node& o, PinDir pinDir, const std::string& pinName, IFlowData* val, const ParameterAttributes& attribs)
    : Parameter(val, attribs)
    , m_owner(o)
    , m_direction(pinDir)
    , m_strName(pinName)
{
}

Pin::Pin(Node& o, PinDir pinDir, const std::string& pinName, const std::string& str, const ParameterAttributes& attribs)
    : Parameter(str, attribs)
    , m_owner(o)
    , m_direction(pinDir)
    , m_strName(pinName)
{
}

Pin::Pin(Node& o, PinDir pinDir, const std::string& pinName, const Parameter& param)
    : Parameter(param)
    , m_owner(o)
    , m_direction(pinDir)
    , m_strName(pinName)
{
}
// Note can be greater than 1 if the value is out of bounds
double Pin::Normalized()
{
    auto min = m_attributes.min.To<double>();
    auto max = m_attributes.max.To<double>();

    double ret;
    if (m_attributes.taper == 1.0f)
    {
        ret = (GetValue<double>() - min) / (max - min);
    }
    else
    {
        ret = std::pow(((GetValue<double>() - min) / (max - min)), (1.0 / m_attributes.taper));
    }
    return std::clamp(ret, 0.0, 1.0);
}

double Pin::NormalizedStep() const
{
    auto min = m_attributes.min.To<double>();
    auto max = m_attributes.max.To<double>();
    return std::abs(m_attributes.step.To<double>() / (max - min));
}

double Pin::NormalizedOrigin() const
{
    auto min = m_attributes.min.To<double>();
    auto max = m_attributes.max.To<double>();
    auto origin = m_attributes.origin.To<double>();
    origin = std::max(origin, min);

    double ret;
    if (m_attributes.taper == 1.0f)
    {
        ret = ((m_attributes.origin.To<double>() - min) / (max - min));
    }
    else
    {
        // Taper == 1 is linear
        auto p = (origin - min) / (max - min);
        ret = (std::pow(p, (1.0 / (double)m_attributes.taper)));
    }
    return std::clamp(ret, 0.0, 1.0);
}

void Pin::SetFromNormalized(double val)
{
    auto min = m_attributes.min.To<double>();
    auto max = m_attributes.max.To<double>();

    val = std::clamp(val, 0.0, 1.0);
    if (m_attributes.taper == 1.0f)
    {
        SetFrom<double>(min + (max - min) * val);
    }

    // algebraic taper
    SetFrom<double>(min + (max - min) * std::pow(val, m_attributes.taper));
}

} // namespace NodeGraph
