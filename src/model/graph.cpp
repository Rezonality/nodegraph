#include <exception>
#include <stdexcept>

#include "mutils/logger/logger.h"
#include "mutils/thread/threadutils.h"

#include "nodegraph/model/graph.h"

using namespace MUtils;

namespace NodeGraph
{

Graph::Graph()
    : m_strName("Graph")
{
}

Graph::~Graph()
{
    Clear();
}

void Graph::DestroyNode(Node* pNode)
{
    if (!pNode)
    {
        return;
    }
    PreModify();

    nodes.erase(pNode);
    m_displayNodes.erase(pNode);
    m_outputNodes.erase(pNode);
    m_mapIdToNode.erase(pNode->GetId());

    delete pNode;

    PostModify();
}

void Graph::Clear()
{
    GRAPH_MODIFY(*this);

    while (!nodes.empty())
    {
        DestroyNode(*nodes.begin());
    }

    currentGeneration = 1;
}

void Graph::Visit(Node& node, PinDir dir, ParameterType type, std::function<bool(Node&)> fn)
{
    fn(node);

    if (dir == PinDir::Input)
    {
        for (auto& in : node.GetInputs())
        {
            if (type == ParameterType::None || type == in->GetType())
            {
                auto pSource = in->GetSource();
                if (pSource)
                {
                    assert(&pSource->GetOwnerNode() != &node);
                    Visit(pSource->GetOwnerNode(), dir, type, fn);
                }
            }
        }
    }
    else
    {
        for (auto& out : node.GetOutputs())
        {
            if (type == ParameterType::None || type == out->GetType())
            {
                for (auto& pTarget : out->GetTargets())
                {
                    assert(&pTarget->GetOwnerNode() != &node);
                    Visit(pTarget->GetOwnerNode(), dir, type, fn);
                }
            }
        }
    }
}

void Graph::Compute(const std::set<Node*>& outNodes, int64_t numTicks)
{
    PROFILE_SCOPE(Graph_Compute);

    currentGeneration++;

    using fnEval = std::function<void(Node * pEvalNode)>;
    fnEval eval = [&](Node* pEvalNode) {
        // Don't re-evaluate
        if (pEvalNode->GetGeneration() == currentGeneration)
            return;

        std::set<Node*> sourceEvalNodes;

        for (auto& pin : pEvalNode->GetInputs())
        {
            if (pin->GetDirection() == PinDir::Input && (pin->GetType() == ParameterType::FlowData || pin->GetType() == ParameterType::ControlData))
            {
                auto pSource = pin->GetSource();
                if (pSource != nullptr && pSource->GetOwnerNode().GetGeneration() != currentGeneration)
                {
                    auto pSourceNode = &pSource->GetOwnerNode();
                    sourceEvalNodes.insert(pSourceNode);
                }
            }
        }

        // Walk up to the parents, evaluating them
        for (auto& pSource : sourceEvalNodes)
        {
            eval(pSource);
        }

        // Portmento updates
        for (auto& pin : pEvalNode->GetInputs())
        {
            pin->Update(numTicks);
        }

        //LOG(DEBUG) << "Computing: " << pEvalNode->GetType().name();

        // Compute the node
        pEvalNode->Compute();

        // Output portmento
        for (auto& pin : pEvalNode->GetOutputs())
        {
            pin->Update(numTicks);
        }

        // It is now at the current generation
        pEvalNode->SetGeneration(currentGeneration);
    };

    for (auto& pNode : outNodes)
    {
        eval(pNode);
    };
}

std::vector<Pin*> Graph::GetControlSurface() const
{
    // All pins that have interesting data to display
    std::vector<Pin*> pins;
    for (auto& pNode : nodes)
    {
        for (auto& in : pNode->GetInputs())
        {
            if ((in->GetSource() == nullptr) && (in->GetType() != ParameterType::FlowData) && (in->GetType() != ParameterType::ControlData))
            {
                pins.emplace_back(in);
            }
        }
        for (auto& in : pNode->GetOutputs())
        {
            if ((in->GetType() != ParameterType::FlowData) && (in->GetType() != ParameterType::ControlData))
            {
                pins.emplace_back(in);
            }
        }
    }
    return pins;
}

// Called to notify that this graph is about to be destroyed
void Graph::NotifyDestroy(Graph* pGraph)
{
    sigDestroy(pGraph);
}

void Graph::SetName(const std::string& name)
{
    m_strName = name;
}

std::string Graph::Name() const
{
    return m_strName;
}

// Helper to make template header function compile with forward ref
bool Graph::IsType(Node& node, ctti::type_id_t type) const
{
    return node.GetType() == type;
}

GraphModify::GraphModify(Graph& graph)
    : m_graph(graph)
{
    m_graph.PreModify();
}

GraphModify::~GraphModify()
{
    m_graph.PostModify();
}

} // namespace NodeGraph
