#pragma once
#include "engine/window/window_context_handler.hpp"
#include "engine/common/smooth/smooth_value.hpp"

#include "user/common/render/utils.hpp"
#include "user/common/creature_drawable.hpp"
#include "user/common/network_renderer.hpp"
#include "user/common/render/card.hpp"


namespace training
{
struct Renderer
{
    static constexpr uint32_t circle_pts = 32;

    CreatureDrawable creature_drawables;

    NetworkRenderer network_renderer;

    sf::Font        font;
    sf::Text        text;

    sf::VertexArray shadow_va;

    Card background;
    Card network_back;
    Card network_out;

    float const card_margin     = 20.0f;
    float const network_padding = 20.0f;
    float const network_outline = 10.0f;

    explicit
    Renderer()
        : shadow_va{sf::PrimitiveType::TriangleFan, circle_pts}
        , background{conf::world_size + Vec2{50.0f, 50.0f}, 25.0f, {50, 50, 50}}
        , network_back({}, 0.0f, sf::Color{50, 50, 50})
        , network_out({}, 0.0f, sf::Color{50, 50, 50})
        , creature_drawables(sf::Color::White)
    {
        font.loadFromFile("res/font.ttf");
        text.setFont(font);
        text.setCharacterSize(200);
        text.setFillColor(sf::Color::Black);

        /*for (auto const& t : simulation.tasks) {
            Vec2 const padding{network_padding, network_padding};
            Vec2 const out = padding + Vec2{network_outline, network_outline};
            network_renderer.initialize(t.genome->network);
            network_renderer.position = Vec2{1600.0f - network_renderer.size.x - out.x - card_margin, card_margin + out.y};
            network_back = Card{network_renderer.size + 2.0f * padding, 20.0f, {50, 50, 50}};
            network_out  = Card{network_renderer.size + 2.0f * out, 20.0f + network_outline, sf::Color::White};
            network_back.position = network_renderer.position - padding;
            network_out.position = network_renderer.position - out;
        }*/

        Utils::generateCircle(shadow_va, 80.0f, circle_pts, {0, 0, 0, 0});
        shadow_va[0].color = {0, 0, 0, 200};

        background.position = {-25.0f, -25.0f};
    }

    void render(pez::render::Context& context, float dt) {
        background.render(context);

    }

    void setNetwork(uint32_t i)
    {
        /*auto const& t = simulation.tasks[i];
        Vec2 const padding{network_padding, network_padding};
        Vec2 const out = padding + Vec2{network_outline, network_outline};
        network_renderer.initialize(t.genome->network);
        network_renderer.position = Vec2{2560.0f - network_renderer.size.x - out.x - card_margin, card_margin + out.y};
        network_back = Card{network_renderer.size + 2.0f * padding, 20.0f, {50, 50, 50}};
        network_out  = Card{network_renderer.size + 2.0f * out, 20.0f + network_outline, sf::Color::White};
        network_back.position = network_renderer.position - padding;
        network_out.position = network_renderer.position - out;*/
    }
};
}
