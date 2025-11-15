#include "device.hpp"

#include <format>
#include <iostream>
#include "renderer/vulkan/deletion_stack.hpp"

namespace jengine::renderer::vulkan {

Device::Device(vkb::PhysicalDevice& physical_device, DeletionStack& deletion_stack) {
    vkb::DeviceBuilder device_builder(physical_device);

    auto device_res = device_builder.build();
    if (!device_res.has_value()) {
        throw std::runtime_error(std::format("Failed to create Vulkan device: {}", device_res.error().message()));
    }

    vkb_device = device_res.value();

    deletion_stack.Push([this]() { Destroy(); });
}

void Device::Destroy() {
    std::cout << "Destroying Vulkan device" << std::endl;
    vkb::destroy_device(vkb_device);
}
}  // namespace jengine::renderer::vulkan
