#pragma once

#include <functional>
#include <stack>
namespace jengine {
    class DeletionStack {
        public:
          void Push(std::function<void()>&& function);
          void Flush();
        private:
            std::stack<std::function<void()>> stack;
    };
}
