#include "instance.hpp"

#include <iostream>
#include <stdexcept>
#include "VkBootstrap.h"
#include <format>

namespace jengine::renderer::vulkan {

Instance::Instance(const char* app_name, bool use_validation_layers) {
    if (vulkan_initialized) {
        throw std::runtime_error("Vulkan instance already initialized, multiple instances not supported");
    }
    vkb::InstanceBuilder instance_builder;

    auto res = instance_builder
        .set_app_name("jengine")
        .request_validation_layers(use_validation_layers)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

    if (!res.has_value()) {
        throw std::runtime_error(std::format("Failed to create Vulkan instance: {}", res.error().message()));
    }

    vkb_instance = res.value();
    vulkan_initialized = true;
}


Instance::Instance(Instance&& other) {
    vkb_instance = other.vkb_instance;
    other.vkb_instance = {};
};

Instance& Instance::operator=(Instance&& other) {
    if (this != &other) {
        vkb_instance = other.vkb_instance;
        other.vkb_instance = {};
    }
    return *this;
}
Instance::~Instance() {
    Destroy();
};
void Instance::Destroy() {
    if (vulkan_initialized) {
        std::cout << "Destroying Vulkan instance" << std::endl;
        vkb::destroy_instance(vkb_instance);
        vulkan_initialized = false;
    }
}
}  // namespace jengine::renderer::vulkan
