#include <map>
#include <memory>
#include <nodegraph/fonts.h>
#include <vulkan/vulkan.h>

namespace NodeGraph {

class VulkanImGuiTexture : public IFontTexture
{
public:
    VulkanImGuiTexture(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkDescriptorPool pool);

    // Texture handling bits using the imgui API
    virtual int UpdateTexture(int image, int x, int y, int w, int h, const unsigned char* data) override;
    virtual int CreateTexture(int w, int h, const unsigned char* data) override;
    virtual void DeleteTexture(int image) override;
    virtual void GetTextureSize(int image, int* w, int* h) override;
    virtual void* GetTexture(int image) override;

    std::vector<void*> GetTextures();

private:
    struct FontInfo
    {
        VkImage image = nullptr;
        VkImageView imageView = nullptr;
        VkDeviceMemory memory = nullptr;
        VkDescriptorSet descriptorSet = nullptr;
        VkSampler sampler = nullptr;
        VkBuffer uploadBuffer = nullptr;
        VkDeviceMemory uploadMemory = nullptr;
        VkDeviceSize memoryAlignment = 4;
        VkCommandPool commandPool = nullptr;
        VkCommandBuffer commandBuffer = nullptr;
        int width = 0;
        int height = 0;
        int textureId = 0;
        bool init = false;
    };

    int m_currentTextureId = 1;
    std::map<int, std::shared_ptr<FontInfo>> m_mapFonts;

    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueue m_queue;
    VkDescriptorPool m_pool;
};

} // Nodegraph
