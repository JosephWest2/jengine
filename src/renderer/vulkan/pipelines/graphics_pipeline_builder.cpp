#include "graphics_pipeline_builder.hpp"

#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "renderer/vulkan/initializers.hpp"

namespace jengine::renderer::vulkan::pipelines {

void GraphicsPipelineBuilder::Reset() { *this = GraphicsPipelineBuilder(); }

GraphicsPipelineBuilder::GraphicsPipelineBuilder() {
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
}
VkPipeline GraphicsPipelineBuilder::Build(VkDevice device) {
    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pNext = nullptr;

    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineColorBlendStateCreateInfo color_blend_state{};
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.pNext = nullptr;

    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &color_blend_attachment_state;

    VkPipelineVertexInputStateCreateInfo vertex_input_state{};
    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state.pNext = nullptr;

    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.pNext = &rendering_info;

    pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
    pipeline_create_info.pStages = shader_stages.data();

    pipeline_create_info.pVertexInputState = &vertex_input_state;
    pipeline_create_info.pInputAssemblyState = &input_assembly_info;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pRasterizationState = &rasterization_info;
    pipeline_create_info.pMultisampleState = &multisample_info;
    pipeline_create_info.pColorBlendState = &color_blend_state;
    pipeline_create_info.pDepthStencilState = &depth_stencil_info;
    pipeline_create_info.layout = pipeline_layout;

    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.pNext = nullptr;

    dynamic_state_create_info.dynamicStateCount = 2;
    dynamic_state_create_info.pDynamicStates = dynamic_states;

    pipeline_create_info.pDynamicState = &dynamic_state_create_info;

    VkPipeline result;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &result) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
    return result;
}
void GraphicsPipelineBuilder::SetShaders(VkShaderModule vertex_shader, VkShaderModule fragment_shader) {
    shader_stages.clear();
    shader_stages.push_back(vulkan::init::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertex_shader));
    shader_stages.push_back(vulkan::init::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader));
}
void GraphicsPipelineBuilder::SetInputTopology(VkPrimitiveTopology topology) {
    input_assembly_info.topology = topology;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;
}
void GraphicsPipelineBuilder::SetPolygonMode(VkPolygonMode polygon_mode) {
    rasterization_info.polygonMode = polygon_mode;
    rasterization_info.lineWidth = 1.0f;
}
void GraphicsPipelineBuilder::SetCullMode(VkCullModeFlags cull_mode_flags, VkFrontFace front_face) {
    rasterization_info.cullMode = cull_mode_flags;
    rasterization_info.frontFace = front_face;
}
void GraphicsPipelineBuilder::SetMultisampling(VkSampleCountFlagBits msaa_samples) {
    multisample_info.sampleShadingEnable = VK_FALSE;
    multisample_info.rasterizationSamples = msaa_samples;
    multisample_info.minSampleShading = 1.0f;
    multisample_info.pSampleMask = nullptr;
    multisample_info.alphaToCoverageEnable = VK_FALSE;
    multisample_info.alphaToOneEnable = VK_FALSE;
}
void GraphicsPipelineBuilder::DisableBlending() {
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}
void GraphicsPipelineBuilder::SetColorAttachmentFormat(VkFormat attachment_format) {
    color_attachment_format = attachment_format;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &color_attachment_format;
}
void GraphicsPipelineBuilder::SetDepthFormat(VkFormat format) {
    rendering_info.depthAttachmentFormat = format;
}
void GraphicsPipelineBuilder::DisableDepthTest() {
    depth_stencil_info.depthTestEnable = VK_FALSE;
    depth_stencil_info.depthWriteEnable = VK_FALSE;
    depth_stencil_info.depthCompareOp = VK_COMPARE_OP_NEVER;
    depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_info.stencilTestEnable = VK_FALSE;
    depth_stencil_info.front = {};
    depth_stencil_info.back = {};
    depth_stencil_info.minDepthBounds = 0.0f;
    depth_stencil_info.maxDepthBounds = 1.0f;
}
}  // namespace jengine::renderer::vulkan::pipelines
