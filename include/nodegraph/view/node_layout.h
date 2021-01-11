#pragma once

#include <nodegraph/view/layout.h>

namespace NodeGraph
{

class GraphView;
class ViewNode;

struct NodeLayout
{
    std::shared_ptr<MUtils::VLayout> spRoot;
    std::shared_ptr<MUtils::HLayout> spTitle;
    
    std::shared_ptr<MUtils::VLayout> spContents;
};

std::shared_ptr<NodeLayout> node_layout_create();
 

}
