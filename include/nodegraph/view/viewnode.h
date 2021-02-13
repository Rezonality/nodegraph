#pragma once

#include "nodegraph/model/node.h"
#include "nodegraph/model/pin.h"

namespace NodeGraph
{

class ViewNode
{
public:
    explicit ViewNode(Node* pModel);
    Node* pModelNode = nullptr;
    bool active = false;
    bool hovered = false;
};

} // namespace NodeGraph
