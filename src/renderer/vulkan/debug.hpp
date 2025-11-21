#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {

class DebugMessenger {
  public:
    DebugMessenger(vk::raii::Instance& instance);

    vk::raii::DebugUtilsMessengerEXT& GetDebugMessenger() { return debug_messenger; }

  private:
    vk::raii::DebugUtilsMessengerEXT debug_messenger;

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                             vk::DebugUtilsMessageTypeFlagsEXT message_type __attribute__((unused)),
                                             const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
                                             void* user_data __attribute__((unused)));

    static vk::raii::DebugUtilsMessengerEXT CreateDebugUtilsMessenger(vk::raii::Instance& instance);
};

}  // namespace jengine::renderer::vulkan
