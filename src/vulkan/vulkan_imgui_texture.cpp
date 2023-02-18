#include <nodegraph/fonts.h>
#include <nodegraph/vulkan/vulkan_imgui_texture.h>
#include <imgui_impl_vulkan.h>
namespace NodeGraph 
{

struct VulkanTextureState state;

/* static ImGui_ImplVulkan_Data* ImGui_ImplVulkan_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData : NULL;
}
*/

int vulkan_imgui_update_texture(void* uptr, int image, int x, int y, int w, int h, const unsigned char* data)
{
    return 0;
}

int vulkan_imgui_create_texture(void* uptr, int w, int h, const unsigned char* data)
{
    /*
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();
    ImGui_ImplVulkan_InitInfo* v = &bd->VulkanInitInfo;

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    size_t upload_size = width * height * 4 * sizeof(char);

    VkResult err;

    // Create the Image:
    {
        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.format = VK_FORMAT_R8G8B8A8_UNORM;
        info.extent.width = width;
        info.extent.height = height;
        info.extent.depth = 1;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        err = vkCreateImage(v->Device, &info, v->Allocator, &bd->FontImage);
        check_vk_result(err);
        VkMemoryRequirements req;
        vkGetImageMemoryRequirements(v->Device, bd->FontImage, &req);
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = req.size;
        alloc_info.memoryTypeIndex = ImGui_ImplVulkan_MemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
        err = vkAllocateMemory(v->Device, &alloc_info, v->Allocator, &bd->FontMemory);
        check_vk_result(err);
        err = vkBindImageMemory(v->Device, bd->FontImage, bd->FontMemory, 0);
        check_vk_result(err);
    }

    // Create the Image View:
    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = bd->FontImage;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = VK_FORMAT_R8G8B8A8_UNORM;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.layerCount = 1;
        err = vkCreateImageView(v->Device, &info, v->Allocator, &bd->FontView);
        check_vk_result(err);
    }

    // Create the Descriptor Set:
    bd->FontDescriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(bd->FontSampler, bd->FontView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Create the Upload Buffer:
    {
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = upload_size;
        buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        err = vkCreateBuffer(v->Device, &buffer_info, v->Allocator, &bd->UploadBuffer);
        check_vk_result(err);
        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(v->Device, bd->UploadBuffer, &req);
        bd->BufferMemoryAlignment = (bd->BufferMemoryAlignment > req.alignment) ? bd->BufferMemoryAlignment : req.alignment;
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = req.size;
        alloc_info.memoryTypeIndex = ImGui_ImplVulkan_MemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
        err = vkAllocateMemory(v->Device, &alloc_info, v->Allocator, &bd->UploadBufferMemory);
        check_vk_result(err);
        err = vkBindBufferMemory(v->Device, bd->UploadBuffer, bd->UploadBufferMemory, 0);
        check_vk_result(err);
    }

    // Upload to Buffer:
    {
        char* map = NULL;
        err = vkMapMemory(v->Device, bd->UploadBufferMemory, 0, upload_size, 0, (void**)(&map));
        check_vk_result(err);
        memcpy(map, pixels, upload_size);
        VkMappedMemoryRange range[1] = {};
        range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range[0].memory = bd->UploadBufferMemory;
        range[0].size = upload_size;
        err = vkFlushMappedMemoryRanges(v->Device, 1, range);
        check_vk_result(err);
        vkUnmapMemory(v->Device, bd->UploadBufferMemory);
    }

    // Copy to Image:
    {
        VkImageMemoryBarrier copy_barrier[1] = {};
        copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier[0].image = bd->FontImage;
        copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_barrier[0].subresourceRange.levelCount = 1;
        copy_barrier[0].subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);

        VkBufferImageCopy region = {};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width = width;
        region.imageExtent.height = height;
        region.imageExtent.depth = 1;
        vkCmdCopyBufferToImage(command_buffer, bd->UploadBuffer, bd->FontImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier use_barrier[1] = {};
        use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier[0].image = bd->FontImage;
        use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        use_barrier[0].subresourceRange.levelCount = 1;
        use_barrier[0].subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
    }

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)bd->FontDescriptorSet);
    */

    return 0;
}

void vulkan_imgui_get_texture_size(FontContext& ctx, int image, int* w, int* h)
{
}

} // Nodegraph
