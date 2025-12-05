#pragma once

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
#include <optional>
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::init {

vk::ImageSubresourceRange ImageSubresourceRange(vk::ImageAspectFlags aspect_mask);
vk::SubmitInfo2 SubmitInfo2(vk::CommandBufferSubmitInfo* command_buffer_submit_info,
                          vk::SemaphoreSubmitInfo* wait_semaphore_submit_info,
                          vk::SemaphoreSubmitInfo* signal_semaphore_submit_info);
vk::ImageCreateInfo ImageCreateInfo(vk::Format format, vk::ImageUsageFlags usage_flags, vk::Extent3D extent);
vk::ImageViewCreateInfo ImageViewCreateInfo(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect_flags);
// if clear_value is set, image will be cleared with the color
vk::RenderingAttachmentInfo RenderingAttachmentInfo(vk::ImageView image_view,
                                                  std::optional<vk::ClearValue> clear_value,
                                                  vk::ImageLayout image_layout);
vk::RenderingAttachmentInfo DepthAttachmentInfo(vk::ImageView image_view, vk::ImageLayout image_layout);
vk::RenderingInfo RenderingInfo(vk::Extent2D render_extent,
                              vk::RenderingAttachmentInfo* color_attachment,
                              vk::RenderingAttachmentInfo* depth_attachment);
}  // namespace jengine::renderer::vulkan::init
