#include "sky_pipeline.hpp"

#include "renderer/vulkan/pipelines/load_shader_module.hpp"

namespace jengine::renderer::vulkan::pipelines {
SkyPipeline::SkyPipeline(const vk::raii::Device& device, const vk::PipelineLayout& pipeline_layout)
    : pipeline(nullptr) {
    vk::raii::ShaderModule sky_shader_module = LoadShaderModule("shaders/sky.comp.spv", device);

    vk::PipelineShaderStageCreateInfo pipeline_shader_stage_create_info{
        .stage = vk::ShaderStageFlagBits::eCompute,
        .module = sky_shader_module,
        .pName = "main",
    };

    vk::ComputePipelineCreateInfo compute_pipeline_create_info{
        .stage = pipeline_shader_stage_create_info,
        .layout = pipeline_layout,
    };

    pipeline = device.createComputePipeline(nullptr, compute_pipeline_create_info);
}
}  // namespace jengine::renderer::vulkan::pipelines
