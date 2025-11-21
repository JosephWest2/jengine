#include "physical_device.hpp"

#include <map>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {

PhysicalDevice::PhysicalDevice(vk::raii::Instance& instance) : physical_device(CreatePhysicalDevice(instance)) {}

vk::raii::PhysicalDevice PhysicalDevice::CreatePhysicalDevice(vk::raii::Instance& instance) {
    std::multimap<int, vk::raii::PhysicalDevice> device_scores;
    for (const auto& physical_device : instance.enumeratePhysicalDevices()) {
        int score = 0;
        if (physical_device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            score += 1000;
        }
        score += physical_device.getProperties().limits.maxImageDimension2D;
        if (!physical_device.getFeatures().geometryShader) {
            score = std::numeric_limits<int>::min();
        }
        device_scores.insert({score, physical_device});
    }
    return device_scores.rbegin()->second;
}
std::optional<uint32_t> PhysicalDevice::GetPhysicalDeviceQueueFamilyIndex(vk::raii::PhysicalDevice& physical_device,
                                                                          vk::QueueFlagBits queue_flags) {
    const auto queues = physical_device.getQueueFamilyProperties();
    for (size_t i = 0; i < queues.size(); i++) {
        if (queues[i].queueFlags & queue_flags) {
            return static_cast<uint32_t>(i);
        }
    }
    return std::nullopt;
}
uint32_t PhysicalDevice::GetQueueFamilyIndex(vk::QueueFlagBits queue_flags) {
    if (!index_cache.contains(queue_flags)) {
        auto index = GetPhysicalDeviceQueueFamilyIndex(physical_device, queue_flags);
        if (!index.has_value()) {
            throw std::runtime_error("Failed to get queue family index");
        }
        index_cache[queue_flags] = index.value();
    }
    return index_cache[queue_flags];
}
uint32_t PhysicalDevice::GetPresentQueueIndex(const vk::SurfaceKHR& surface) {
    if (present_index_cache.contains(surface)) {
        return present_index_cache[surface];
    }
    const auto queues = physical_device.getQueueFamilyProperties();
    for (size_t i = 0; i < queues.size(); i++) {
        if (physical_device.getSurfaceSupportKHR(i, surface)) {
            present_index_cache[surface] = static_cast<uint32_t>(i);
            return static_cast<uint32_t>(i);
        }
    }
    throw std::runtime_error("Failed to get present queue index for surface");
}
}  // namespace jengine::renderer::vulkan
