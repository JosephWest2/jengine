#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>
namespace jengine::renderer::vulkan::pipelines {
class GraphicsPipelineBuilder {
  public:
    GraphicsPipelineBuilder();

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages{};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
    VkPipelineRasterizationStateCreateInfo rasterization_info{};
    VkPipelineMultisampleStateCreateInfo multisample_info{};
    VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
    VkPipelineLayout pipeline_layout{};
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
    VkPipelineRenderingCreateInfo rendering_info{};
    VkFormat color_attachment_format{};

    void Reset();

    void SetShaders(VkShaderModule vertex_shader, VkShaderModule fragment_shader);
    void SetInputTopology(VkPrimitiveTopology topology);
    void SetPolygonMode(VkPolygonMode polygon_mode);
    void SetCullMode(VkCullModeFlags cull_mode_flags, VkFrontFace front_face);
    void SetMultisampling(VkSampleCountFlagBits msaa_samples);
    void DisableBlending();
    void SetColorAttachmentFormat(VkFormat attachment_format);
    void SetDepthFormat(VkFormat format);
    void DisableDepthTest();
    VkPipeline Build(VkDevice device);

  private:
};

}  // namespace jengine::renderer::vulkan::pipelines
