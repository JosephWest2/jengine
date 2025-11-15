#pragma once

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/deletion_stack.hpp"
namespace jengine::renderer::vulkan::pipelines {
class GradientPipeline {
  public:
    GradientPipeline(VkDescriptorSetLayout* draw_image_descriptor_layout_ptr,
                     VkDevice device,
                     DeletionStack& deletion_stack);
    void Destroy(VkDevice device);

    VkPipelineLayout GetPipelineLayout() const { return pipeline_layout; }
    VkPipeline GetPipeline() const { return pipeline; }

  private:
    VkPipelineLayout pipeline_layout{};
    VkPipeline pipeline{};
};
}  // namespace jengine::renderer::vulkan::pipelines
