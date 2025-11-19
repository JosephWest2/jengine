#pragma once

#include <vulkan/vulkan_core.h>

#include <glm/glm.hpp>
#include <string>

namespace jengine::renderer::vulkan::pipelines {

class GradientAndSkyPipeline {
  public:
    struct ComputePushConstants {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };

    enum class PipelineType { GRADIENT, SKY };
    GradientAndSkyPipeline(VkDescriptorSetLayout* draw_image_descriptor_layout_ptr, VkDevice& device);
    ~GradientAndSkyPipeline();

    VkPipelineLayout GetPipelineLayout() const { return pipeline_layout; }
    VkPipeline GetGradientPipeline() const { return gradient_pipeline; }

    ComputePushConstants& SkyPushConstants() {
        return sky_push_constants;
    }
    ComputePushConstants& GradientPushConstants() {
        return gradient_push_constants;
    }
    PipelineType& SelectedPipeline() { return selected_pipeline; }
    std::string SelectedPipelineString() {
        if (selected_pipeline == PipelineType::GRADIENT) {
            return "Gradient";
        } else {
            return "Sky";
        }
    }

    

  private:
    ComputePushConstants gradient_push_constants{};
    ComputePushConstants sky_push_constants{};
    VkPipelineLayout pipeline_layout{};
    VkPipeline gradient_pipeline{};
    VkPipeline sky_pipeline{};
    PipelineType selected_pipeline = PipelineType::GRADIENT;

    // held for destruction
    VkDevice& device;
};
}  // namespace jengine::renderer::vulkan::pipelines
