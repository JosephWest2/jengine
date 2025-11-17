#include "context.hpp"

#include <vulkan/vulkan_core.h>

#include <iterator>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
#include "renderer/vulkan/initializers.hpp"

namespace jengine::renderer::imgui {

Context::Context(SDL_Window* window,
                 VkDevice& device,
                 VkInstance instance,
                 VkPhysicalDevice physical_device,
                 VkQueue queue,
                 uint32_t queue_family_index,
                 VkFormat* swapchain_format_ptr): device(device) {
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 100},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100}};

    VkDescriptorPoolCreateInfo pool_create_info{};
    pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_create_info.maxSets = 100;
    pool_create_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
    pool_create_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(device, &pool_create_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create imgui descriptor pool");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplSDL3_InitForVulkan(window);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physical_device;
    init_info.Device = device;
    init_info.QueueFamily = queue_family_index;
    init_info.Queue = queue;
    init_info.DescriptorPool = descriptor_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;

    init_info.UseDynamicRendering = true;

    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = swapchain_format_ptr;

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
void Context::NewFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
}
void Context::Draw(VkCommandBuffer command_buffer, VkImageView target_image_view, VkExtent2D swapchain_extent) {
    VkRenderingAttachmentInfo color_attachment = vulkan::init::RenderingAttachmentInfo(
        target_image_view, std::nullopt, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo rendering_info = vulkan::init::RenderingInfo(swapchain_extent, &color_attachment, nullptr);

    vkCmdBeginRendering(command_buffer, &rendering_info);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
    vkCmdEndRendering(command_buffer);
}
Context::~Context() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
}
}  // namespace jengine::renderer::imgui
