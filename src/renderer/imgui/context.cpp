#include "context.hpp"

#include <vulkan/vulkan_core.h>

#include <functional>
#include <iterator>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
#include "renderer/vulkan/initializers.hpp"

namespace jengine::renderer::imgui {

Context::Context(SDL_Window* window,
                 const vk::raii::Device& device,
                 const vk::Instance& instance,
                 const vk::PhysicalDevice& physical_device,
                 const vk::Queue& queue,
                 uint32_t queue_family_index,
                 const vk::Format swapchain_format)
    : descriptor_pool(nullptr) {
    constexpr uint32_t POOL_SIZE = 100;
    vk::DescriptorPoolSize pool_sizes[] = {{vk::DescriptorType::eSampler, POOL_SIZE},
                                           {vk::DescriptorType::eCombinedImageSampler, POOL_SIZE},
                                           {vk::DescriptorType::eSampledImage, POOL_SIZE},
                                           {vk::DescriptorType::eStorageImage, POOL_SIZE},
                                           {vk::DescriptorType::eUniformTexelBuffer, POOL_SIZE},
                                           {vk::DescriptorType::eStorageTexelBuffer, POOL_SIZE},
                                           {vk::DescriptorType::eUniformBuffer, POOL_SIZE},
                                           {vk::DescriptorType::eStorageBuffer, POOL_SIZE},
                                           {vk::DescriptorType::eUniformBufferDynamic, POOL_SIZE},
                                           {vk::DescriptorType::eStorageBufferDynamic, POOL_SIZE},
                                           {vk::DescriptorType::eInputAttachment, POOL_SIZE}};

    vk::DescriptorPoolCreateInfo pool_create_info{.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                                  .maxSets = POOL_SIZE,
                                                  .poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes)),
                                                  .pPoolSizes = pool_sizes};

    descriptor_pool = device.createDescriptorPool(pool_create_info);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplSDL3_InitForVulkan(window);

    VkFormat swapchain_format_vk = static_cast<VkFormat>(swapchain_format);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physical_device;
    init_info.Device = *device;
    init_info.QueueFamily = queue_family_index;
    init_info.Queue = queue;
    init_info.DescriptorPool = *descriptor_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;

    init_info.UseDynamicRendering = true;

    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchain_format_vk;

    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);

    SDL_SetEventFilter(&Context::EventFilter, nullptr);
}
bool Context::EventFilter(void* user_data __attribute__((unused)), SDL_Event* event) {
    ImGui_ImplSDL3_ProcessEvent(event);
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP ||
            event->type == SDL_EVENT_MOUSE_MOTION || event->type == SDL_EVENT_MOUSE_WHEEL ||
            event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
            return false;
        }
    }
    return true;
}
void Context::NewFrame(std::function<void()>&& fn) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    fn();
    ImGui::ShowDemoWindow();
    ImGui::Render();
}
void Context::Draw(const vk::CommandBuffer& command_buffer,
                   const vk::ImageView& target_image_view,
                   const vk::Extent2D& swapchain_extent) {
    vk::RenderingAttachmentInfo color_attachment = vulkan::init::RenderingAttachmentInfo(
        target_image_view, std::nullopt, vk::ImageLayout::eColorAttachmentOptimal);
    vk::RenderingInfo rendering_info = vulkan::init::RenderingInfo(swapchain_extent, &color_attachment, nullptr);

    command_buffer.beginRendering(rendering_info);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
    command_buffer.endRendering();
}
Context::~Context() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
}  // namespace jengine::renderer::imgui
