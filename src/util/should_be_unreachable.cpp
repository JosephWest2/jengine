#include "should_be_unreachable.hpp"

#include <print>

namespace jengine::util {
void should_be_unreachable(std::string_view error_message) {
    std::println(stderr, "Unreachable code reached {}", error_message);
    std::abort();
}
}  // namespace jengine::util
