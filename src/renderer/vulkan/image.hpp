#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/memory_allocator.hpp"
namespace jengine::renderer::vulkan {

class AllocatedImage {
  public:
    AllocatedImage(VkExtent3D extent,
                   VkFormat format,
                   VkImageUsageFlags usage_flags,
                   VkDevice& device,
                   VmaAllocator& allocator);
    ~AllocatedImage();

    VkImage GetImage() const { return image; }
    VkImageView GetImageView() const { return image_view; }
    VkExtent3D GetExtent() const { return extent; }
    VkFormat GetFormat() const { return format; }

  private:
    VkImage image{};
    VkImageView image_view{};
    VmaAllocation allocation{};
    VkExtent3D extent{};
    VkFormat format{};

    // held for destruction
    VkDevice& device;
    VmaAllocator& allocator;
};

void TransitionImage(VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout);

void CopyImageBlit(VkCommandBuffer command_buffer,
                   VkImage src_image,
                   VkImage dst_image,
                   VkExtent2D src_extent,
                   VkExtent2D dst_extent);

}  // namespace jengine::renderer::vulkan
