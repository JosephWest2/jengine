#include "instance.hpp"

#include <SDL3/SDL_vulkan.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "VkBootstrap.h"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {

Instance::Instance(const char* app_name, bool use_validation_layers)
    : context(), instance(CreateInstance(context, app_name, use_validation_layers)) {
    vkb::InstanceBuilder instance_builder;
}

bool LayersAvailable(std::vector<const char*> validation_layers) {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
    for (auto& validation_layer : validation_layers) {
        bool found = std::ranges::any_of(
            layers, [&](const auto& layer) { return strcmp(layer.layerName, validation_layer) == 1; });
        if (!found) {
            std::cerr << "Requested validation layer not available: " << validation_layer << std::endl;
            return false;
        }
    }
    return true;
}

vk::raii::Instance CreateInstance(vk::raii::Context& context, const char* app_name, bool use_validation_layers) {
    vk::ApplicationInfo app_info = {
        .pApplicationName = app_name,
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "JEngine",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_3,
    };

    uint32_t extension_count;
    auto extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);

    std::vector<const char*> extension_names(extensions, extensions + extension_count);
    extension_names.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    std::vector<const char*> validation_layers;
    if (use_validation_layers) {
        validation_layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    if (!LayersAvailable(validation_layers)) {
        throw std::runtime_error("Requested validation layers not available");
    }

    vk::InstanceCreateInfo create_info = {
        .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(validation_layers.size()),
        .ppEnabledLayerNames = validation_layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extension_names.size()),
        .ppEnabledExtensionNames = extension_names.data(),
    };

    return vk::raii::Instance(context, create_info);
}
}  // namespace jengine::renderer::vulkan
