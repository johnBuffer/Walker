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
        pez::render::WindowContextHandler app("Walk - Training", sf::Vector2u(conf::win::window_width, conf::win::window_height), settings, sf::Style::Default);
        training::registerSystems();

        pez::render::setFocus(conf::world_size * 0.5f);
        pez::render::setZoom(0.8f);

        app.getEventManager().addKeyPressedCallback(sf::Keyboard::S, [&](sfev::CstEv) {
            app.toggleUnlimitedFramerate();
        });

        RMean<float> update_time(100);

        constexpr uint32_t fps_cap = 60;
        // Main loop
        const float dt = 1.0f / static_cast<float>(fps_cap);
        while (app.run()) {
            sf::Clock clock;
            pez::core::update(dt);
            auto const ms = clock.getElapsedTime().asMilliseconds();
            update_time.addValue(static_cast<float>(ms));
            //std::cout << "Update avg. time: " << update_time.get() << std::endl;

            pez::core::render({80, 80, 80});
        }

        return 0;
    }
};
