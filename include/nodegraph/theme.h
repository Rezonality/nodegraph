#pragma once

#include <zest/settings/settings.h>

namespace NodeGraph
{

class ThemeManager : public Zest::SettingManager
{
public:
    static ThemeManager& Instance()
    {
        static ThemeManager theme;
        return theme;
    }
};

#ifdef DECLARE_THEME_SETTINGS
#define DECLARE_THEME_SETTING_VALUE(name) Zest::StringId name(#name);
#else
#define DECLARE_THEME_SETTING_VALUE(name) extern Zest::StringId name;
#endif

// Grid
DECLARE_THEME_SETTING_VALUE(s_gridLineSize);
DECLARE_THEME_SETTING_VALUE(c_gridLines);

// Node
DECLARE_THEME_SETTING_VALUE(s_nodeBorderRadius);
DECLARE_THEME_SETTING_VALUE(s_nodeShadowSize);
DECLARE_THEME_SETTING_VALUE(s_nodeBorderSize);
DECLARE_THEME_SETTING_VALUE(c_nodeShadowColor);
DECLARE_THEME_SETTING_VALUE(c_nodeCenterColor);
DECLARE_THEME_SETTING_VALUE(c_nodeBorderColor);

// - Title
DECLARE_THEME_SETTING_VALUE(s_nodeTitleSize);
DECLARE_THEME_SETTING_VALUE(s_nodeTitleFontPad);
DECLARE_THEME_SETTING_VALUE(s_nodeTitleBorder);
DECLARE_THEME_SETTING_VALUE(s_nodeTitleBorderRadius);
DECLARE_THEME_SETTING_VALUE(s_nodeTitlePad);
DECLARE_THEME_SETTING_VALUE(s_nodeTitleShadowSize);
DECLARE_THEME_SETTING_VALUE(s_nodeTitleBorderSize);
 
DECLARE_THEME_SETTING_VALUE(c_nodeTitleShadowColor);
DECLARE_THEME_SETTING_VALUE(c_nodeTitleCenterColor);
DECLARE_THEME_SETTING_VALUE(c_nodeTitleBorderColor);

// Slider
DECLARE_THEME_SETTING_VALUE(s_sliderBorderSize);

DECLARE_THEME_SETTING_VALUE(s_sliderThumbPad);
DECLARE_THEME_SETTING_VALUE(s_sliderThumbShadowSize);
DECLARE_THEME_SETTING_VALUE(s_sliderThumbRadius);
DECLARE_THEME_SETTING_VALUE(c_sliderThumbShadowColor);
DECLARE_THEME_SETTING_VALUE(c_sliderThumbColor);

DECLARE_THEME_SETTING_VALUE(s_sliderBorderRadius);
DECLARE_THEME_SETTING_VALUE(s_sliderShadowSize);
DECLARE_THEME_SETTING_VALUE(s_sliderFontPad);
DECLARE_THEME_SETTING_VALUE(c_sliderBorderColor);
DECLARE_THEME_SETTING_VALUE(c_sliderCenterColor);
DECLARE_THEME_SETTING_VALUE(c_sliderShadowColor);

DECLARE_THEME_SETTING_VALUE(s_sliderTipBorderRadius);
DECLARE_THEME_SETTING_VALUE(s_sliderTipShadowSize);
DECLARE_THEME_SETTING_VALUE(s_sliderTipFontPad);
DECLARE_THEME_SETTING_VALUE(s_sliderTipFontSize);
DECLARE_THEME_SETTING_VALUE(s_sliderTipBorderSize);
DECLARE_THEME_SETTING_VALUE(c_sliderTipBorderColor);
DECLARE_THEME_SETTING_VALUE(c_sliderTipCenterColor);
DECLARE_THEME_SETTING_VALUE(c_sliderTipShadowColor);
DECLARE_THEME_SETTING_VALUE(c_sliderTipFontColor);

DECLARE_THEME_SETTING_VALUE(c_labelColor);
DECLARE_THEME_SETTING_VALUE(c_knobChannelColor);
DECLARE_THEME_SETTING_VALUE(c_knobShadowColor);
DECLARE_THEME_SETTING_VALUE(c_knobChannelHLColor);
DECLARE_THEME_SETTING_VALUE(c_knobMarkColor);
DECLARE_THEME_SETTING_VALUE(c_knobMarkHLColor);
DECLARE_THEME_SETTING_VALUE(c_knobFillColor);
DECLARE_THEME_SETTING_VALUE(c_knobFillHLColor);
DECLARE_THEME_SETTING_VALUE(c_knobTextColor);

DECLARE_THEME_SETTING_VALUE(s_knobChannelWidth);
DECLARE_THEME_SETTING_VALUE(s_knobChannelGap);
DECLARE_THEME_SETTING_VALUE(s_knobShadowSize);
DECLARE_THEME_SETTING_VALUE(s_knobTextSize);
DECLARE_THEME_SETTING_VALUE(s_knobTextInset);

DECLARE_THEME_SETTING_VALUE(b_debugShowLayout);

DECLARE_THEME_SETTING_VALUE(c_socketColor);
DECLARE_THEME_SETTING_VALUE(c_socketShadowColor);

} // namespace Nodegraph

