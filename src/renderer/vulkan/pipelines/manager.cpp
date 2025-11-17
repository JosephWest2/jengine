#include "manager.hpp"

namespace jengine::renderer::vulkan::pipelines {

Manager::Manager(VkDevice& device, VkDescriptorSetLayout* draw_image_descriptor_layout_ptr)
    : gradient_pipeline(draw_image_descriptor_layout_ptr, device) {}
}  // namespace jengine::renderer::vulkan::pipelines
