#pragma once

#include "renderer/vulkan/pipelines/gradient_pipeline.hpp"
namespace jengine::renderer::vulkan::pipelines {

class Manager {
  public:
    Manager(VkDevice& device, VkFormat draw_image_format, VkDescriptorSetLayout* draw_image_descriptor_layout_ptr);
    ~Manager();

    GradientAndSkyPipeline& GetGradientAndSkyPipeline() { return gradient_and_sky_pipeline; }
    VkPipeline GetTrianglePipeline() const { return triangle_pipeline; }
    VkPipelineLayout GetTrianglePipelineLayout() const { return triangle_pipeline_layout; }

  private:
    void InitTrianglePipeline(VkDevice& device, VkFormat draw_image_format);
    GradientAndSkyPipeline gradient_and_sky_pipeline;
    VkPipelineLayout triangle_pipeline_layout{};
    VkPipeline triangle_pipeline{};

    // held for destruction
    VkDevice& device;
};
}  // namespace jengine::renderer::vulkan::pipelines
