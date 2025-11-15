#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/deletion_stack.hpp"
#include "renderer/vulkan/memory_allocator.hpp"
namespace jengine::renderer::vulkan {

struct AllocatedImage {
    VkImage image{};
    VkImageView image_view{};
    VmaAllocation allocation{};
    VkExtent3D extent{};
    VkFormat format{};

    void Destroy(VkDevice device, VmaAllocator allocator);
};

AllocatedImage CreateDrawImage(uint width,
                               uint height,
                               VmaAllocator allocator,
                               VkDevice device,
                               DeletionStack& deletion_stack);

void TransitionImage(VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout);

void CopyImageBlit(VkCommandBuffer command_buffer,
                   VkImage src_image,
                   VkImage dst_image,
                   VkExtent2D src_extent,
                   VkExtent2D dst_extent);

}  // namespace jengine::renderer::vulkan
