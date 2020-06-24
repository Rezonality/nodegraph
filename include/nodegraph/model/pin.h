#pragma once

#include "nodegraph/model/parameter.h"
#include <cassert>
#include <cstdint>
#include <variant>
#include <unordered_set>

#include <mutils/math/math.h>

namespace NodeGraph
{

class Node;

class IFlowData
{
public:
    virtual ~IFlowData() {}
};

class IControlData
{
public:
    virtual ~IControlData() {}
};


enum class PinDir
{
    Input,
    Output
};

// A pin on an audio node; connects (optionally) to another node.
class Pin : public Parameter
{
public:
    Pin(Node& node, PinDir pinDir, const std::string& pinName, float fVal, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, double dVal, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, bool bVal, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, int64_t val, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, IFlowData* pData, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, IControlData* pData, const ParameterAttributes& attribs = {});
    Pin(Node& node, PinDir pinDir, const std::string& pinName, const std::string& pData, const ParameterAttributes& attribs = {});

    Pin(Node& o, PinDir pinDir, const std::string& pinName, const Parameter& param);

    // Can't copy or assign pins
    Pin(const Pin& pin) = delete;
    void operator=(const Pin& pin) = delete;

    void ConnectTo(Pin& out)
    {
        m_targets.insert(&out);
        m_pSource = nullptr;
    }

    virtual IFlowData* GetFlowData() const override
    {
        assert(m_value.type == ParameterType::FlowData);
        if (m_pSource == nullptr)
        {
            // might wind up null
            return Parameter::GetFlowData();
        }
        return m_pSource->GetFlowData();
    }
    
    virtual IControlData* GetControlData() const override
    {
        assert(m_value.type == ParameterType::ControlData);
        if (m_pSource == nullptr)
        {
            // might wind up null
            return Parameter::GetControlData();
        }
        return m_pSource->GetControlData();
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

    const MUtils::NRectf& GetViewCells() const
    {
        return m_viewCells;
    }

    void SetViewCells(const MUtils::NRectf& cells)
    {
        m_viewCells = cells;
    }

private:
    PinDir m_direction;                     // The direction of this pin
    std::string m_strName;                  // The name of this pin

    Node& m_owner;                          // Node that owns this pin

    std::unordered_set<Pin*> m_targets;     // Which pins I'm connected to
    Pin* m_pSource = nullptr;               // Which pin I'm connected from

    MUtils::NRectf m_viewCells = MUtils::NRectf(0, 0, 0, 0);            // Cells that this parameter should be shown in for UI
};


} // namespace NodeGraph
