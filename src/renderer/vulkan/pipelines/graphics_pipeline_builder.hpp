#pragma once

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>

#include <vector>
#include <vulkan/vulkan_raii.hpp>
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::pipelines {
class GraphicsPipelineBuilder {
  public:
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages{};
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info{};
    vk::PipelineRasterizationStateCreateInfo rasterization_info{};
    vk::PipelineMultisampleStateCreateInfo multisample_info{};
    vk::PipelineColorBlendAttachmentState color_blend_attachment_state{};
    vk::PipelineLayout pipeline_layout{};
    vk::PipelineDepthStencilStateCreateInfo depth_stencil_info{};
    vk::PipelineRenderingCreateInfo rendering_info{};
    vk::Format color_attachment_format{};

    void Reset();

    void SetShaders(const vk::ShaderModule& vertex_shader, const vk::ShaderModule& fragment_shader);
    void SetInputTopology(vk::PrimitiveTopology&& topology);
    void SetPolygonMode(vk::PolygonMode&& polygon_mode);
    void SetCullMode(vk::CullModeFlags&& cull_mode_flags, vk::FrontFace&& front_face);
    void SetMultisampling(vk::SampleCountFlagBits&& msaa_samples);
    void DisableBlending();
    void SetColorAttachmentFormat(vk::Format attachment_format);
    void SetDepthFormat(vk::Format format);
    void EnableDepthTest(bool enable_depth_write, vk::CompareOp depth_compare_op);
    void DisableDepthTest();
    void EnableBlendingAdditive();
    void EnableBlendingAlpha();
    vk::raii::Pipeline Build(const vk::raii::Device& device);

  private:
};

}  // namespace jengine::renderer::vulkan::pipelines
