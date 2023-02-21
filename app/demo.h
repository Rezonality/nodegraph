#pragma once

#include <glm/glm.hpp>

namespace NodeGraph 
{
struct IFontTexture;
}
void demo_resize(const glm::vec2& size, NodeGraph::IFontTexture* pFontTexture);
void demo_draw();
