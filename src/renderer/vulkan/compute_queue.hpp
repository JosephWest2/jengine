#pragma once

#include <functional>
#include <stack>
#include "VkBootstrap.h"
namespace jengine::renderer::vulkan {

class ComputeQueue {
  public:
    ComputeQueue(vkb::Device& device);
    VkQueue GetQueue() const { return queue; }
    uint32_t GetQueueFamilyIndex() const { return queue_family_index; }

  private:
    VkQueue queue{};
    uint32_t queue_family_index{};
};

}  // namespace jengine::renderer::vulkan
