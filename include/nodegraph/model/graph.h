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

// A collection of nodes that can be computed
class Graph
{
public:
    Graph();
    virtual ~Graph();

    virtual void Clear();

    // Use this method to create nodes and add them to the m_graph
    template <typename T, typename... Args>
    T* CreateNode(Args&&... args)
    {
        PreModifyGraph();

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
    virtual std::vector<Pin*> GetControlSurface() const;

    virtual void Compute(const std::vector<Node*>& nodes, int64_t numTicks);
    
    virtual void PreModifyGraph() { m_modified = true; }
    virtual bool HasModified() const { return m_modified; }
    virtual void ResetModified() { m_modified = false; }

    const std::set<std::shared_ptr<Node>>& GetNodes() const { return nodes; }

    const std::vector<Node*>& GetDisplayNodes() const { return m_displayNodes; }
    void SetDisplayNodes(const std::vector<Node*>& nodes) { m_displayNodes = nodes; }
   
    const std::vector<Node*>& GetOutputNodes() const { return m_outputNodes; }
    void SetOutputNodes(const std::vector<Node*>& nodes) { m_outputNodes = nodes; }

protected:
    std::set<std::shared_ptr<Node>> nodes;
    std::vector<Node*> m_displayNodes;
    uint64_t currentGeneration = 1;
    std::vector<Node*> m_outputNodes;
    bool m_modified = false;
}; // Graph

} // namespace NodeGraph
