#pragma once

#include <vulkan/vulkan_core.h>

#include "VkBootstrap.h"

namespace jengine::renderer::vulkan {
class GraphicsQueue {
  public:
    GraphicsQueue(vkb::Device& device);

    VkQueue GetQueue() const { return queue; }
    uint32_t GetQueueFamilyIndex() const { return queue_family_index; }

  private:
    VkQueue queue{};
    uint32_t queue_family_index{};
};
}  // namespace jengine::renderer::vulkan
