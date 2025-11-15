#include "gradient_pipeline.hpp"

#include "renderer/vulkan/pipelines/load_shader_module.hpp"

namespace jengine::renderer::vulkan::pipelines {

GradientPipeline::GradientPipeline(VkDescriptorSetLayout* draw_image_descriptor_layout_ptr,
                                   VkDevice device,
                                   DeletionStack& deletion_stack) {
    VkPipelineLayoutCreateInfo compute_pipeline_layout_create_info{};
    compute_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    compute_pipeline_layout_create_info.pNext = nullptr;
    compute_pipeline_layout_create_info.pSetLayouts = draw_image_descriptor_layout_ptr;
    compute_pipeline_layout_create_info.setLayoutCount = 1;

    if (vkCreatePipelineLayout(device, &compute_pipeline_layout_create_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    VkShaderModule compute_shader_module = LoadShaderModule("shaders/gradient.comp.spv", device);
    if (!compute_shader_module) {
        throw std::runtime_error("Failed to create compute shader module");
    }

    VkPipelineShaderStageCreateInfo compute_pipeline_shader_stage_create_info{};
    compute_pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    compute_pipeline_shader_stage_create_info.pNext = nullptr;
    compute_pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    compute_pipeline_shader_stage_create_info.module = compute_shader_module;
    compute_pipeline_shader_stage_create_info.pName = "main";

    VkComputePipelineCreateInfo compute_pipeline_create_info{};
    compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_create_info.pNext = nullptr;
    compute_pipeline_create_info.layout = pipeline_layout;
    compute_pipeline_create_info.stage = compute_pipeline_shader_stage_create_info;

    if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &pipeline) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create compute pipeline");
    }

    vkDestroyShaderModule(device, compute_shader_module, nullptr);

    deletion_stack.Push([this, device]() { Destroy(device); });
}
void GradientPipeline::Destroy(VkDevice device) {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
}
}  // namespace jengine::renderer::vulkan::pipelines
