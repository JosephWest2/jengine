#include "mesh_pipeline.hpp"
#include <print>

#include "renderer/vulkan/buffers/mesh_buffers.hpp"
#include "renderer/vulkan/pipelines/graphics_pipeline_builder.hpp"
#include "renderer/vulkan/pipelines/load_shader_module.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::pipelines {
MeshPipeline::MeshPipeline(const vk::raii::Device& device,
                           const vk::Format& draw_image_format,
                           const vk::Format& depth_image_format)
    : pipeline_layout(nullptr), pipeline(nullptr) {
    auto fragment_shader = LoadShaderModule("shaders/colored_triangle.frag.spv", device);
    auto vertex_shader = LoadShaderModule("shaders/colored_triangle_mesh.vert.spv", device);

    vk::PushConstantRange push_constant_range{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .offset = 0,
        .size = sizeof(buffers::MeshDrawPushConstants),
    };
    vk::PipelineLayoutCreateInfo pipeline_layout_create_info{
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &push_constant_range,
    };
    pipeline_layout = device.createPipelineLayout(pipeline_layout_create_info);

    GraphicsPipelineBuilder pipeline_builder{};
    pipeline_builder.pipeline_layout = pipeline_layout;
    pipeline_builder.SetShaders(vertex_shader, fragment_shader);
    pipeline_builder.SetInputTopology(vk::PrimitiveTopology::eTriangleList);
    pipeline_builder.SetPolygonMode(vk::PolygonMode::eFill);
    pipeline_builder.SetCullMode(vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise);
    pipeline_builder.SetMultisampling(vk::SampleCountFlagBits::e1);
    pipeline_builder.SetColorAttachmentFormat(draw_image_format);
    std::println("DEPTH IMAGE FORMAT {}", depth_image_format == vk::Format::eD32Sfloat);
    pipeline_builder.SetDepthFormat(depth_image_format);
    pipeline_builder.EnableBlendingAdditive();
    pipeline_builder.EnableDepthTest(true, vk::CompareOp::eGreaterOrEqual);
    pipeline = pipeline_builder.Build(device);
}
}  // namespace jengine::renderer::vulkan::pipelines
