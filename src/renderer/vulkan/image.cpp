#include "image.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <stdexcept>

#include "renderer/vulkan/initializers.hpp"

namespace jengine::renderer::vulkan {
void TransitionImage(VkCommandBuffer command_buffer,
                     VkImage image,
                     VkImageLayout old_layout,
                     VkImageLayout new_layout) {
    VkImageMemoryBarrier2 image_barrier{};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_barrier.pNext = nullptr;

    image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    image_barrier.oldLayout = old_layout;
    image_barrier.newLayout = new_layout;

    image_barrier.image = image;

    VkImageAspectFlags aspect_mask =
        old_layout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange = init::ImageSubresourceRange(aspect_mask);

    VkDependencyInfo dependency_info{};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.pNext = nullptr;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &image_barrier;

    vkCmdPipelineBarrier2(command_buffer, &dependency_info);
}
AllocatedImage CreateDrawImage(uint width, uint height, VmaAllocator allocator, VkDevice device, DeletionStack& deletion_stack) {
    AllocatedImage result = {
        .extent = {.width = width, .height = height, .depth = 1},
        .format = VK_FORMAT_R16G16B16A16_SFLOAT,
    };

    VkImageCreateInfo image_create_info =
        init::ImageCreateInfo(result.format,
                              VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

                              result.extent);
    VmaAllocationCreateInfo allocation_create_info = {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocation_create_info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (vmaCreateImage(
            allocator, &image_create_info, &allocation_create_info, &result.image, &result.allocation, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create draw image");
    }

    VkImageViewCreateInfo image_view_create_info =
        init::ImageViewCreateInfo(result.image, result.format, VK_IMAGE_ASPECT_COLOR_BIT);

    assert(device);
    assert(result.image);
    if (vkCreateImageView(device, &image_view_create_info, nullptr, &result.image_view) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create draw image view");
    }

    deletion_stack.Push([device, allocator, result]() {
        vkDestroyImageView(device, result.image_view, nullptr);
        vmaDestroyImage(allocator, result.image, result.allocation);
    });

    return result;
}
void CopyImageBlit(VkCommandBuffer command_buffer,
                   VkImage src_image,
                   VkImage dst_image,
                   VkExtent2D src_extent,
                   VkExtent2D dst_extent) {
    VkImageBlit2 image_blit{};
    image_blit.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
    image_blit.pNext = nullptr;

    image_blit.srcSubresource.layerCount = 1;
    image_blit.srcSubresource.baseArrayLayer = 0;
    image_blit.srcSubresource.mipLevel = 0;
    image_blit.srcOffsets[1] = {static_cast<int>(src_extent.width), static_cast<int>(src_extent.height), 1};
    image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    image_blit.dstSubresource.layerCount = 1;
    image_blit.dstSubresource.baseArrayLayer = 0;
    image_blit.dstSubresource.mipLevel = 0;
    image_blit.dstOffsets[1] = {static_cast<int>(dst_extent.width), static_cast<int>(dst_extent.height), 1};
    image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkBlitImageInfo2 blit_image_info{};
    blit_image_info.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    blit_image_info.pNext = nullptr;
    blit_image_info.srcImage = src_image;
    blit_image_info.dstImage = dst_image;
    blit_image_info.regionCount = 1;
    blit_image_info.pRegions = &image_blit;
    blit_image_info.filter = VK_FILTER_LINEAR;
    blit_image_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    blit_image_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    vkCmdBlitImage2(command_buffer, &blit_image_info);
}
void AllocatedImage::Destroy(VkDevice device, VmaAllocator allocator) {
    assert(image);
    assert(image_view);
    assert(device);
    assert(allocator);
    vkDestroyImageView(device, image_view, nullptr);
    vmaDestroyImage(allocator, image, allocation);
    *this = {};
}
}  // namespace jengine::renderer::vulkan
