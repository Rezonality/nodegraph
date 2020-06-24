#include <cassert>

#include "mutils/logger/logger.h"

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

void Node::ConnectTo(Node* pDest, uint32_t outputIndex, int32_t inputIndex)
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
            pDest->AddInput(std::string("FlowIn_") + std::to_string(size), (IFlowData*)nullptr);
            inputIndex = size;
        }
        else if (m_outputs[outputIndex]->GetType() == ParameterType::ControlData)
        {
            int size = (int)pDest->GetControlInputs().size();
            pDest->AddInput(std::string("ControlIn_") + std::to_string(size), (IControlData*)nullptr);
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
}

void Node::ConnectTo(Node* pDest, const std::string& outputName, const std::string& inName)
{
    std::string inputName = inName;
    if (inputName.empty())
    {
        inputName = outputName;
    }

    Pin* pOut = nullptr;
    for (auto& pPin : m_outputs)
    {
        if (pPin->GetName() == outputName)
        {
            pOut = pPin;
            break;
        }
    }
    
    if (pOut == nullptr)
    {
        throw std::invalid_argument("Can't find output pin: " + outputName);
    }
   
    Pin* pIn = nullptr;
    if (inputName != str_AutoGen)
    {
        for (auto& pPin : pDest->GetInputs())
        {
            if (pPin->GetName() == inputName)
            {
                pIn = pPin;
                break;
            }
        }

        if (pIn == nullptr)
        {
            throw std::invalid_argument("Can't find target pin: " + inputName);
        }
    }
    else
    {
        if (pOut->GetType() == ParameterType::FlowData)
        {
            int size = (int)pDest->GetFlowInputs().size();
            pIn = pDest->AddInput(std::string("FlowIn_") + std::to_string(size), (IFlowData*)nullptr);
        }
        else if (pOut->GetType() == ParameterType::ControlData)
        {
            int size = (int)pDest->GetControlInputs().size();
            pIn = pDest->AddInput(std::string("ControlIn_") + std::to_string(size), (IControlData*)nullptr);
        }
        else
        {
            throw std::invalid_argument("Can only generate inputs of flow data type");
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

} // namespace NodeGraph
