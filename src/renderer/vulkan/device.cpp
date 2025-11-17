#include "device.hpp"

#include <format>
#include <iostream>

namespace jengine::renderer::vulkan {

Device::Device(vkb::PhysicalDevice& physical_device) {
    vkb::DeviceBuilder device_builder(physical_device);

    auto device_res = device_builder.build();
    if (!device_res.has_value()) {
        throw std::runtime_error(std::format("Failed to create Vulkan device: {}", device_res.error().message()));
    }

    vkb_device = device_res.value();
}

Device::~Device() {
    std::cout << "Destroying Vulkan device" << std::endl;
    vkb::destroy_device(vkb_device);
}

}  // namespace jengine::renderer::vulkan
