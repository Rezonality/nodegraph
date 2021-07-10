#pragma once
#include "nodegraph/model/parameter.h"
#include <map>

namespace NodeGraph {

namespace ChannelFlags {
enum
{
    None = 0,
};
}
class Channel
{
public:
    Channel(uint32_t size = 0)
        : data(size, 0)
    {
    }

    template <typename T>
    T* Ptr(uint32_t index = 0) const
    {
        if (data.empty())
        {
            return (T*)nullptr;
        }
        return (T*)&data[sizeof(T) * index]; 
    }
    
    template <typename T>
    T& Val(uint32_t index = 0) const
    {
        assert(data.size() > (sizeof(T) * index));
        return (T&)data[sizeof(T) * index]; 
    }

    uint32_t ByteSize() const
    {
        return uint32_t(data.size());
    }

    void SetSizeInBytes(uint32_t size)
    {
        data.resize(size, 0);
    }

    void SetFrom(const std::vector<uint8_t>& rhs)
    {
        data = rhs; 
    }

    const std::vector<uint8_t>& GetVector() const
    {
        return data; 
    }

    uint32_t flags = ChannelFlags::None;

private:
    std::vector<uint8_t> data;
};

class IFlowData
{
public:
    virtual uint32_t GetFlowType() const = 0; // e.g. modulate/midi/audio
    virtual ParameterType GetParameterType() const = 0;
    virtual uint32_t GetNumSamples(uint32_t id) const = 0; // How many data samples this flow contains
    virtual uint32_t GetNumChannels() const = 0; // Number of data channels this flow has
    virtual bool HasChannelId(uint32_t id) const = 0;
    virtual Channel* GetChannelById(uint32_t id, uint32_t size) = 0;
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

class FlowData : public IFlowData
{
public:
    FlowData(uint32_t flowType, ParameterType type)
        : m_flowType(flowType)
        , m_parameterType(type)
    {
    }
    virtual ~FlowData()
    {
    }

    virtual uint32_t GetFlowType() const override
    {
        return m_flowType;
    }

    virtual uint32_t GetNumSamples(uint32_t id) const override
    {
        if (m_data.find(id) == m_data.end())
        {
            return 0; 
        }
        return uint32_t(m_data.at(id).ByteSize() / GetParameterTypeSize(m_parameterType));
    }

    virtual ParameterType GetParameterType() const override
    {
        return m_parameterType;
    }

    virtual uint32_t GetNumChannels() const override
    {
        return (uint32_t)m_data.size();
    }

    virtual bool HasChannelId(uint32_t id) const override
    {
        return m_data.find(id) != m_data.end();
    }

    virtual Channel* GetChannelById(uint32_t id, uint32_t numSamples) override
    {
        auto channelSize = numSamples * GetParameterTypeSize(m_parameterType);
        if (m_data.find(id) != m_data.end())
        {
            m_data[id].SetSizeInBytes(channelSize);
            return &m_data[id];
        }

        m_data[id] = Channel(channelSize);
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
            m_data[id].SetSizeInBytes(channel.ByteSize());
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
            if (m_data.at(channel).ByteSize() == 0)
                return nullptr;
            return m_data.at(channel).Ptr<uint8_t>();
        }

        if (m_data.empty())
        {
            return nullptr;
        }

        auto& ch = m_data.at(channel);
        auto numSamples = ch.ByteSize() / GetParameterTypeSize(m_parameterType);
       
        // Size the temporary buffer : TODO cleaner methods for this
        auto psize = GetParameterTypeSize(type);
        m_buffer.resize(psize * numSamples);
        for (uint32_t sample = 0; sample < numSamples; sample++)
        {
            switch (type)
            {
            case ParameterType::Float:
            {
                float* pTarget = (float*)&m_buffer[sample * psize];
                switch (m_parameterType)
                {
                case ParameterType::Float:
                    *pTarget = ch.Val<float>(sample);
                    break;
                case ParameterType::Bool:
                    *pTarget = ch.Val<bool>(sample) ? 1.0f : 0.0f;
                    break;
                case ParameterType::Double:
                    *pTarget = (float)ch.Val<double>(sample);
                    break;
                case ParameterType::Int64:
                    *pTarget = (float)ch.Val<int64_t>(sample);
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
                    *pTarget = (double)ch.Val<float>(sample);
                    break;
                case ParameterType::Bool:
                    *pTarget = ch.Val<bool>(sample) ? 1.0 : 0.0;
                    break;
                case ParameterType::Double:
                    *pTarget = ch.Val<double>(sample);
                    break;
                case ParameterType::Int64:
                    *pTarget = (double)ch.Val<int64_t>(sample);
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

    // Given a single float, make a flow data that represents it
    virtual void From(float fVal, uint32_t channel = 0) override
    {
        m_parameterType = ParameterType::Float;
        m_data[channel].SetSizeInBytes(GetParameterTypeSize(m_parameterType));
        m_data[channel].Val<float>(0) = fVal;
    }

    virtual void FreeChannels()
    {
        m_data.clear();
    }

private:
    uint32_t m_flowType = 0;
    ParameterType m_parameterType;
    std::map<uint32_t, Channel> m_data;
    mutable std::vector<uint32_t> m_buffer;
};

} // namespace NodeGraph
