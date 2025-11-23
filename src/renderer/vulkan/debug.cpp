#include "debug.hpp"

#include <iostream>

namespace jengine::renderer::vulkan {
DebugMessenger::DebugMessenger(const vk::raii::Instance& instance) : debug_messenger(nullptr) {
    vk::DebugUtilsMessengerCreateInfoEXT create_info = {
        .messageSeverity =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType =
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = &DebugCallback,
        .pUserData = nullptr,
    };

    debug_messenger = vk::raii::DebugUtilsMessengerEXT(instance, create_info);
}
VkBool32 VKAPI_CALL DebugMessenger::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                  vk::DebugUtilsMessageTypeFlagsEXT message_type
                                                  __attribute__((unused)),
                                                  const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
                                                  void* user_data __attribute__((unused))) {
    if (message_severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        std::cerr << "Validation layer: " << callback_data->pMessage << std::endl;
    }
    return VK_FALSE;
}
}  // namespace jengine::renderer::vulkan
