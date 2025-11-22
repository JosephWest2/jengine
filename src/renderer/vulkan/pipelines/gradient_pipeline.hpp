#pragma once

#include <vulkan/vulkan_core.h>

#include <glm/glm.hpp>
#include <string>
#include <vulkan/vulkan_raii.hpp>
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::pipelines {

class GradientPipeline {
  public:
    struct ComputePushConstants {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };

    enum class PipelineType { GRADIENT, SKY };
    GradientPipeline(vk::DescriptorSetLayout* draw_image_descriptor_layout_ptr, const vk::raii::Device& device);
    ~GradientPipeline();

    vk::PipelineLayout GetPipelineLayout() const { return pipeline_layout; }
    vk::Pipeline GetGradientPipeline() const { return gradient_pipeline; }

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
    vk::PipelineLayout pipeline_layout{};
    vk::raii::Pipeline gradient_pipeline;
    vk::Pipeline sky_pipeline{};
    PipelineType selected_pipeline = PipelineType::GRADIENT;

    // held for destruction
    const vk::Device& device;
};
}  // namespace jengine::renderer::vulkan::pipelines
