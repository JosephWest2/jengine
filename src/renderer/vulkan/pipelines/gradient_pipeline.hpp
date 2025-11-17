#pragma once

#include <vulkan/vulkan_core.h>

#include <glm/glm.hpp>

namespace jengine::renderer::vulkan::pipelines {

class GradientPipeline {
  public:
    struct ComputePushConstants {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };
    GradientPipeline(VkDescriptorSetLayout* draw_image_descriptor_layout_ptr, VkDevice& device);
    ~GradientPipeline();

    VkPipelineLayout GetPipelineLayout() const { return pipeline_layout; }
    VkPipeline GetPipeline() const { return pipeline; }

  private:
    VkPipelineLayout pipeline_layout{};
    VkPipeline pipeline{};

    // held for destruction
    VkDevice& device;
};
}  // namespace jengine::renderer::vulkan::pipelines
