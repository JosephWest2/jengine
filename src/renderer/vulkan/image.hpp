#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>

#include "renderer/vulkan/memory_allocator.hpp"
namespace jengine::renderer::vulkan {

class AllocatedImage {
  public:
    AllocatedImage(const vk::Extent3D& extent,
                   const vk::Format& format,
                   const vk::ImageAspectFlags& aspect_flags,
                   const vk::ImageUsageFlags& usage_flags,
                   const vk::raii::Device& device,
                   const VmaAllocator& allocator);
    ~AllocatedImage();

    const vk::Image& GetImage() const { return image; }
    const vk::ImageView& GetImageView() const { return *image_view; }
    vk::Extent3D GetExtent() { return extent; }
    vk::Format GetFormat() { return format; }

  private:
    vk::Image image{};
    vk::raii::ImageView image_view;
    VmaAllocation allocation{};
    vk::Extent3D extent{};
    vk::Format format{};

    const VmaAllocator& allocator;
};

void TransitionImage(const vk::CommandBuffer& command_buffer,
                     const vk::Image& image,
                     const vk::ImageLayout& old_layout,
                     const vk::ImageLayout& new_layout);

void CopyImageBlit(const vk::CommandBuffer& command_buffer,
                   const vk::Image& src_image,
                   const vk::Image& dst_image,
                   const vk::Extent2D& src_extent,
                   const vk::Extent2D& dst_extent);

}  // namespace jengine::renderer::vulkan
