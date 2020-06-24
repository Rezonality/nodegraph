#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <functional>

#include <ctti/type_id.hpp>

#include "pin.h"

struct NVGcontext;

namespace NodeGraph
{

class GraphView;
class Canvas;
class Node;

#define DECLARE_NODE(className, APIName)                  \
    static ctti::type_id_t TypeID()                         \
    {                                                       \
        return ctti::type_id<className>();                  \
    }                                                       \
    virtual ctti::type_id_t GetType() const override        \
    {                                                       \
        return TypeID();                                    \
    } \
    virtual const char* GetAPIName() const override { return #APIName; }

constexpr auto str_AutoGen = "auto";

class Graph;

enum class DecoratorType
{
    Label,
    Line
};

struct NodeDecorator
{
    NodeDecorator(DecoratorType t, const std::string& name)
        : type(t),
        strName(name)
    {
    }
    
    NodeDecorator(DecoratorType t)
        : type(t)
    {
    }

    DecoratorType type = DecoratorType::Label;
    std::string strName;
    MUtils::NRectf gridLocation;
};

// A node with inputs, outputs and computation
class Node
{
public:
    explicit Node(Graph& m_graph, const std::string& name)
        : m_strName(name),
        m_graph(m_graph),
        m_Id(CurrentId++)
    {
    };

    Node(const Node& node) = delete;
    const Node& operator =(const Node& Node) = delete;

    virtual ~Node();

    void ConnectTo(Node* pTarget, uint32_t outPin = 0, int32_t targetPin = 0);
    void ConnectTo(Node* pDest, const std::string& outputName, const std::string& inputName = "");

    virtual void Compute();

    // Set
    void SetGeneration(uint64_t gen) { m_generation = gen; }

    // Get
    virtual ctti::type_id_t GetType() const = 0;
    virtual const char* GetAPIName() const = 0;
    const std::string& GetName() const { return m_strName; }
    const std::vector<Pin*>& GetInputs() const { return m_inputs; }
    const std::vector<Pin*>& GetOutputs() const { return m_outputs; }
    const uint64_t GetGeneration() const { return m_generation; }
    
    const std::vector<Pin*>& GetControlInputs() const { return m_controlInputs; }
    const std::vector<Pin*>& GetControlOutputs() const { return m_controlOutputs; }
    
    const std::vector<Pin*>& GetFlowInputs() const { return m_flowInputs; }
    const std::vector<Pin*>& GetFlowOutputs() const { return m_flowOutputs; }

    Pin* GetPin(const std::string& name) const;

    // Make an output pin
    template<class T>
    Pin* AddOutput(const std::string& strName, T val, const ParameterAttributes& attrib = ParameterAttributes{})
    {
        m_outputs.push_back(new Pin(*this, PinDir::Output, strName, val, attrib));
        return m_outputs[m_outputs.size() - 1];
    }
    
    Pin* AddOutput(const std::string& strName, IFlowData* val, const ParameterAttributes& attrib = ParameterAttributes{})
    {
        auto pPin = new Pin(*this, PinDir::Output, strName, val, attrib);
        m_outputs.push_back(pPin);
        m_flowOutputs.push_back(pPin);
        return pPin;
    }
    
    Pin* AddOutput(const std::string& strName, IControlData* val, const ParameterAttributes& attrib = ParameterAttributes{})
    {
        auto pPin = new Pin(*this, PinDir::Output, strName, val, attrib);
        m_outputs.push_back(pPin);
        m_controlOutputs.push_back(pPin);
        return pPin;
    }

    // Make an input pin
    template<class T>
    Pin* AddInput(const std::string& strName, T val, const ParameterAttributes& attrib = ParameterAttributes{})
    {
        m_inputs.push_back(new Pin(*this, PinDir::Input, strName, val, attrib));
        return m_inputs[m_inputs.size() - 1];
    }
    
    Pin* AddInput(const std::string& strName, IFlowData* val, const ParameterAttributes& attrib = ParameterAttributes{})
    {
        auto pPin = new Pin(*this, PinDir::Input, strName, val, attrib);
        m_inputs.push_back(pPin);
        m_flowInputs.push_back(pPin);
        return pPin;
    }
    
    Pin* AddInput(const std::string& strName, IControlData* val, const ParameterAttributes& attrib = ParameterAttributes{})
    {
        auto pPin = new Pin(*this, PinDir::Input, strName, val, attrib);
        m_inputs.push_back(pPin);
        m_controlInputs.push_back(pPin);
        return pPin;
    }

    NodeDecorator* AddDecorator(NodeDecorator* decorator)
    {
        m_decorators.push_back(decorator);
        return decorator;
    }

    const std::vector<NodeDecorator*>& GetDecorators() const
    {
        return m_decorators;
    }

    virtual const MUtils::NVec2f GetGridScale() const
    {
        return m_gridScale;
    }

    const MUtils::NRectf& GetCustomViewCells() const
    {
        return m_viewCells;
    }

    void SetCustomViewCells(const MUtils::NRectf& cells)
    {
        m_viewCells = cells;
    }

    virtual void DrawCustom(GraphView& view, Canvas& canvas, const MUtils::NRectf&) { };
    virtual void DrawCustomPin(GraphView& view, Canvas& canvas, const MUtils::NRectf&, Pin& pin) { };

    bool IsHidden() const { return m_hidden; }
    void SetHidden(bool hidden) { m_hidden = hidden; }

    Graph& GetGraph() const
    {
        return m_graph;
    }

    uint64_t GetId() const
    {
        return m_Id;
    }

protected:
    uint64_t m_Id;
    static uint64_t CurrentId;
    ctti::type_id_t m_nodeType;
    std::string m_strName;
    std::vector<Pin*> m_inputs;
    std::vector<Pin*> m_outputs;
    std::vector<Pin*> m_flowInputs;
    std::vector<Pin*> m_flowOutputs;
    std::vector<Pin*> m_controlInputs;
    std::vector<Pin*> m_controlOutputs;
    std::vector<NodeDecorator*> m_decorators;
    uint64_t m_generation = 0;
    MUtils::NRectf m_viewCells;
    MUtils::NVec2f m_gridScale = MUtils::NVec2f(1.0f);
    bool m_hidden = false;
    Graph& m_graph;
};

// A node that has no inputs/outputs or parameters
class EmptyNode : public Node
{
public:
    DECLARE_NODE(EmptyNode, empty);

    EmptyNode(Graph& m_graph, const std::string& strName)
        : Node(m_graph, strName)
    {
    }
};

} /// NodeGraph
