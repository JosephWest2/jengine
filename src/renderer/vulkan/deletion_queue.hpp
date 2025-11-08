#pragma once

#include <typeindex>
#include <utility>
#include <vector>
namespace jengine::renderer::vulkan {
    class DeletionQueue {
        public:
            // delete all objects in the queue
          void Flush();

          void Push(std::pair<void*, std::type_index> deletion) { queue.push_back(deletion); }

        private:

            std::vector<std::pair<void*, std::type_index>> queue{};

        
    };

}
