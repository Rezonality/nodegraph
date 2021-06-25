#include "nodegraph/model/parameter.h"
#include <map>

namespace NodeGraph {

class IFlowData
{
public:
    virtual uint32_t GetFlowType() const = 0; // e.g. modulate/midi/audio
    virtual uint32_t GetNumSamples() const = 0; // How many data samples this flow contains
    virtual void SetNumSamples(uint32_t num) = 0; // Set the number of samples to use
    virtual uint32_t GetNumChannels() const = 0; // Number of data channels this flow has
    virtual bool HasChannelId(uint32_t id) const = 0;
    //virtual bool CanConvert(ParameterType type) const = 0; // Can we make flow data from this input parameter type?
    virtual void* To(ParameterType type, uint32_t channel = 0) const = 0;

    virtual void From(Parameter& value, uint32_t channel = 0) = 0;
    virtual void From(float value, uint32_t channel = 0) = 0;

    virtual ~IFlowData()
    {
    }
};

const uint32_t FlowType_Midi = 0;
const uint32_t FlowType_Audio = 1;
const uint32_t FlowType_Data = 2;

class FlowData : public IFlowData
{
public:
    FlowData(uint32_t flowType, ParameterType type)
        : m_flowType(flowType)
        , m_parameterType(type)
    {
        SetNumSamples(1);
    }
    virtual ~FlowData()
    {
    }

    virtual uint32_t GetFlowType() const override
    {
        return m_flowType;
    }

    virtual uint32_t GetNumSamples() const override
    {
        return m_numSamples;
    }

    void SetNumSamples(uint32_t num) override
    {
        m_numSamples = num;
        auto dataSize = num * GetParameterTypeSize(m_parameterType);
        if (m_channelData.empty())
        {
            m_channelData.insert(std::make_pair(0, std::vector<uint8_t>(dataSize)));
            return;
        }

        for (auto& ch : m_channelData)
        {
            ch.second.resize(dataSize);
        }
    }

    virtual uint32_t GetNumChannels() const override
    {
        return (uint32_t)m_channelData.size();
    }

    virtual bool HasChannelId(uint32_t id) const override
    {
        return m_channelData.find(id) != m_channelData.end();
    }

    /*virtual bool CanConvert(ParameterType type) const
    {
        switch (type)
        {
        case ParameterType::
        }
        return true;
    }
    */

    virtual void* To(ParameterType type, uint32_t channel = 0) const override
    {
        if (m_channelData.find(channel) == m_channelData.end())
        {
            return nullptr;
        }
        auto psize = GetParameterTypeSize(type);
        m_buffer.resize(psize * m_numSamples);
        auto& data = m_channelData.at(channel);
        for (uint32_t sample = 0; sample < m_numSamples; sample++)
        {
            switch (type)
            {
            case ParameterType::Float:
            {
                float* pTarget = (float*)&m_buffer[sample * psize];
                switch (m_parameterType)
                {
                case ParameterType::Float:
                    *pTarget = *(float*)&data[sample * psize];
                    break;
                case ParameterType::Bool:
                    *pTarget = *(bool*)data[sample * psize] ? 1.0f : 0.0f;
                    break;
                case ParameterType::Double:
                    *pTarget = (float)*(double*)data[sample * psize];
                    break;
                case ParameterType::Int64:
                    *pTarget = (float)*(int64_t*)data[sample * psize];
                    break;
                default:
                    assert(!"Invalid");
                }
            }
            break;
            case ParameterType::Bool:
                break;
            case ParameterType::Double:
            {
                double* pTarget = (double*)&m_buffer[sample * psize];
                switch (m_parameterType)
                {
                case ParameterType::Float:
                    *pTarget = (double)*(float*)&data[sample * psize];
                    break;
                case ParameterType::Bool:
                    *pTarget = *(bool*)data[sample * psize] ? 1.0 : 0.0;
                    break;
                case ParameterType::Double:
                    *pTarget = *(double*)data[sample * psize];
                    break;
                case ParameterType::Int64:
                    *pTarget = (double)*(int64_t*)data[sample * psize];
                    break;
                default:
                    assert(!"Invalid");
                }
            }
            break;
            case ParameterType::Int64:
                break;
            case ParameterType::String:
            case ParameterType::None:
                assert(!"Not happy");
                break;
            }
        }
        return (void*)&m_buffer[0];
    }

    virtual void From(Parameter& value, uint32_t channel = 0) override
    {
        SetNumSamples(1);
        switch (value.GetType())
        {
        default:
        case ParameterType::Float:
        {
            From(value.To<float>());
        }
        break;
        }
    }

    virtual void From(float fVal, uint32_t channel = 0) override
    {
        SetNumSamples(1);
        *(float*)&m_channelData[0][0] = fVal;
    }

private:
    uint32_t m_flowType = 0;
    uint32_t m_numSamples = 1;
    uint32_t m_numChannels = 1;
    ParameterType m_parameterType;
    std::map<uint32_t, std::vector<uint8_t>> m_channelData;
    mutable std::vector<uint32_t> m_buffer;
};

} // namespace NodeGraph
