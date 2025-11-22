#include "draw_image_descriptors.hpp"

#include "renderer/vulkan/descriptors/layout_builder.hpp"

namespace jengine::renderer::vulkan::descriptors {

DrawImageDescriptors::DrawImageDescriptors(const DescriptorAllocator& allocator,
                                           vk::Device& device,
                                           vk::ImageView draw_image_view)
    : device(device) {
    DescriptorLayoutBuilder layout_builder{};
    layout_builder.AddBinding(0, vk::DescriptorType::eStorageImage);
    descriptor_set_layout = layout_builder.Build(device, vk::ShaderStageFlagBits::eCompute);
    descriptor_set = allocator.Allocate(descriptor_set_layout);

    vk::DescriptorImageInfo image_info{
        .imageView = draw_image_view,
        .imageLayout = vk::ImageLayout::eGeneral,
    };

    vk::WriteDescriptorSet draw_image_write{
        .dstSet = descriptor_set,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eStorageImage,
        .pImageInfo = &image_info,
    };
    device.updateDescriptorSets({draw_image_write}, {});
}
DrawImageDescriptors::~DrawImageDescriptors() { vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr); }
}  // namespace jengine::renderer::vulkan::descriptors
