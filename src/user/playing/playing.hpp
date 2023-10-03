#include "engine/window/window_context_handler.hpp"

#include "user/playing/render/renderer.hpp"
#include "engine/common/smooth/smooth_value.hpp"

#include "user/playing/sand/physics.hpp"
#include "user/playing/initialize.hpp"


struct Playing
{
    static int main()
    {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;
        pez::render::WindowContextHandler app("Walk - Replay", sf::Vector2u(conf::win::window_width, conf::win::window_height), settings, sf::Style::Default);
        playing::registerSystems();
        // Initialize solver and renderer
        bool emit = true;
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::Space, [&](sfev::CstEv) {
            emit = !emit;
        });

        auto& simulation = pez::core::getProcessor<playing::Simulation>();
        playing::Renderer renderer{simulation};
        SmoothVec2  focus;
        SmoothFloat zoom;
        focus.setInterpolationFunction(Interpolation::Linear);
        focus.setSpeed(1.5f);
        focus.setValueInstant(conf::world_size * 0.5f);
        renderer.setNetwork(0);

        zoom.setValueInstant(conf::win::window_height / conf::maximum_distance * 0.95f);

        float const creature_zoom = 2.5f;
        int32_t focus_creature = -1;
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::A, [&](sfev::CstEv){ focus_creature = 0; zoom = creature_zoom; renderer.setNetwork(0);});
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::Z, [&](sfev::CstEv){ focus_creature = 1; zoom = creature_zoom; renderer.setNetwork(1);});
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::E, [&](sfev::CstEv){ focus_creature = 2; zoom = creature_zoom; renderer.setNetwork(2);});
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::R, [&](sfev::CstEv){ focus_creature = 3; zoom = creature_zoom; renderer.setNetwork(3);});
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::C, [&](sfev::CstEv){
            focus_creature = -1;
            focus = conf::world_size * 0.5f;
            zoom  = conf::win::window_height / conf::maximum_distance * 0.95f;
            pez::render::setFocus(conf::world_size * 0.5f);
            pez::render::setZoom(conf::win::window_height / conf::maximum_distance * 0.9f);
        });

        pez::render::setFocus(conf::world_size * 0.5f);
        pez::render::setZoom(conf::win::window_height / conf::maximum_distance * 0.9f);

        constexpr uint32_t fps_cap = 60;

        // Main loop
        const float dt = 1.0f / static_cast<float>(fps_cap);
        while (app.run()) {
            pez::core::update(dt);

            if (focus_creature != -1) {
                focus = simulation.walkers[focus_creature].getHeadPosition();
                pez::render::setFocus(focus);
            }

            pez::render::Context& render_context = app.getRenderContext();
            render_context.clear({80, 80, 80});
            renderer.render(render_context, dt);
            render_context.display();
        }

        return 0;
    }
};
