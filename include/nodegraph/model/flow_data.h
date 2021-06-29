#pragma once
#include "nodegraph/model/parameter.h"
#include <map>

namespace NodeGraph {

struct Channel
{
    Channel(uint32_t size = 0)
        : data(size, 0)
    {
    }

    std::vector<uint8_t> data;
    uint32_t flags = 0;
};

class IFlowData
{
public:
    virtual uint32_t GetFlowType() const = 0; // e.g. modulate/midi/audio
    virtual uint32_t GetNumSamples() const = 0; // How many data samples this flow contains
    virtual void SetNumSamples(uint32_t num) = 0; // Set the number of samples to use
    virtual uint32_t GetNumChannels() const = 0; // Number of data channels this flow has
    virtual bool HasChannelId(uint32_t id) const = 0;
    virtual Channel* GetChannelById(uint32_t id) = 0;
    virtual const std::map<uint32_t, Channel>& GetChannels() const = 0;
    
    virtual void* ToPtr(ParameterType type, uint32_t channel = 0) const = 0;
    virtual float* ToFloatPtr(uint32_t channel = 0) const = 0;

    virtual void From(Parameter& value, uint32_t channel = 0) = 0;
    virtual void From(float value, uint32_t channel = 0) = 0;
   
    // Optional copy of all channel data, otherwise just resize to match dimensions
    // (which may be 'free')
    virtual void MatchChannelInput(IFlowData& flowData, bool copy = false) = 0;

    virtual ~IFlowData()
    {
    }
};

const uint32_t FlowType_Midi = 0;
const uint32_t FlowType_Audio = 1;
const uint32_t FlowType_Data = 2;

namespace ChannelFlags {
enum
{

};
}

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
        if (m_data.empty())
        {
            m_data.insert(std::make_pair(0, Channel(dataSize)));
            return;
        }

        for (auto& ch : m_data)
        {
            ch.second.data.resize(dataSize);
        }
    }

    virtual uint32_t GetNumChannels() const override
    {
        return (uint32_t)m_data.size();
    }

    virtual bool HasChannelId(uint32_t id) const override
    {
        return m_data.find(id) != m_data.end();
    }

    virtual Channel* GetChannelById(uint32_t id) override
    {
        assert(HasChannelId(id));
        return &m_data[id];
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

    virtual void MatchChannelInput(IFlowData& flowData, bool copy = false) override
    {
        if (copy)
        {
            m_data = flowData.GetChannels();
            return;
        }

        for (auto& [id, channel] : flowData.GetChannels())
        {
            m_data[id].data.resize(channel.data.size());
            m_data[id].flags = channel.flags;
        }
    }

    virtual const std::map<uint32_t, Channel>& GetChannels() const override
    {
        return m_data;
    }

    virtual void* ToPtr(ParameterType type, uint32_t channel = 0) const override
    {
        if (m_data.find(channel) == m_data.end())
        {
            return nullptr;
        }

        if (type == m_parameterType)
        {
            if (m_data.at(channel).data.empty())
                return nullptr;
            return (void*)&m_data.at(channel).data[0];
        }

        auto psize = GetParameterTypeSize(type);
        m_buffer.resize(psize * m_numSamples);
        auto& data = m_data.at(channel).data;
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
   
    virtual float* ToFloatPtr(uint32_t channel = 0) const override
    {
        return (float*)ToPtr(ParameterType::Float, channel);
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
        *(float*)&m_data[0].data[0] = fVal;
    }

    virtual void FreeChannels()
    {
        m_data.clear();
    }

private:
    uint32_t m_flowType = 0;
    uint32_t m_numSamples = 1;
    uint32_t m_numChannels = 1;
    ParameterType m_parameterType;
    std::map<uint32_t, Channel> m_data;
    mutable std::vector<uint32_t> m_buffer;
};

} // namespace NodeGraph
