#include "should_be_unreachable.hpp"

#include <iostream>

namespace jengine::util {
void should_be_unreachable(std::string_view error_message) {
    std::cerr << "Unreachable code reached " << error_message << std::endl;
    std::abort();
}
}  // namespace jengine::util
