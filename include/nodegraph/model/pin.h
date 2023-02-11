#pragma once

#include <cassert>
#include <cstdint>
#include <unordered_set>
#include <variant>
#include <cstring>

#include <mutils/math/math.h>

#include "nodegraph/model/flow_data.h"
#include "nodegraph/model/parameter.h"
#include "nodegraph/view/layout_control.h"

namespace NodeGraph {

class Node;

enum class PinDir
{
    Input,
    Output
};

enum class Side
{
    Left, Right, Top, Bottom
};

// A pin on an audio node; connects (optionally) to another node.
class Pin : public Parameter, public MUtils::LayoutControl
{
public:
    Pin(Node& node, PinDir pinDir, const std::string& pinName, float fVal, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, double dVal, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, bool bVal, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, int64_t val, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, IFlowData* pData, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, const std::string& pData, const ParameterAttributes& attribs = {});

    Pin(Node& o, PinDir pinDir, const std::string& pinName, const Parameter& param);

    // Can't copy or assign pins
    Pin(const Pin& pin) = delete;
    void operator=(const Pin& pin) = delete;

    void ConnectTo(Pin& out)
    {
        assert(m_direction == PinDir::Output);
        m_targets.insert(&out);
        m_pSource = nullptr;
    }

    void Detach()
    {
        if (m_direction == PinDir::Input)
        {
            m_pSource = nullptr;
            assert(m_targets.empty());
        }
        else
        {
            m_targets.clear();
            assert(m_pSource == nullptr);
        }

        RemoveShadow();
    }

    virtual IFlowData* GetFlowData() const override
    {
        if (m_pSource == nullptr)
        {
            // might wind up null
            assert(m_value.type == ParameterType::FlowData);
            return Parameter::GetFlowData();
        }
        return m_pSource->GetFlowData();
    }
    
    virtual ParameterValue Update(uint64_t tick) override
    {
        if (!m_targets.empty() || m_pSource)
        {
            auto pData = GetFlowData();
            if (pData)
            {
                // Note we are working in bytes here; this needs to be nicely aligned for all types
                const uint32_t DisplayDataSize = 4096 * 4;

                // Never-lock-copy-if-can 
                MUtils::ProducerMemLock lk(m_outputDataDisplay);

                // Remove channels that aren't being displayed
                std::vector<uint32_t> victims;
                for (auto& [ch, data] : lk.Data())
                {
                    if (!pData->HasChannelId(ch))
                    {
                        victims.push_back(ch);
                    }
                }
                
                for (auto& v : victims)
                {
                    lk.Data().erase(v);
                }

                for (auto& chPair : pData->GetChannels())
                {
                    auto& data = lk.Data()[chPair.first];
                    data.resize(DisplayDataSize);

                    // Copy the new data onto the end of our buffer, shuffling the existing along
                    auto& vec = chPair.second.GetVector();
                    memmove(&data[0], &data[vec.size()], (DisplayDataSize - vec.size()));
                    memcpy(&data[DisplayDataSize - vec.size()], &vec[0], vec.size());

                    #ifdef DEBUG
                    if (pData->GetParameterType() == ParameterType::Float)
                    {
                        auto pFloat = (float*)&data[0];
                        for (uint32_t i =0; i < data.size() / sizeof(float); i++)
                        {
                            assert(std::isfinite(pFloat[i]));
                        }
                    }
                    #endif
                }
            }
        }
        return Parameter::Update(tick);
    }

    template <typename T>
    T GetValue() const
    {
        if (!m_pSource)
        {
            return Parameter::To<T>();
        }
        else
        {
            return m_pSource->GetValue<T>();
        }
    }

    ParameterValue& GetParameterValue()
    {
        return m_value;
    }

    // Only 1 source can be connected to this pin
    const Pin* GetSource() const
    {
        return m_pSource;
    }

    // This pin can connect to multiple targets
    const std::unordered_set<Pin*>& GetTargets() const
    {
        return m_targets;
    }

    Node& GetOwnerNode() const
    {
        return m_owner;
    }

    const std::string& GetName() const
    {
        return m_strName;
    }

    PinDir GetDirection() const
    {
        return m_direction;
    }

    void SetSource(Pin* pin)
    {
        m_pSource = pin;
    }

    void AddTarget(Pin* pin)
    {
        m_targets.insert(pin);
    }

    void RemoveTarget(Pin* pin)
    {
        m_targets.erase(pin);
    }

    void ClearTargets()
    {
        m_targets.clear();
    }

    void SetPadRect(MUtils::NRectf& pad, Side orient, Side location)
    {
        m_padRect = pad;
        m_padOrientation = orient;
        m_padLocation = location;
    }

    const MUtils::NRectf& GetPadRect() const
    {
        return m_padRect;
    }

    Side GetPadOrientation() const
    {
        return m_padOrientation; 
    }
    
    Side GetPadLocation() const
    {
        return m_padLocation; 
    }
    /* 
    const MUtils::NRectf& GetViewCells() const
    {
        return m_viewCells;
    }

    void SetViewCells(const MUtils::NRectf& cells)
    {
        m_viewCells = cells;
    }
    */

    double Normalized();
    double NormalizedStep() const;
    double NormalizedOrigin() const;
    void SetFromNormalized(double val);

    using flowDataMap_t = std::map<uint32_t, std::vector<uint8_t>>;
    
    MUtils::PNL_CL_Memory<flowDataMap_t, MUtils::audio_spin_mutex>& GetDisplayFlowData()
    {
        return m_outputDataDisplay;
    };
    uint32_t lastTriggerIndex = 0;

private:
    PinDir m_direction; // The direction of this pin
    std::string m_strName; // The name of this pin

    Node& m_owner; // Node that owns this pin

    std::unordered_set<Pin*> m_targets; // Which pins I'm connected to
    Pin* m_pSource = nullptr; // Which pin I'm connected from

    MUtils::NRectf m_viewCells = MUtils::NRectf(0, 0, 0, 0); // Cells that this parameter should be shown in for UI
    MUtils::NRectf m_padRect;
    Side m_padOrientation;
    Side m_padLocation;
    
    MUtils::PNL_CL_Memory<flowDataMap_t, MUtils::audio_spin_mutex> m_outputDataDisplay;
};

} // namespace NodeGraph
