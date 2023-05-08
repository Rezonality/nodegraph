#pragma once

#include <glm/glm.hpp>

namespace NodeGraph 
{
struct IFontTexture;
class Canvas;

}
void demo_resize(const glm::vec2& size, NodeGraph::IFontTexture* pFontTexture);
void demo_draw();
void demo_cleanup();
NodeGraph::Canvas* demo_get_canvas();
