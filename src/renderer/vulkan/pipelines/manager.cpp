#include "manager.hpp"

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/pipelines/gradient_pipeline.hpp"
#include "renderer/vulkan/pipelines/shared_compute_pipeline_layout.hpp"
#include "util/should_be_unreachable.hpp"

namespace jengine::renderer::vulkan::pipelines {

Manager::Manager(const vk::raii::Device& device,
                 vk::Format draw_image_format,
                 const vk::DescriptorSetLayout* draw_image_descriptor_layout)
    : shared_compute_pipeline_layout(device, draw_image_descriptor_layout),
      gradient_pipeline(device, shared_compute_pipeline_layout.GetPipelineLayout()),
      sky_pipeline(device, shared_compute_pipeline_layout.GetPipelineLayout()),
      triangle_pipeline(device, draw_image_format),
      mesh_pipeline(device, draw_image_format) {}

const vk::Pipeline& Manager::GetSelectedSharedComputePipeline() const {
    switch (selected_shared_compute_pipeline) {
        case SelectedSharedComputePipeline::GRADIENT:
            return gradient_pipeline.GetPipeline();
        case SelectedSharedComputePipeline::SKY:
            return sky_pipeline.GetPipeline();
    }
    util::should_be_unreachable();
}
const std::string_view Manager::GetSelectedSharedComputePipelineName() const {
    switch (selected_shared_compute_pipeline) {
        case SelectedSharedComputePipeline::GRADIENT:
            return "gradient";
        case SelectedSharedComputePipeline::SKY:
            return "sky";
    }
    util::should_be_unreachable();
}
}  // namespace jengine::renderer::vulkan::pipelines
