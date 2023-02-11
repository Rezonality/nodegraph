#include <nodegraph/model/parameter.h>
#include <nodegraph/model/flow_data.h>

namespace NodeGraph 
{

float Parameter::FlowToFloat() const
{
    auto pData = m_value.pFVal->ToPtr(ParameterType::Float);
    if (!pData)
    {
        return 0.0f;
    }
    return *(float*)pData;
}

double Parameter::FlowToDouble() const
{
    auto pData = m_value.pFVal->ToPtr(ParameterType::Float);
    if (!pData)
    {
        return 0.0f;
    }
    return *(float*)pData;
}

int64_t Parameter::FlowToInt64() const
{
    auto pData = m_value.pFVal->ToPtr(ParameterType::Float);
    if (!pData)
    {
        return int64_t(0);
    }
    return *(int64_t*)pData;
}

bool Parameter::FlowToBool() const
{
    auto pData = m_value.pFVal->ToPtr(ParameterType::Bool);
    if (!pData)
    {
        return false;
    }
    return *(bool*)pData;
}

} //nodegraph