#include "image.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <stdexcept>

#include "renderer/vulkan/initializers.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {
void TransitionImage(const vk::CommandBuffer& command_buffer,
                     const vk::Image& image,
                     const vk::ImageLayout& old_layout,
                     const vk::ImageLayout& new_layout) {
    vk::ImageMemoryBarrier2 image_barrier{

        .srcStageMask = vk::PipelineStageFlagBits2::eAllCommands,
        .srcAccessMask = vk::AccessFlagBits2::eMemoryWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eAllCommands,
        .dstAccessMask = vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,

        .oldLayout = old_layout,
        .newLayout = new_layout,

        .image = image,
    };

    vk::ImageAspectFlags aspect_mask = old_layout == vk::ImageLayout::eDepthReadOnlyOptimal
                                           ? vk::ImageAspectFlagBits::eDepth
                                           : vk::ImageAspectFlagBits::eColor;
    image_barrier.subresourceRange = init::ImageSubresourceRange(aspect_mask);

    vk::DependencyInfo dependency_info{.imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &image_barrier};

    command_buffer.pipelineBarrier2(&dependency_info);
}
void CopyImageBlit(const vk::CommandBuffer& command_buffer,
                   const vk::Image& src_image,
                   const vk::Image& dst_image,
                   const vk::Extent2D& src_extent,
                   const vk::Extent2D& dst_extent) {
    vk::ImageBlit2 image_blit{
        .srcSubresource =
            {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .layerCount = 1,
            },
        .dstSubresource =
            {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .layerCount = 1,
            },

    };
    image_blit.srcOffsets[1] = vk::Offset3D{static_cast<int>(src_extent.width), static_cast<int>(src_extent.height), 1};
    image_blit.dstOffsets[1] = vk::Offset3D{static_cast<int>(dst_extent.width), static_cast<int>(dst_extent.height), 1};

    vk::BlitImageInfo2 blit_image_info{
        .srcImage = src_image,
        .srcImageLayout = vk::ImageLayout::eTransferSrcOptimal,
        .dstImage = dst_image,
        .dstImageLayout = vk::ImageLayout::eTransferDstOptimal,
        .regionCount = 1,
        .pRegions = &image_blit,
        .filter = vk::Filter::eLinear,
    };

    command_buffer.blitImage2(&blit_image_info);
}
AllocatedImage::~AllocatedImage() {
    device.destroyImageView(image_view);
    vmaDestroyImage(allocator, image, allocation);
}
AllocatedImage::AllocatedImage(const vk::Extent3D& extent,
                               const vk::Format& format,
                               const vk::ImageUsageFlags& usage_flags,
                               const vk::Device& device,
                               const VmaAllocator& allocator)
    : extent(extent), format(format), device(device), allocator(allocator) {
    vk::ImageCreateInfo image_create_info = init::ImageCreateInfo(format, usage_flags, extent);
    VmaAllocationCreateInfo allocation_create_info = {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocation_create_info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkImage result;
    if (vmaCreateImage(allocator,
                       &static_cast<VkImageCreateInfo&>(image_create_info),
                       &allocation_create_info,
                       &result,
                       &allocation,
                       nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image");
    }
    image = result;

    vk::ImageViewCreateInfo image_view_create_info =
        init::ImageViewCreateInfo(image, format, vk::ImageAspectFlagBits::eColor);

    image_view = device.createImageView(image_view_create_info);
}
}  // namespace jengine::renderer::vulkan
