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
    auto spNodeLayout = std::make_shared<NodeLayout>();

    spNodeLayout->spRoot = std::make_shared<VLayout>();

    spNodeLayout->spTitle = std::make_shared<HLayout>();
    spNodeLayout->spContents = std::make_shared<VLayout>();

    auto& style = StyleManager::Instance();

    spNodeLayout->spRoot->AddItem(spNodeLayout->spTitle.get(), NVec2f(0.0f, style.GetFloat(style_nodeTitleHeight)));
    spNodeLayout->spRoot->AddItem(spNodeLayout->spContents.get(), NVec2f(0.0f, 0.0f));

    spNodeLayout->spRoot->SetPadding(style.GetVec4f(style_nodeOuter));
    spNodeLayout->spTitle->SetPadding(style.GetVec4f(style_nodeTitlePad));
    spNodeLayout->spContents->SetPadding(style.GetVec4f(style_nodeContentsPad));

    return spNodeLayout;
}

} // namespace NodeGraph