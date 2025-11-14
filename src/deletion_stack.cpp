#include "deletion_stack.hpp"

namespace jengine {

void DeletionStack::Push(std::function<void()>&& function) {
    stack.push(function);
}
void DeletionStack::Flush() {
    while (!stack.empty()) {
        stack.top()();
        stack.pop();
    }
}
}  // namespace jengine
