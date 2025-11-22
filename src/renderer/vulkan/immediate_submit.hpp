#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>
#include <vulkan/vulkan_raii.hpp>
#include "vulkan/vulkan.hpp"
namespace jengine::renderer::vulkan {
class ImmediateSubmit {
  public:
    ImmediateSubmit(const vk::raii::Device& device, uint32_t graphics_queue_family_index);
    ~ImmediateSubmit();

    void Submit(const vk::Device& device, const vk::Queue& queue, std::function<void(vk::CommandBuffer)>&& function);

  private:

    static constexpr uint64_t TIMEOUT_ONE_SECOND = 1000000000;

    vk::raii::CommandPool command_pool;
    vk::raii::Fence fence;
    vk::CommandBuffer command_buffer;
};
}  // namespace jengine::renderer::vulkan
