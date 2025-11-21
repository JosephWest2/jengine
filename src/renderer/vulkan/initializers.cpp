#include "initializers.hpp"

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
namespace jengine::renderer::vulkan::init {

vk::ImageSubresourceRange ImageSubresourceRange(vk::ImageAspectFlags aspect_mask) {
    return vk::ImageSubresourceRange{
        .aspectMask = aspect_mask,
        .levelCount = vk::RemainingMipLevels,
        .layerCount = vk::RemainingArrayLayers,
    };
}
vk::SubmitInfo2 SubmitInfo2(vk::CommandBufferSubmitInfo* command_buffer_submit_info,
                            vk::SemaphoreSubmitInfo* wait_semaphore_submit_info,
                            vk::SemaphoreSubmitInfo* signal_semaphore_submit_info) {
    return vk::SubmitInfo2{
        .waitSemaphoreInfoCount = static_cast<uint32_t>(wait_semaphore_submit_info ? 1 : 0),
        .pWaitSemaphoreInfos = wait_semaphore_submit_info,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = command_buffer_submit_info,
        .signalSemaphoreInfoCount = static_cast<uint32_t>(signal_semaphore_submit_info ? 1 : 0),
        .pSignalSemaphoreInfos = signal_semaphore_submit_info,
    };
}
vk::ImageCreateInfo ImageCreateInfo(vk::Format format, vk::ImageUsageFlags usage_flags, vk::Extent3D extent) {
    return vk::ImageCreateInfo{
        .imageType = vk::ImageType::e2D,
        .format = format,
        .extent = extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = usage_flags,
    };
}
vk::ImageViewCreateInfo ImageViewCreateInfo(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect_flags) {
    return vk::ImageViewCreateInfo{
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange =
            vk::ImageSubresourceRange{
                .aspectMask = aspect_flags,
                .levelCount = 1,
                .layerCount = 1,
            },
    };
}
vk::RenderingAttachmentInfo RenderingAttachmentInfo(vk::ImageView image_view,
                                                    std::optional<vk::ClearValue> clear_value,
                                                    vk::ImageLayout image_layout) {
    return vk::RenderingAttachmentInfo{
        .imageView = image_view,
        .imageLayout = image_layout,
        .loadOp = clear_value ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = {clear_value ? clear_value.value() : vk::ClearValue{}},
    };
}
vk::RenderingInfo RenderingInfo(vk::Extent2D render_extent,
                                vk::RenderingAttachmentInfo* color_attachment,
                                vk::RenderingAttachmentInfo* depth_attachment) {
    return vk::RenderingInfo{
        .renderArea = vk::Rect2D{vk::Offset2D{0, 0}, render_extent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = color_attachment,
        .pDepthAttachment = depth_attachment,
    };
}
}  // namespace jengine::renderer::vulkan::init
