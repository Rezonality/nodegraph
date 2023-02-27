#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <unordered_map>
#include <nodegraph/string/string_utils.h>

namespace NodeGraph
{

enum class ThemeType
{
    Unknown,
    Float,
    Vec2f,
    Vec3f,
    Vec4f
};

struct ThemeValue
{
    ThemeValue()
        : type(ThemeType::Unknown)
        , f4(glm::vec4(0.0f))
    {
    }
    ThemeValue(const glm::vec4& val)
        : type(ThemeType::Vec4f)
        , f4(val)
    {
    }
    ThemeValue(const glm::vec3& val)
        : type(ThemeType::Vec3f)
        , f3(val)
    {
    }
    ThemeValue(const glm::vec2& val)
        : type(ThemeType::Vec2f)
        , f2(val)
    {
    }
    ThemeValue(const float& val)
        : type(ThemeType::Float)
        , f(val)
    {
    }

    glm::vec4 ToVec4f() const
    {
        if (type == ThemeType::Unknown)
        {
            type = ThemeType::Vec4f;
        }

        if (type == ThemeType::Vec4f)
        {
            return f4;
        }
        return glm::vec4(f);
    }
    
    glm::vec2 ToVec2f() const
    {
        if (type == ThemeType::Unknown)
        {
            type = ThemeType::Vec2f;
        }

        switch (type)
        {
        case ThemeType::Vec2f:
            return f2;
            break;
        case ThemeType::Vec3f:
            return glm::vec2(f3);
            break;
        case ThemeType::Vec4f:
            return glm::vec2(f4);
            break;
        case ThemeType::Float:
            return glm::vec2(f);
            break;
        }
        return glm::vec2(0.0f);
    }

    glm::vec3 ToVec3f() const
    {
        if (type == ThemeType::Unknown)
        {
            type = ThemeType::Vec3f;
        }

        switch (type)
        {
        case ThemeType::Vec2f:
            return glm::vec3(f2.x, f2.y, 0.0f);
            break;
        case ThemeType::Vec3f:
            return f3;
            break;
        case ThemeType::Vec4f:
            return glm::vec3(f4);
            break;
        case ThemeType::Float:
            return glm::vec3(f);
            break;
        }
        return glm::vec3(0.0f);
    }

    float ToFloat() const
    {
        if (type == ThemeType::Unknown)
        {
            type = ThemeType::Float;
        }

        if (type == ThemeType::Float)
        {
            return f;
        }
        return f4.x;
    }

    union
    {
        glm::vec4 f4 = glm::vec4(1.0f);
        glm::vec3 f3;
        glm::vec2 f2;
        float f;
    };
    mutable ThemeType type;
};

using ThemeMap = std::unordered_map<StringId, ThemeValue>;
class ThemeManager
{
public:
    static ThemeManager& Instance()
    {
        static ThemeManager theme;
        return theme;
    }

    bool Save(const std::filesystem::path& path);
    bool Load(const std::filesystem::path& path);
    void Set(const StringId& id, const ThemeValue& value)
    {
        auto& slot = m_themes[m_currentTheme][id];
        slot = value;
    }

    const ThemeValue& Get(const StringId& id)
    {
        auto& theme = m_themes[m_currentTheme];
        return theme[id];
    }

    float GetFloat(const StringId& id)
    {
        auto& theme = m_themes[m_currentTheme];
        return theme[id].ToFloat();
    }

    glm::vec2 GetVec2f(const StringId& id)
    {
        auto& theme = m_themes[m_currentTheme];
        return theme[id].ToVec2f();
    }

    glm::vec4 GetVec4f(const StringId& id)
    {
        auto& theme = m_themes[m_currentTheme];
        return theme[id].ToVec4f();
    }

    std::unordered_map<std::string, ThemeMap> m_themes;
    std::string m_currentTheme = "Default Theme";
};

} // namespace MUtils

#ifdef DECLARE_THEMES
#define DECLARE_THEME_VALUE(name) NodeGraph::StringId name(#name);
#else
#define DECLARE_THEME_VALUE(name) extern NodeGraph::StringId name;
#endif

// Grid
DECLARE_THEME_VALUE(s_gridLineSize);
DECLARE_THEME_VALUE(c_gridLines);

// Node
DECLARE_THEME_VALUE(s_nodeBorderRadius);
DECLARE_THEME_VALUE(s_nodeShadowSize);
DECLARE_THEME_VALUE(c_nodeShadow);
DECLARE_THEME_VALUE(c_nodeBackground);

// Title
DECLARE_THEME_VALUE(s_nodeTitleFontSize);
DECLARE_THEME_VALUE(s_nodeTitleFontPad);
DECLARE_THEME_VALUE(s_nodeTitleBorder);
DECLARE_THEME_VALUE(s_nodeTitleBorderRadius);

DECLARE_THEME_VALUE(c_nodeTitleBackground);
