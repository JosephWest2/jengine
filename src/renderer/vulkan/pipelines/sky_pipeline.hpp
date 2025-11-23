#pragma once

#include <vulkan/vulkan_raii.hpp>
namespace jengine::renderer::vulkan::pipelines {
class SkyPipeline {
  public:
    SkyPipeline(const vk::raii::Device& device, const vk::PipelineLayout& pipeline_layout);

    const vk::Pipeline& GetPipeline() const { return *pipeline; }

  private:
    vk::raii::Pipeline pipeline;
};
}  // namespace jengine::renderer::vulkan::pipelines
