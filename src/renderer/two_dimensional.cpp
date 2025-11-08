#include "two_dimensional.hpp"

#include <format>
#include <stdexcept>

namespace jengine::renderer {

TwoDimensional::TwoDimensional(SDL_Window* window) {
    if (!(renderer = SDL_CreateRenderer(window, nullptr))) {
        throw std::runtime_error(std::format("SDL_CreateRenderer failed: {}", SDL_GetError()));
    }
}

}  // namespace jengine::renderer
