#pragma once
#include "engine/engine.hpp"
#include "engine/common/smooth/smooth_value.hpp"

#include "user/common/render/utils.hpp"
#include "user/common/walker_drawable.hpp"
#include "user/common/network_renderer.hpp"
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
        , background{conf::world_size, conf::maximum_distance, {200, 200, 200}}
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

        Utils::generateCircle(shadow_va, 80.0f, circle_pts, {0, 0, 0, 0});
        shadow_va[0].color = {0, 0, 0, 200};
    }

    void render(pez::render::Context& context) override
    {
        background.render(context);

        float const r{2.0f};
        sf::CircleShape target{r};
        target.setOrigin(r, r);
        target.setFillColor(sf::Color::Red);

        auto& sequence = pez::core::get<TargetSequence>(1);
        for (auto const& v : sequence.targets) {
            target.setPosition(v);
            context.draw(target);
        }
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
