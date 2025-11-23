#include "gradient_pipeline.hpp"

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/pipelines/load_shader_module.hpp"

namespace jengine::renderer::vulkan::pipelines {

GradientPipeline::GradientPipeline(const vk::raii::Device& device, const vk::PipelineLayout& pipeline_layout)
    : pipeline(nullptr) {
    vk::raii::ShaderModule gradient_shader_module = LoadShaderModule("shaders/gradient.comp.spv", device);

    vk::PipelineShaderStageCreateInfo gradient_pipeline_shader_stage_create_info{
        .stage = vk::ShaderStageFlagBits::eCompute,
        .module = gradient_shader_module,
        .pName = "main",
    };

    vk::ComputePipelineCreateInfo compute_pipeline_create_info{
        .stage = gradient_pipeline_shader_stage_create_info,
        .layout = pipeline_layout,
    };

    pipeline = device.createComputePipeline(nullptr, compute_pipeline_create_info);
}
}  // namespace jengine::renderer::vulkan::pipelines
