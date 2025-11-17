#include "draw_image_descriptors.hpp"

#include "renderer/vulkan/descriptors/layout_builder.hpp"

namespace jengine::renderer::vulkan::descriptors {

DrawImageDescriptors::DrawImageDescriptors(DescriptorAllocator& allocator,
                                           VkDevice& device,
                                           VkImageView draw_image_view): device(device) {
    DescriptorLayoutBuilder layout_builder{};
    layout_builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    descriptor_set_layout = layout_builder.Build(device, VK_SHADER_STAGE_COMPUTE_BIT);
    descriptor_set = allocator.Allocate(descriptor_set_layout);

    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    image_info.imageView = draw_image_view;

    VkWriteDescriptorSet draw_image_write{};
    draw_image_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    draw_image_write.pNext = nullptr;

    draw_image_write.dstSet = descriptor_set;
    draw_image_write.dstBinding = 0;

    draw_image_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    draw_image_write.descriptorCount = 1;

    draw_image_write.pImageInfo = &image_info;

    vkUpdateDescriptorSets(device, 1, &draw_image_write, 0, nullptr);

}
DrawImageDescriptors::~DrawImageDescriptors() {
    vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
}
}  // namespace jengine::renderer::vulkan::descriptors
