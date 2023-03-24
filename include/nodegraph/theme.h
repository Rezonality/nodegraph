#pragma once

#include <nodegraph/setting.h>

namespace NodeGraph
{

class ThemeManager : public SettingManager
{
public:
    static ThemeManager& Instance()
    {
        static ThemeManager theme;
        return theme;
    }
};

// Grid
DECLARE_SETTING_VALUE(s_gridLineSize);
DECLARE_SETTING_VALUE(c_gridLines);

// Node
DECLARE_SETTING_VALUE(s_nodeBorderRadius);
DECLARE_SETTING_VALUE(s_nodeShadowSize);
DECLARE_SETTING_VALUE(s_nodeBorderSize);
DECLARE_SETTING_VALUE(c_nodeShadowColor);
DECLARE_SETTING_VALUE(c_nodeCenterColor);
DECLARE_SETTING_VALUE(c_nodeBorderColor);

// - Title
DECLARE_SETTING_VALUE(s_nodeTitleSize);
DECLARE_SETTING_VALUE(s_nodeTitleFontPad);
DECLARE_SETTING_VALUE(s_nodeTitleBorder);
DECLARE_SETTING_VALUE(s_nodeTitleBorderRadius);
DECLARE_SETTING_VALUE(s_nodeTitlePad);
DECLARE_SETTING_VALUE(s_nodeTitleShadowSize);
DECLARE_SETTING_VALUE(s_nodeTitleBorderSize);
 
DECLARE_SETTING_VALUE(c_nodeTitleShadowColor);
DECLARE_SETTING_VALUE(c_nodeTitleCenterColor);
DECLARE_SETTING_VALUE(c_nodeTitleBorderColor);

// Slider
DECLARE_SETTING_VALUE(s_sliderBorderSize);

DECLARE_SETTING_VALUE(s_sliderThumbPad);
DECLARE_SETTING_VALUE(s_sliderThumbShadowSize);
DECLARE_SETTING_VALUE(s_sliderThumbRadius);
DECLARE_SETTING_VALUE(c_sliderThumbShadowColor);
DECLARE_SETTING_VALUE(c_sliderThumbColor);

DECLARE_SETTING_VALUE(s_sliderBorderRadius);
DECLARE_SETTING_VALUE(s_sliderShadowSize);
DECLARE_SETTING_VALUE(s_sliderFontPad);
DECLARE_SETTING_VALUE(c_sliderBorderColor);
DECLARE_SETTING_VALUE(c_sliderCenterColor);
DECLARE_SETTING_VALUE(c_sliderShadowColor);

DECLARE_SETTING_VALUE(s_sliderTipBorderRadius);
DECLARE_SETTING_VALUE(s_sliderTipShadowSize);
DECLARE_SETTING_VALUE(s_sliderTipFontPad);
DECLARE_SETTING_VALUE(s_sliderTipFontSize);
DECLARE_SETTING_VALUE(s_sliderTipBorderSize);
DECLARE_SETTING_VALUE(c_sliderTipBorderColor);
DECLARE_SETTING_VALUE(c_sliderTipCenterColor);
DECLARE_SETTING_VALUE(c_sliderTipShadowColor);
DECLARE_SETTING_VALUE(c_sliderTipFontColor);

DECLARE_SETTING_VALUE(b_debugShowLayout);

} // namespace Nodegraph

