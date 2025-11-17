#pragma once

#include "renderer/vulkan/pipelines/gradient_pipeline.hpp"
namespace jengine::renderer::vulkan::pipelines {

class Manager {
  public:
    Manager(VkDevice& device, VkDescriptorSetLayout* draw_image_descriptor_layout_ptr);

    GradientPipeline& GetGradientPipeline() { return gradient_pipeline; }

  private:
    GradientPipeline gradient_pipeline;
};
}  // namespace jengine::renderer::vulkan::pipelines
