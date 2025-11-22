#include "manager.hpp"

#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "renderer/vulkan/pipelines/graphics_pipeline_builder.hpp"
#include "renderer/vulkan/pipelines/load_shader_module.hpp"

namespace jengine::renderer::vulkan::pipelines {

Manager::Manager(const vk::Device& device, vk::Format draw_image_format, vk::DescriptorSetLayout* draw_image_descriptor_layout_ptr)
    : gradient_and_sky_pipeline(draw_image_descriptor_layout_ptr, device), device(device) {}

Manager::~Manager() {
    if (triangle_pipeline) {
        vkDestroyPipeline(device, triangle_pipeline, nullptr);
    }
    if (triangle_pipeline_layout) {
        vkDestroyPipelineLayout(device, triangle_pipeline_layout, nullptr);
    }
}
}  // namespace jengine::renderer::vulkan::pipelines
