#pragma once
#include "engine/engine.hpp"
#include "engine/common/smooth/smooth_value.hpp"

#include "user/common/render/utils.hpp"
#include "user/common/render/walker_drawable.hpp"
#include "user/common/render/network_renderer.hpp"
#include "user/common/render/card.hpp"

#include "user/training/demo.hpp"


namespace training
{

struct Renderer : public pez::core::IRenderer
{
    static constexpr uint32_t circle_pts = 32;

    WalkerDrawable creature_drawables;

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

    WalkerDrawable walker;

    TrainingState& state;

    explicit
    Renderer()
        : shadow_va{sf::PrimitiveType::TriangleFan, circle_pts}
        , background{conf::world_size + Vec2{50.0f, 50.0f}, 25.0f, {50, 50, 50}}
        , network_back({}, 0.0f, sf::Color{50, 50, 50})
        , network_out({}, 0.0f, sf::Color{50, 50, 50})
        , creature_drawables(sf::Color::White)
        , walker{{121, 123, 255}}
        , state{pez::core::getSingleton<TrainingState>()}
    {
        font.loadFromFile("res/font.ttf");
        text.setFont(font);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);

        auto const& demo = pez::core::getProcessor<Demo>();
        walker.initialize(demo.task.walker);

        common::Utils::generateCircle(shadow_va, 80.0f, circle_pts, {0, 0, 0, 0});
        shadow_va[0].color = {0, 0, 0, 200};

        background.position = {-25.0f, -25.0f};
    }

    void render(pez::render::Context& context) override
    {
        background.render(context);

        if (!state.demo) {
            return;
        }

        auto const& demo = pez::core::getProcessor<Demo>();

        float const r{10.0f};
        sf::CircleShape target(r);
        target.setOrigin(r, r);
        target.setPosition(demo.task.getCurrentTarget());
        context.draw(target);

        float const dt = 0.016f;
        walker.update(demo.task.walker, dt);
        walker.render(demo.task.walker, demo.task.getCurrentTarget(), context);

        network_out.renderHud(context);
        network_back.renderHud(context);
        network_renderer.update();
        network_renderer.render(context);

        text.setPosition(card_margin, card_margin);
        text.setString(toString(demo.time));
        context.drawDirect(text);
    }

    void updateNetwork()
    {
        auto const& demo = pez::core::getProcessor<Demo>();
        Vec2 const padding{network_padding, network_padding};
        Vec2 const out = padding + Vec2{network_outline, network_outline};
        network_renderer.initialize(demo.task.network);
        network_renderer.position = Vec2{1600.0f - network_renderer.size.x - out.x - card_margin, card_margin + out.y};
        network_back = Card{network_renderer.size + 2.0f * padding, 20.0f, {50, 50, 50}};
        network_out  = Card{network_renderer.size + 2.0f * out, 20.0f + network_outline, walker.base_color};
        network_back.position = network_renderer.position - padding;
        network_out.position  = network_renderer.position - out;
    }
};

}
