#pragma once
#include <nodegraph/widgets/widget.h>

namespace NodeGraph {

class Canvas;
class Node : public Widget 
{
public:
    virtual void Draw(Canvas& canvas) override;
};

}
