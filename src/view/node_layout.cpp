#include <mutils/ui/colors.h>
#include <nodegraph/view/node_layout.h>
#include <nodegraph/view/graphview.h>
#include <nodegraph/view/canvas.h>

using namespace MUtils;
namespace NodeGraph
{

float node_margin = 4.0f;
float node_titleHeight = 30.0f;
float node_titlePad = 2.0f;
float node_contentsPad = 2.0f;

std::shared_ptr<NodeLayout> node_layout_create()
{
    auto spNodeLayout = std::make_shared<NodeLayout>();

    spNodeLayout->spRoot = std::make_shared<VLayout>();
    spNodeLayout->spTitle = std::make_shared<HLayout>();
    spNodeLayout->spContents = std::make_shared<VLayout>();

    spNodeLayout->spRoot->AddItem(spNodeLayout->spTitle.get(), NVec2f(0.0f, node_titleHeight));
    spNodeLayout->spRoot->AddItem(spNodeLayout->spContents.get(), NVec2f(0.0f, 0.0f));

    spNodeLayout->spRoot->SetPadding(NVec4f(node_margin));
    spNodeLayout->spTitle->SetPadding(NVec4f(node_titlePad));
    spNodeLayout->spContents->SetPadding(NVec4f(node_contentsPad));

    return spNodeLayout;
}


} // namespace NodeGraph