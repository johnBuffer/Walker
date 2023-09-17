#include "engine/window/window_context_handler.hpp"

#include "user/common/configuration.hpp"

#include "user/training/render/renderer.hpp"
#include "user/training/initialize.hpp"


struct Training
{
    static int main()
    {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;
        pez::render::WindowContextHandler app("Walk - Training", sf::Vector2u(conf::window_width, conf::window_height), settings, sf::Style::Default);
        training::registerSystems();

        pez::render::setFocus(conf::world_size * 0.5f);
        pez::render::setZoom(0.8f);

        app.getEventManager().addKeyPressedCallback(sf::Keyboard::S, [&](sfev::CstEv) {
            app.toggleUnlimitedFramerate();
        });

        constexpr uint32_t fps_cap = 60;
        // Main loop
        const float dt = 1.0f / static_cast<float>(fps_cap);
        while (app.run()) {
            pez::core::update(dt);
            pez::core::render(sf::Color::White);
        }

        return 0;
    }
};
