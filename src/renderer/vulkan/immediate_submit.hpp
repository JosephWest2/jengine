#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>
namespace jengine::renderer::vulkan {
class ImmediateSubmit {
  public:
    ImmediateSubmit(VkDevice& device, uint32_t graphics_queue_family_index);
    ~ImmediateSubmit();

    void Submit(VkQueue queue, std::function<void(VkCommandBuffer)> function);

  private:
    VkFence fence;
    VkCommandBuffer command_buffer;
    VkCommandPool command_pool;

    // held for destruction
    VkDevice& device;
};
}  // namespace jengine::renderer::vulkan
