#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include <ctti/type_id.hpp>

#include "pin.h"

#include <nodegraph/model/graph.h>

struct NVGcontext;

namespace NodeGraph
{

class GraphView;
class Canvas;
class Node;
class Graph;
class ViewNode;

struct NodeLayout;

class GraphModify final
{
public:
    GraphModify(Graph& graph);
    ~GraphModify();
    Graph& m_graph;
};

#define GRAPH_MODIFY(a) NodeGraph::GraphModify __graphModify(a);

#define DECLARE_NODE(className, APIName)             \
    static ctti::type_id_t TypeID()                  \
    {                                                \
        return ctti::type_id<className>();           \
    }                                                \
    static const char* StaticAPIName()               \
    {                                                \
        return #APIName;                             \
    }                                                \
    virtual ctti::type_id_t GetType() const override \
    {                                                \
        return TypeID();                             \
    }                                                \
    virtual const char* GetAPIName() const override  \
    {                                                \
        return #APIName;                             \
    }

constexpr auto str_AutoGen = "auto";

enum class DecoratorType
{
    Label,
    Line
};

struct NodeDecorator
{
    NodeDecorator(DecoratorType t, const std::string& name)
        : type(t)
        , strName(name)
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

namespace NodeFlags
{

enum
{
    None = (0),
    Hidden = (1 << 0),
    OwnerDraw = (1 << 1)
};

};

// A node with inputs, outputs and computation
class Node
{
public:
    explicit Node(Graph& m_graph, const std::string& name);
    Node(const Node& node) = delete;
    const Node& operator=(const Node& Node) = delete;

    virtual void Detach();
    virtual ~Node();

    virtual void ConnectIndexTo(Node* pTarget, uint32_t outPin = 0, int32_t targetPin = 0);
    virtual void ConnectTo(Node* pDest, const std::string& outputName = "Flow", const std::string& inputName = "");

    virtual void Compute();

    // Set
    void SetGeneration(uint64_t gen)
    {
        m_generation = gen;
    }

    // Get
    virtual ctti::type_id_t GetType() const = 0;
    virtual const char* GetAPIName() const = 0;

    const std::string& GetName() const
    {
        return m_strName;
    }
    const std::vector<Pin*>& GetInputs() const
    {
        return m_inputs;
    }
    const std::vector<Pin*>& GetOutputs() const
    {
        return m_outputs;
    }
    const uint64_t GetGeneration() const
    {
        return m_generation;
    }

    const std::vector<Pin*>& GetControlInputs() const
    {
        return m_controlInputs;
    }
    const std::vector<Pin*>& GetControlOutputs() const
    {
        return m_controlOutputs;
    }

    const std::vector<Pin*>& GetFlowInputs() const
    {
        return m_flowInputs;
    }
    const std::vector<Pin*>& GetFlowOutputs() const
    {
        return m_flowOutputs;
    }

    const std::vector<Pin*>& GetFlowControlInputs() const;
    const std::vector<Pin*>& GetFlowControlOutputs() const;

    Pin* GetPin(const std::string& name) const;

    // Make an output pin
    template<typename T, typename = std::enable_if_t<!std::is_pointer<T>::value>>
    Pin* AddOutput(const std::string& strName, T val, const ParameterAttributes& attrib = ParameterAttributes{})
    {
        GraphModify __modify(m_graph);
        m_outputs.push_back(new Pin(*this, PinDir::Output, strName, val, attrib));
        return m_outputs[m_outputs.size() - 1];
    }

    Pin* AddOutputFlow(const std::string& strName, IFlowData* val, const ParameterAttributes& attrib = ParameterAttributes{});

    // Make an input pin
    template<typename T, typename = std::enable_if_t<!std::is_pointer<T>::value>>
    Pin* AddInput(const std::string& strName, T val, const ParameterAttributes& attrib = ParameterAttributes{})
    {
        GraphModify __modify(m_graph);
        m_inputs.push_back(new Pin(*this, PinDir::Input, strName, val, attrib));
        return m_inputs[m_inputs.size() - 1];
    }

    Pin* AddInputFlow(const std::string& strName, IFlowData* val, const ParameterAttributes& attrib = ParameterAttributes{});

    NodeDecorator* AddDecorator(NodeDecorator* decorator);

    const std::vector<NodeDecorator*>& GetDecorators() const;
    void ClearDecorators();

    virtual const MUtils::NVec2f GetGridScale() const;
    const MUtils::NRectf& GetCustomViewCells() const;
    void SetCustomViewCells(const MUtils::NRectf& cells);

    virtual void PreDraw(){};
    virtual void Draw(GraphView&, Canvas&, ViewNode&);
    virtual void DrawCustom(GraphView& view, Canvas& canvas, const MUtils::NRectf&){};
    virtual void DrawCustomPin(GraphView& view, Canvas& canvas, const MUtils::NRectf&, Pin& pin){};

    uint32_t Flags() const;
    void SetFlags(uint32_t flags);

    Graph& GetGraph() const
    {
        return m_graph;
    }

    uint64_t GetId() const
    {
        return m_Id;
    }

    const MUtils::NVec2f& GetPos() const
    {
        return m_viewPos;
    }

    void SetPos(const MUtils::NVec2f& pos)
    {
        m_viewPos = pos;
    }

    MUtils::NVec2f GetCenter() const;

    NodeLayout& GetLayout()
    {
        return *m_spLayout;
    }

    nod::signal<void(Node*)> sigDetach;
    nod::signal<void(Node*)> sigDestroy;

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
    mutable std::vector<Pin*> m_flowControlInputs;
    mutable std::vector<Pin*> m_flowControlOutputs;
    std::vector<NodeDecorator*> m_decorators;
    uint64_t m_generation = 0;
    MUtils::NRectf m_viewCells;
    MUtils::NVec2f m_gridScale = MUtils::NVec2f(1.0f);
    MUtils::NVec2f m_viewPos;
    uint32_t m_flags = NodeFlags::None;
    Graph& m_graph;
    std::shared_ptr<NodeLayout> m_spLayout;
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

} // namespace NodeGraph
