#include "manager.hpp"

#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "renderer/vulkan/initializers.hpp"
#include "renderer/vulkan/pipelines/graphics_pipeline_builder.hpp"
#include "renderer/vulkan/pipelines/load_shader_module.hpp"

namespace jengine::renderer::vulkan::pipelines {

Manager::Manager(VkDevice& device, VkFormat draw_image_format, VkDescriptorSetLayout* draw_image_descriptor_layout_ptr)
    : gradient_and_sky_pipeline(draw_image_descriptor_layout_ptr, device), device(device) {
        InitTrianglePipeline(device, draw_image_format);
    }

void Manager::InitTrianglePipeline(VkDevice& device, VkFormat draw_image_format) {
    VkShaderModule fragment_shader = LoadShaderModule("shaders/colored_triangle.frag.spv", device);
    VkShaderModule vertex_shader = LoadShaderModule("shaders/colored_triangle.vert.spv", device);

    VkPipelineLayoutCreateInfo layout_create_info = init::PipelineLayoutCreateInfo();
    if (vkCreatePipelineLayout(device, &layout_create_info, nullptr, &triangle_pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    GraphicsPipelineBuilder pipeline_builder{};
    pipeline_builder.pipeline_layout = triangle_pipeline_layout;
    pipeline_builder.SetShaders(vertex_shader, fragment_shader);
    pipeline_builder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline_builder.SetPolygonMode(VK_POLYGON_MODE_FILL);
    pipeline_builder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    pipeline_builder.SetMultisampling(VK_SAMPLE_COUNT_1_BIT);
    pipeline_builder.SetColorAttachmentFormat(draw_image_format);
    pipeline_builder.SetDepthFormat(VK_FORMAT_UNDEFINED);
    pipeline_builder.DisableBlending();
    pipeline_builder.DisableDepthTest();
    triangle_pipeline = pipeline_builder.Build(device);

    vkDestroyShaderModule(device, vertex_shader, nullptr);
    vkDestroyShaderModule(device, fragment_shader, nullptr);
}
Manager::~Manager() {
    if (triangle_pipeline) {
        vkDestroyPipeline(device, triangle_pipeline, nullptr);
    }
    if (triangle_pipeline_layout) {
        vkDestroyPipelineLayout(device, triangle_pipeline_layout, nullptr);
    }
}
}  // namespace jengine::renderer::vulkan::pipelines
