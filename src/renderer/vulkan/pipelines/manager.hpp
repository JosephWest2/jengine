#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "renderer/vulkan/pipelines/gradient_pipeline.hpp"
namespace jengine::renderer::vulkan::pipelines {

class Manager {
  public:
    Manager(const vk::Device& device, vk::Format draw_image_format, vk::DescriptorSetLayout* draw_image_descriptor_layout_ptr);
    ~Manager();

    GradientPipeline& GetGradientAndSkyPipeline() { return gradient_and_sky_pipeline; }
    vk::Pipeline GetTrianglePipeline() const { return triangle_pipeline; }
    vk::PipelineLayout GetTrianglePipelineLayout() const { return triangle_pipeline_layout; }


  private:
    GradientPipeline gradient_and_sky_pipeline;
    vk::PipelineLayout triangle_pipeline_layout{};
    vk::Pipeline triangle_pipeline{};

    // held for destruction
    const vk::Device& device;
};
}  // namespace jengine::renderer::vulkan::pipelines
