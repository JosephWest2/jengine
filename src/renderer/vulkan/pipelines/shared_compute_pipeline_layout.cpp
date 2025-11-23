#include "shared_compute_pipeline_layout.hpp"

namespace jengine::renderer::vulkan::pipelines {
SharedComputePipelineLayout::SharedComputePipelineLayout(
    const vk::raii::Device& device,
    const vk::DescriptorSetLayout* const draw_image_descriptor_layout)
    : layout(nullptr) {
    vk::PushConstantRange push_constant_range{
        .stageFlags = vk::ShaderStageFlagBits::eCompute,
        .offset = 0,
        .size = sizeof(ComputePushConstants),
    };
    vk::PipelineLayoutCreateInfo compute_pipeline_layout_create_info{.setLayoutCount = 1,
                                                                     .pSetLayouts = draw_image_descriptor_layout,
                                                                     .pushConstantRangeCount = 1,
                                                                     .pPushConstantRanges = &push_constant_range};
    layout = device.createPipelineLayout(compute_pipeline_layout_create_info);
}
}  // namespace jengine::renderer::vulkan::pipelines
