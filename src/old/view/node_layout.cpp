#include <mutils/ui/colors.h>

#include <nodegraph/view/canvas.h>
#include <nodegraph/view/graphview.h>
#include <nodegraph/view/node_layout.h>
#include <nodegraph/view/style.h>

using namespace MUtils;
using namespace MUtils::Style;

namespace NodeGraph
{

std::shared_ptr<NodeLayout> node_layout_create()
{
    auto& style = StyleManager::Instance();

    auto spNodeLayout = std::make_shared<NodeLayout>();

    // Layout margin is the border around the layout contents (not the spacing of inner items)
    spNodeLayout->spRoot = std::make_shared<VLayout>(style.GetVec4f(style_nodeLayoutMargin));
    spNodeLayout->spTitle = std::make_shared<HLayout>(style.GetVec4f(style_nodeLayoutMargin));
    spNodeLayout->spFooter = std::make_shared<HLayout>(style.GetVec4f(style_nodeLayoutMargin));
    spNodeLayout->spContents = std::make_shared<VLayout>(style.GetVec4f(style_nodeLayoutMargin));

    spNodeLayout->spRoot->AddItem(spNodeLayout->spTitle.get(), NVec2f(0.0f, style.GetFloat(style_nodeTitleHeight)));
    spNodeLayout->spRoot->AddItem(spNodeLayout->spContents.get(), NVec2f(0.0f, 0.0f));
    spNodeLayout->spRoot->AddItem(spNodeLayout->spFooter.get(), NVec2f(0.0f, 0.0f));//style.GetFloat(style_nodeTitleHeight)));

    // TODO: Calculate a minimum title size for nodes with small UI?
    // Minimum size for now
    spNodeLayout->spRoot->SetMinSize(NVec2f(100.0f, 0.0f));

    return spNodeLayout;
}

} // namespace NodeGraph