#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "vulkan/vulkan.hpp"
namespace jengine::renderer::vulkan::pipelines {
class TrianglePipeline {
  public:
    TrianglePipeline(const vk::raii::Device& device, const vk::Format& draw_image_format);
    ~TrianglePipeline();

  private:
    vk::Pipeline pipeline;

    const vk::Device& device;
};
}  // namespace jengine::renderer::vulkan::pipelines
