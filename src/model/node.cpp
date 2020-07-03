#include <cassert>

#include "mutils/logger/logger.h"

#include "nodegraph/model/graph.h"
#include "nodegraph/model/node.h"
#include "nodegraph/model/pin.h"

using namespace MUtils;

namespace NodeGraph
{

uint64_t Node::CurrentId = 0;

Node::~Node()
{
    for (auto& input : m_inputs)
    {
        delete input;
    }
    for (auto& output : m_outputs)
    {
        delete output;
    }
    for (auto& decorator : m_decorators)
    {
        delete decorator;
    }
}

void Node::ClearDecorators()
{
    for (auto& decorator : m_decorators)
    {
        delete decorator;
    }
    m_decorators.clear();
}

void Node::ConnectIndexTo(Node* pDest, uint32_t outputIndex, int32_t inputIndex)
{
    if (m_outputs.size() <= (size_t)outputIndex)
    {
        throw std::invalid_argument("outputIndex too big");
    }

    if (pDest == this)
    {
        throw std::invalid_argument("Cannot connect to the same node");
    }

    if (inputIndex >= 0)
    {
        if (pDest->GetInputs().size() <= (size_t)inputIndex)
        {
            throw std::invalid_argument("inputIndex too big");
        }

        if (pDest->GetInputs()[inputIndex]->GetTargets().size() > 0)
        {
            throw std::invalid_argument("Can't connect more than one signal to the same input");
        }

        if (pDest->GetInputs()[inputIndex]->GetType() != m_outputs[outputIndex]->GetType())
        {
            throw std::invalid_argument("Types don't match on pins");
        }
    }
    else
    {
        if (m_outputs[outputIndex]->GetType() == ParameterType::FlowData)
        {
            int size = (int)pDest->GetFlowInputs().size();
            pDest->AddInput(std::string("Flow_") + std::to_string(size), (IFlowData*)nullptr);
            inputIndex = size;
        }
        else if (m_outputs[outputIndex]->GetType() == ParameterType::ControlData)
        {
            int size = (int)pDest->GetControlInputs().size();
            pDest->AddInput(std::string("Control_") + std::to_string(size), (IControlData*)nullptr);
            inputIndex = size;
        }
        else
        {
            throw std::invalid_argument("Can only generate inputs of flow data type");
        }
    }

    // Connect it up
    m_outputs[outputIndex]->AddTarget(pDest->GetInputs()[inputIndex]);
    pDest->GetInputs()[inputIndex]->SetSource(m_outputs[outputIndex]);

    m_graph.SetLayoutModified(true);
}

void Node::ConnectTo(Node* pDest, const std::string& outputName, const std::string& inName)
{
    std::string searchOutputName;
    if (!outputName.empty())
    {
        searchOutputName = outputName;
    }
    else
    {
        searchOutputName = "Flow";
    }

    Pin* pOut = nullptr;
    for (auto& pPin : m_outputs)
    {
        if (pPin->GetName() == searchOutputName)
        {
            pOut = pPin;
            break;
        }
    }

    if (pOut == nullptr)
    {
        throw std::invalid_argument("Can't find output pin: " + searchOutputName);
    }

    Pin* pIn = nullptr;
    if (!inName.empty() && inName != str_AutoGen)
    {
        // Provided a name for the input, find it
        for (auto& pPin : pDest->GetInputs())
        {
            if (pPin->GetName() == inName)
            {
                pIn = pPin;
                break;
            }
        }
        if (pIn == nullptr)
        {
            throw std::invalid_argument("Can't find input pin: " + inName);
        }
    }
    else
    {
        // Try to match output name
        for (auto& pPin : pDest->GetInputs())
        {
            if (pPin->GetName() == searchOutputName)
            {
                pIn = pPin;
                break;
            }
        }

        // Auto gen or nothing
        if (!pIn)
        {
            // Auto connecting flow
            if (pOut->GetType() == ParameterType::FlowData)
            {
                int size = (int)pDest->GetFlowInputs().size();
                pIn = pDest->AddInput(std::string("Flow_") + std::to_string(size), (IFlowData*)nullptr);
            }
            else if (pOut->GetType() == ParameterType::ControlData)
            {
                int size = (int)pDest->GetControlInputs().size();
                pIn = pDest->AddInput(std::string("Control_") + std::to_string(size), (IControlData*)nullptr);
            }
            else
            {
                throw std::invalid_argument("Can only generate inputs of flow data type");
            }
        }
    }

    if (pDest == this)
    {
        throw std::invalid_argument("Cannot connect to the same node");
    }

    if (pIn->GetSource() != nullptr)
    {
        throw std::invalid_argument("Can't connect more than one signal to the same input");
    }

    if (pOut->GetSource() != nullptr)
    {
        throw std::invalid_argument("Can't connect more than one signal to the same input");
    }

    // Connect it up
    pOut->AddTarget(pIn);
    pIn->SetSource(pOut);

    m_graph.SetLayoutModified(true);
}

void Node::Compute()
{
    /* Default compute; do nothing */
}

// TODO: Optimize with a map later
Pin* Node::GetPin(const std::string& name) const
{
    for (auto& p : m_inputs)
    {
        if (p->GetName() == name)
        {
            return p;
        }
    }

    for (auto& p : m_outputs)
    {
        if (p->GetName() == name)
        {
            return p;
        }
    }
    return nullptr;
}
Pin* Node::AddInput(const std::string& strName, IFlowData* val, const ParameterAttributes& attrib)
{
    auto pPin = new Pin(*this, PinDir::Input, strName, val, attrib);
    m_inputs.push_back(pPin);
    m_flowInputs.push_back(pPin);
    m_graph.SetLayoutModified(true);
    return pPin;
}

Pin* Node::AddInput(const std::string& strName, IControlData* val, const ParameterAttributes& attrib)
{
    auto pPin = new Pin(*this, PinDir::Input, strName, val, attrib);
    m_inputs.push_back(pPin);
    m_controlInputs.push_back(pPin);
    m_graph.SetLayoutModified(true);
    return pPin;
}

Pin* Node::AddOutput(const std::string& strName, IFlowData* val, const ParameterAttributes& attrib)
{
    auto pPin = new Pin(*this, PinDir::Output, strName, val, attrib);
    m_outputs.push_back(pPin);
    m_flowOutputs.push_back(pPin);
    m_graph.SetLayoutModified(true);
    return pPin;
}

Pin* Node::AddOutput(const std::string& strName, IControlData* val, const ParameterAttributes& attrib)
{
    auto pPin = new Pin(*this, PinDir::Output, strName, val, attrib);
    m_outputs.push_back(pPin);
    m_controlOutputs.push_back(pPin);
    m_graph.SetLayoutModified(true);
    return pPin;
}

NodeDecorator* Node::AddDecorator(NodeDecorator* decorator)
{
    m_decorators.push_back(decorator);
    return decorator;
}

const std::vector<NodeDecorator*>& Node::GetDecorators() const
{
    return m_decorators;
}

const MUtils::NVec2f Node::GetGridScale() const
{
    return m_gridScale;
}

const MUtils::NRectf& Node::GetCustomViewCells() const
{
    return m_viewCells;
}

void Node::SetCustomViewCells(const MUtils::NRectf& cells)
{
    m_viewCells = cells;
}

} // namespace NodeGraph
