#pragma once

#include <string_view>

namespace jengine::util {

[[noreturn]] void should_be_unreachable(std::string_view error_message = "");

}
