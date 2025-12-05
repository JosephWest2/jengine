#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace jengine::renderer::vulkan::pipelines {
class MeshPipeline {
  public:
    MeshPipeline(const vk::raii::Device& device,
                 const vk::Format& draw_image_format,
                 const vk::Format& depth_image_format);

    const vk::Pipeline& GetPipeline() const { return *pipeline; }

    const vk::PipelineLayout& GetPipelineLayout() const { return *pipeline_layout; }

  private:
    vk::raii::PipelineLayout pipeline_layout;
    vk::raii::Pipeline pipeline;
};
}  // namespace jengine::renderer::vulkan::pipelines
