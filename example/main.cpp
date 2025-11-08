#include "engine.hpp"
#include "renderer/three_dimensional.hpp"

int main() {
    jengine::Engine engine;
    jengine::Window window("Hello World", 800, 600, jengine::WindowType::VULKAN, true);
    jengine::renderer::vulkan::Instance instance("jengine", true);
    jengine::renderer::ThreeDimensional renderer(window.SDL(), instance);
    window.SetRenderer(&renderer);
    engine.AddWindow(std::move(window));
    engine.Run();
    return 0;
}
