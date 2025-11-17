#include "initializers.hpp"

#include <vulkan/vulkan_core.h>
namespace jengine::renderer::vulkan::init {

VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t graphics_queue_family_index, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.queueFamilyIndex = graphics_queue_family_index;
    create_info.flags = flags;
    return create_info;
}

VkCommandBufferAllocateInfo PrimaryCommandBufferAllocateInfo(VkCommandPool pool, uint32_t count) {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = count;
    return alloc_info;
}

VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags) {
    VkFenceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = flags;
    return create_info;
}

VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags) {
    VkSemaphoreCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = flags;
    return create_info;
}
VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags usage_flags) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = nullptr;
    begin_info.pInheritanceInfo = nullptr;
    begin_info.flags = usage_flags;
    return begin_info;
}
VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspect_mask) {
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = aspect_mask;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = VK_REMAINING_MIP_LEVELS;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount = VK_REMAINING_ARRAY_LAYERS;
    return subresource_range;
}
VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkSemaphore semaphore, VkPipelineStageFlags stage_flags) {
    VkSemaphoreSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.semaphore = semaphore;
    submit_info.stageMask = stage_flags;
    submit_info.deviceIndex = 0;
    submit_info.value = 0;
    return submit_info;
}
VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer command_buffer) {
    VkCommandBufferSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.commandBuffer = command_buffer;
    submit_info.deviceMask = 0;
    return submit_info;
}
VkSubmitInfo2 SubmitInfo2(VkCommandBufferSubmitInfo* command_buffer_submit_info,
                          VkSemaphoreSubmitInfo* wait_semaphore_submit_info,
                          VkSemaphoreSubmitInfo* signal_semaphore_submit_info) {
    VkSubmitInfo2 submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.pNext = nullptr;

    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = command_buffer_submit_info;

    submit_info.signalSemaphoreInfoCount = signal_semaphore_submit_info ? 1 : 0;
    submit_info.pSignalSemaphoreInfos = signal_semaphore_submit_info;

    submit_info.waitSemaphoreInfoCount = wait_semaphore_submit_info ? 1 : 0;
    submit_info.pWaitSemaphoreInfos = wait_semaphore_submit_info;

    return submit_info;
}
VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent) {
    VkImageCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.imageType = VK_IMAGE_TYPE_2D;

    create_info.format = format;
    create_info.extent = extent;

    create_info.mipLevels = 1;
    create_info.arrayLayers = 1;

    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    create_info.usage = usage_flags;

    // for MSAA
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;

    return create_info;
}
VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags) {
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.image = image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format;
    create_info.subresourceRange.aspectMask = aspect_flags;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.layerCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.baseMipLevel = 0;
    return create_info;
}
VkRenderingAttachmentInfo RenderingAttachmentInfo(VkImageView image_view,
                                                  std::optional<VkClearValue> clear_value,
                                                  VkImageLayout image_layout) {
    VkRenderingAttachmentInfo attachment_info = {};
    attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachment_info.pNext = nullptr;
    attachment_info.imageView = image_view;
    attachment_info.imageLayout = image_layout;
    attachment_info.loadOp = clear_value ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_info.clearValue = clear_value ? clear_value.value() : VkClearValue();
    return attachment_info;
}
VkRenderingInfo RenderingInfo(VkExtent2D render_extent,
                              VkRenderingAttachmentInfo* color_attachment,
                              VkRenderingAttachmentInfo* depth_attachment) {
    VkRenderingInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.pNext = nullptr;
    render_info.renderArea = VkRect2D{VkOffset2D{0, 0}, render_extent};
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = color_attachment;
    render_info.pDepthAttachment = depth_attachment;
    render_info.pStencilAttachment = nullptr;
    return render_info;
}
}  // namespace jengine::renderer::vulkan::init
