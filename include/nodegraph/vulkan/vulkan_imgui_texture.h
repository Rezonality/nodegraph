#include <nodegraph/fonts.h>

namespace NodeGraph {

struct VulkanTextureState
{

};

// Texture handling bits using the imgui API
int vulkan_imgui_update_texture(void* uptr, int image, int x, int y, int w, int h, const unsigned char* data);
int vulkan_imgui_create_texture(void* uptr, int w, int h, const unsigned char* data);
void vulkan_imgui_get_texture_size(FontContext& ctx, int image, int* w, int* h);

} // Nodegraph
