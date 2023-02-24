#pragma once

#include <cassert>
#include <exception>
#include <functional>
#include <set>
#include <map>

#include <nod/nod.hpp>

#include <gsl-lite/gsl-lite.hpp>

#include "mutils/time/profiler.h"

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
        PreModify();

        auto pNode = new T(*this, std::forward<Args>(args)...);
        nodes.insert(pNode);
        m_displayNodes.insert(pNode);
        m_mapIdToNode[pNode->GetId()] = pNode;

        PostModify();
        return pNode;
    }

    void DestroyNode(Node* pNode);

    bool IsType(Node& node, ctti::type_id_t type) const;

    template <class T>
    std::set<T*> Find(ctti::type_id_t type) const
    {
        std::set<T*> found;
        for (auto& pNode : nodes)
        {
            if (IsType(*pNode, type))
            {
                found.insert(static_cast<T*>(pNode));
            }
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

    virtual void Compute(const std::set<Node*>& nodes, int64_t numTicks);

    const std::set<Node*>& GetNodes() const
    {
        return nodes;
    }

    const std::set<Node*>& GetDisplayNodes() const
    {
        return m_displayNodes;
    }
    void SetDisplayNodes(const std::set<Node*>& nodes)
    {
        m_displayNodes = nodes;
    }

    const std::set<Node*>& GetOutputNodes() const
    {
        return m_outputNodes;
    }
    void SetOutputNodes(const std::set<Node*>& nodes)
    {
        m_outputNodes = nodes;
    }

    void PreModify()
    {
        if (m_modifyTracker == 0)
        {
            sigBeginModify(this);
        }

        m_modifyTracker++;
    }

    void PostModify()
    {
        assert(m_modifyTracker > 0);
        m_modifyTracker--;
        if (m_modifyTracker == 0)
        {
            sigEndModify(this);
        }
    }

    void SetName(const std::string& name);
    std::string Name() const;

    // Called to notify that this graph is about to be destroyed
    void NotifyDestroy(Graph* pGraph);

    const std::map<uint64_t, Node*>& GetNodesById() const
    {
        return m_mapIdToNode;
    }

    // Signals
    nod::signal<void(Graph*)> sigBeginModify;
    nod::signal<void(Graph*)> sigEndModify;
    nod::signal<void(Graph*)> sigDestroy;

protected:
    uint32_t m_modifyTracker = 0;

    std::map<uint64_t, Node*> m_mapIdToNode;

    std::set<Node*> nodes;
    std::set<Node*> m_displayNodes;
    std::set<Node*> m_outputNodes;

    uint64_t currentGeneration = 1;
    std::string m_strName;
}; // Graph

} // namespace NodeGraph
