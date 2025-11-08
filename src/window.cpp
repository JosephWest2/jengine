#include "window.hpp"

#include <format>
#include <stdexcept>
#include <unordered_map>

#include "SDL3/SDL_video.h"
namespace jengine {

std::unordered_map<WindowType, SDL_WindowFlags> window_flags = {
    {WindowType::VULKAN, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN}};

Window::Window(const char* title, int width, int height, WindowType window_type, bool constant_redraw)
    : constant_redraw(constant_redraw) {
    if (!(window = SDL_CreateWindow(title, width, height, window_flags[window_type]))) {
        throw std::runtime_error(std::format("SDL_CreateWindow failed: {}", SDL_GetError()));
    }
}

bool Window::ConstantRedraw() const { return constant_redraw; }

SDL_Window* Window::SDL() const { return window; };

Window::~Window() {
    if (window) {
        SDL_DestroyWindow(window);
    }
}

Window::Window(Window&& other) : renderer(other.renderer), window(other.window), constant_redraw(other.constant_redraw) {
    other.window = nullptr;
    other.renderer = nullptr;
}

Window& Window::operator=(Window&& other) {
    if (this != &other) {
        window = other.window;
        constant_redraw = other.constant_redraw;
        renderer = other.renderer;
        other.window = nullptr;
        other.renderer = nullptr;
    }
    return *this;
}
void Window::DrawFrame() {
    if (!renderer) {
        throw std::runtime_error("Renderer not set");
    }
    renderer->DrawFrame();
}
void Window::SetRenderer(renderer::Base* renderer) { this->renderer = renderer; }
}  // namespace jengine
