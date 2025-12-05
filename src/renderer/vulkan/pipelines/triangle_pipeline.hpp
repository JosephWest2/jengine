#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "vulkan/vulkan.hpp"
namespace jengine::renderer::vulkan::pipelines {
class TrianglePipeline {
  public:
    TrianglePipeline(const vk::raii::Device& device, const vk::Format& draw_image_format,
                     const vk::Format& depth_image_format);

    const vk::Pipeline& GetPipeline() const { return *pipeline; }

  private:
    vk::raii::Pipeline pipeline;
};
}  // namespace jengine::renderer::vulkan::pipelines
