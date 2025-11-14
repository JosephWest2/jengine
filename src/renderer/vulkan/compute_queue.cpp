#include "compute_queue.hpp"

#include <format>

namespace jengine::renderer::vulkan {
ComputeQueue::ComputeQueue(vkb::Device& device) {
    auto queue_res = device.get_queue(vkb::QueueType::compute);
    if (!queue_res.has_value()) {
        throw std::runtime_error(std::format("Failed to get graphics queue: {}", queue_res.error().message()));
    }
    queue = queue_res.value();

    auto queue_family_index_res = device.get_queue_index(vkb::QueueType::compute);
    if (!queue_family_index_res.has_value()) {
        throw std::runtime_error(
            std::format("Failed to get graphics queue family: {}", queue_family_index_res.error().message()));
    }
    queue_family_index = queue_family_index_res.value();
}

}
