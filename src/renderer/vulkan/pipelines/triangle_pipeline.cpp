#include "triangle_pipeline.hpp"

#include "renderer/vulkan/pipelines/graphics_pipeline_builder.hpp"
#include "renderer/vulkan/pipelines/load_shader_module.hpp"

namespace jengine::renderer::vulkan::pipelines {
TrianglePipeline::TrianglePipeline(const vk::raii::Device& device, const vk::Format& draw_image_format,
                                   const vk::Format& depth_image_format)
    : pipeline(nullptr) {
    vk::raii::ShaderModule fragment_shader = LoadShaderModule("shaders/colored_triangle.frag.spv", device);
    vk::raii::ShaderModule vertex_shader = LoadShaderModule("shaders/colored_triangle.vert.spv", device);

    vk::raii::PipelineLayout triangle_pipeline_layout = device.createPipelineLayout(vk::PipelineLayoutCreateInfo{});

    GraphicsPipelineBuilder pipeline_builder{};
    pipeline_builder.pipeline_layout = triangle_pipeline_layout;
    pipeline_builder.SetShaders(vertex_shader, fragment_shader);
    pipeline_builder.SetInputTopology(vk::PrimitiveTopology::eTriangleList);
    pipeline_builder.SetPolygonMode(vk::PolygonMode::eFill);
    pipeline_builder.SetCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise);
    pipeline_builder.SetMultisampling(vk::SampleCountFlagBits::e1);
    pipeline_builder.SetColorAttachmentFormat(draw_image_format);
    pipeline_builder.SetDepthFormat(depth_image_format);
    pipeline_builder.DisableBlending();
    pipeline_builder.DisableDepthTest();
    pipeline = pipeline_builder.Build(device);
}
}  // namespace jengine::renderer::vulkan::pipelines
