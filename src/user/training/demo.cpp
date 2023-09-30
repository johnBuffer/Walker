#include "demo.hpp"
#include "user/training/render/renderer.hpp"


void training::Demo::initialize()
{
    std::cout << "Initializing demo" << std::endl;
    time = 0.0f;
    task.initialize();
    need_init = false;

    auto& renderer = pez::core::getRenderer<Renderer>();
    renderer.updateNetwork();
}
