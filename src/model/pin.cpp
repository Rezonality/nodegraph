#include <exception>
#include <stdexcept>

#include "mutils/logger/logger.h"
#include "nodegraph/model/pin.h"
#include "nodegraph/model/node.h"

using namespace MUtils;

namespace NodeGraph
{

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

Pin::Pin(Node& o, PinDir pinDir, const std::string& pinName, IControlData* val, const ParameterAttributes& attribs)
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

} // namespace NodeGraph
