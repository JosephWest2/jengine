#include "gradient_pipeline.hpp"

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/pipelines/load_shader_module.hpp"

namespace jengine::renderer::vulkan::pipelines {

GradientPipeline::GradientPipeline(vk::DescriptorSetLayout* draw_image_descriptor_layout_ptr,
                                               const vk::raii::Device& device)
    : device(*device) {
    vk::PushConstantRange push_constant_range{
        .stageFlags = vk::ShaderStageFlagBits::eCompute,
        .offset = 0,
        .size = sizeof(ComputePushConstants),
    };
    vk::PipelineLayoutCreateInfo compute_pipeline_layout_create_info{.setLayoutCount = 1,
                                                                     .pSetLayouts = draw_image_descriptor_layout_ptr,
                                                                     .pushConstantRangeCount = 1,
                                                                     .pPushConstantRanges = &push_constant_range};

    pipeline_layout = device.createPipelineLayout(compute_pipeline_layout_create_info);
    vk::raii::ShaderModule sky_shader_module = LoadShaderModule("shaders/sky.comp.spv", device);

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

    gradient_pipeline = device.createComputePipeline(nullptr, compute_pipeline_create_info);
}
GradientPipeline::~GradientPipeline() {

    device.destroyPipeline(gradient_pipeline);
    device.destroyPipelineLayout(pipeline_layout);
}
}  // namespace jengine::renderer::vulkan::pipelines
