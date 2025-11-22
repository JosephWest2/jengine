#pragma once

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>

namespace jengine::renderer::vulkan {
class Queue {
  public:
    Queue(uint32_t queue_family_index, const vk::Device& device);

    vk::Queue& GetQueue() { return queue; }
    uint32_t GetQueueFamilyIndex() const { return queue_family_index; }

  private:
    vk::Queue queue{};
    uint32_t queue_family_index{};
};
}  // namespace jengine::renderer::vulkan
