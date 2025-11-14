#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>
#include <stack>
namespace jengine::renderer::vulkan {
class ImmediateSubmit {
  public:
    ImmediateSubmit(VkDevice device, uint32_t graphics_queue_family_index, std::stack<std::function<void()>>& deletion_stack);

    void Destroy(VkDevice device);

    void Submit(VkDevice device, VkQueue queue, std::function<void(VkCommandBuffer)> function);

  private:
    VkFence fence;
    VkCommandBuffer command_buffer;
    VkCommandPool command_pool;
};
}  // namespace jengine::renderer::vulkan
