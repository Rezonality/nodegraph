#pragma once

#include <cassert>
#include <exception>
#include <functional>
#include <set>

#include "mutils/profile/profile.h"

#include "threadpool/threadpool.h"

#include "nodegraph/model/node.h"
#include "nodegraph/model/pin.h"

namespace NodeGraph
{

// A collection of nodes
class Graph
{
public:
    Graph();
    virtual ~Graph();

    void Destroy();

    // Use this method to create nodes and add them to the m_graph
    template <typename T, typename... Args>
    T* CreateNode(Args&&... args)
    {
        auto pNode = std::make_shared<T>(*this, std::forward<Args>(args)...);
        nodes.insert(pNode);
        m_displayNodes.push_back(pNode.get());
        return pNode.get();
    }

    template <class T>
    std::set<T*> Find(ctti::type_id_t type) const
    {
        std::set<T*> found;
        for (auto& pNode : nodes)
        {
            if (pNode->GetType() == type)
                found.insert(static_cast<T*>(pNode.get()));
        }
        return found;
    }

    template <class T>
    std::set<T*> Find(const std::vector<ctti::type_id_t>& nodeTypes) const
    {
        std::set<T*> found;
        for (auto& t : nodeTypes)
        {
            auto f = Find<T>(t);
            found.insert(f.begin(), f.end());
        }
        return found;
    }

    void Visit(Node& node, PinDir dir, ParameterType type, std::function<bool(Node&)> fn);

    // Get the list of pins that could be on the UI
    std::vector<Pin*> GetControlSurface() const;

    void Compute(const std::vector<Node*>& nodes, int64_t numTicks);

    const std::set<std::shared_ptr<Node>>& GetNodes() const { return nodes; }

    TPool& ThreadPool() { return m_threadPool; }

    const std::vector<Node*>& GetDisplayNodes() const { return m_displayNodes; }
    void SetDisplayNodes(const std::vector<Node*>& nodes) { m_displayNodes = nodes; }
   
    const std::vector<Node*>& GetOutputNodes() const { return m_outputNodes; }
    void SetOutputNoes(const std::vector<Node*>& nodes) { m_outputNodes = nodes; }
protected:
    std::set<std::shared_ptr<Node>> nodes;
    std::vector<Node*> m_displayNodes;
    uint64_t currentGeneration = 1;
    TPool m_threadPool;
    std::vector<Node*> m_outputNodes;
}; // Graph

} // namespace NodeGraph
