#include "graphics_pipeline_builder.hpp"

#include <vulkan/vulkan_core.h>
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::pipelines {

void GraphicsPipelineBuilder::Reset() { *this = GraphicsPipelineBuilder(); }

vk::raii::Pipeline GraphicsPipelineBuilder::Build(const vk::raii::Device& device) {
    vk::PipelineViewportStateCreateInfo viewport_state{
        .viewportCount = 1,
        .scissorCount = 1,
    };

    vk::PipelineColorBlendStateCreateInfo color_blend_state{
        .logicOpEnable = VK_FALSE,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment_state,
    };

    vk::PipelineVertexInputStateCreateInfo vertex_input_state{};

    std::array<vk::DynamicState, 2> dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{
        .dynamicStateCount = dynamic_states.size(),
        .pDynamicStates = dynamic_states.data(),
    };

    vk::GraphicsPipelineCreateInfo pipeline_create_info{
        .pNext = &rendering_info,
        .stageCount = static_cast<uint32_t>(shader_stages.size()),
        .pStages = shader_stages.data(),
        .pVertexInputState = &vertex_input_state,
        .pInputAssemblyState = &input_assembly_info,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterization_info,
        .pMultisampleState = &multisample_info,
        .pDepthStencilState = &depth_stencil_info,
        .pColorBlendState = &color_blend_state,
        .pDynamicState = &dynamic_state_create_info,
        .layout = pipeline_layout,
    };

    return device.createGraphicsPipeline(nullptr, pipeline_create_info);
}
void GraphicsPipelineBuilder::SetShaders(const vk::ShaderModule& vertex_shader, const vk::ShaderModule& fragment_shader) {
    shader_stages.clear();
    shader_stages.push_back({.stage = vk::ShaderStageFlagBits::eVertex, .module = vertex_shader, .pName = "main"});
    shader_stages.push_back({.stage = vk::ShaderStageFlagBits::eFragment, .module = fragment_shader, .pName = "main"});
}
void GraphicsPipelineBuilder::SetInputTopology(vk::PrimitiveTopology&& topology) {
    input_assembly_info.topology = topology;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;
}
void GraphicsPipelineBuilder::SetPolygonMode(vk::PolygonMode&& polygon_mode) {
    rasterization_info.polygonMode = polygon_mode;
    rasterization_info.lineWidth = 1.0f;
}
void GraphicsPipelineBuilder::SetCullMode(vk::CullModeFlags&& cull_mode_flags, vk::FrontFace&& front_face) {
    rasterization_info.cullMode = cull_mode_flags;
    rasterization_info.frontFace = front_face;
}
void GraphicsPipelineBuilder::SetMultisampling(vk::SampleCountFlagBits&& msaa_samples) {
    multisample_info.sampleShadingEnable = VK_FALSE;
    multisample_info.rasterizationSamples = msaa_samples;
    multisample_info.minSampleShading = 1.0f;
    multisample_info.pSampleMask = nullptr;
    multisample_info.alphaToCoverageEnable = VK_FALSE;
    multisample_info.alphaToOneEnable = VK_FALSE;
}
void GraphicsPipelineBuilder::DisableBlending() {
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
}
void GraphicsPipelineBuilder::SetColorAttachmentFormat(vk::Format attachment_format) {
    color_attachment_format = attachment_format;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachmentFormats = &color_attachment_format;
}
void GraphicsPipelineBuilder::SetDepthFormat(vk::Format format) { rendering_info.depthAttachmentFormat = format; }
void GraphicsPipelineBuilder::DisableDepthTest() {
    depth_stencil_info.depthTestEnable = VK_FALSE;
    depth_stencil_info.depthWriteEnable = VK_FALSE;
    depth_stencil_info.depthCompareOp = vk::CompareOp::eNever;
    depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_info.stencilTestEnable = VK_FALSE;
    depth_stencil_info.front = vk::StencilOpState();
    depth_stencil_info.back = vk::StencilOpState();
    depth_stencil_info.minDepthBounds = 0.0f;
    depth_stencil_info.maxDepthBounds = 1.0f;
}
}  // namespace jengine::renderer::vulkan::pipelines
