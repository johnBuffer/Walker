#pragma once
#include "engine/window_context_handler.hpp"

#include "engine/common/smooth/smooth_value.hpp"

#include "user/playing/sand/physics.hpp"
#include "user/playing/simulation/simulation.hpp"
#include "user/playing/render/renderer.hpp"


struct Playing
{
    static int main()
    {
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;
        WindowContextHandler app("Walk", sf::Vector2u(conf::window_width, conf::window_height), settings, sf::Style::Default);
        // Initialize solver and renderer
        bool emit = true;
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::Space, [&](sfev::CstEv) {
            emit = !emit;
        });

        SimulationPlaying simulation;
        play::Renderer    renderer{simulation};
        SmoothVec2  focus;
        SmoothFloat zoom;
        focus.setInterpolationFunction(Interpolation::Linear);
        focus.setSpeed(1.5f);
        focus.setValueInstant(conf::world_size * 0.5f);

        zoom.setValueInstant(conf::window_height / conf::maximum_distance * 0.95f);

        float const creature_zoom = 2.5f;
        int32_t focus_creature = -1;
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::A, [&](sfev::CstEv){ focus_creature = 0; zoom = creature_zoom; renderer.setNetwork(0);});
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::Z, [&](sfev::CstEv){ focus_creature = 1; zoom = creature_zoom; renderer.setNetwork(1);});
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::E, [&](sfev::CstEv){ focus_creature = 2; zoom = creature_zoom; renderer.setNetwork(2);});
        app.getEventManager().addKeyPressedCallback(sf::Keyboard::R, [&](sfev::CstEv){ focus_creature = 3; zoom = creature_zoom; renderer.setNetwork(3);});

        app.getEventManager().addKeyPressedCallback(sf::Keyboard::C, [&](sfev::CstEv){
            focus_creature = -1;
            focus = conf::world_size * 0.5f;
            zoom  = conf::window_height / conf::maximum_distance * 0.95f;
            app.getRenderContext().setFocus(conf::world_size * 0.5f);
            app.getRenderContext().setZoom(conf::window_height / conf::maximum_distance * 0.95f);
        });

        //app.getRenderContext().setZoom(3.0f);

        constexpr uint32_t fps_cap = 60;

        // Main loop
        const float dt = 1.0f / static_cast<float>(fps_cap);
        while (app.run()) {

            //app.getRenderContext().setFocus(focus);
            //app.getRenderContext().setZoom(zoom);

            Timer::update(dt);
            simulation.update(dt);

            if (focus_creature != -1) {
                focus = simulation.creatures[focus_creature].getHeadPosition();
            }

            RenderContext& render_context = app.getRenderContext();
            render_context.clear({80, 80, 80});
            renderer.render(render_context, dt);
            render_context.display();
        }

        return 0;
    }
};
