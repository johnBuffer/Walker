#pragma once
#include "engine/window/window_context_handler.hpp"
#include "user/playing/simulation/simulation.hpp"
#include "engine/common/smooth/smooth_value.hpp"

#include "user/common/render/utils.hpp"
#include "user/common/render/walker_drawable.hpp"
#include "./target.hpp"
#include "user/common/render/network_renderer.hpp"
#include "./walker_card.hpp"


namespace playing
{
struct Renderer
{
    static constexpr uint32_t circle_pts = 32;

    Simulation&     simulation;
    tp::ThreadPool& thread_pool;

    std::vector<WalkerDrawable> walker_drawables;

    NetworkRenderer network_renderer;

    sf::VertexArray objects_va;
    sf::VertexArray hud_va;
    sf::Texture     object_texture;
    sf::Font        font;
    sf::Text        text;

    sf::VertexArray shadow_va;

    std::vector<WalkerCard> cards;
    Card                    background;
    Card                    network_back;
    Card                    network_out;

    float const card_margin     = 20.0f;
    float const network_padding = 20.0f;
    float const network_outline = 10.0f;

    explicit
    Renderer(Simulation& simulation_)
        : simulation{simulation_}
        , thread_pool{pez::core::getSingleton<tp::ThreadPool>()}
        , objects_va{sf::PrimitiveType::Quads}
        , shadow_va{sf::PrimitiveType::TriangleFan, circle_pts}
        , background{conf::world_size + Vec2{50.0f, 50.0f}, 25.0f, {50, 50, 50}}
        , hud_va{sf::PrimitiveType::Quads, 4}
        , network_back({}, 0.0f, sf::Color{50, 50, 50})
        , network_out({}, 0.0f, sf::Color{50, 50, 50})
    {
        object_texture.loadFromFile("res/circle.png");

        font.loadFromFile("res/font.ttf");
        text.setFont(font);
        text.setCharacterSize(200);
        text.setFillColor(sf::Color::Black);

        for (auto const& t : simulation.tasks) {
            walker_drawables.emplace_back(t.color);
            walker_drawables.back().initialize(simulation.walkers[t.walker_idx]);
            walker_drawables.back().render_target.color = t.color;
        }

        common::Utils::generateCircle(shadow_va, 80.0f, circle_pts, {0, 0, 0, 0});
        shadow_va[0].color = {0, 0, 0, 200};

        uint32_t i{0};
        cards.reserve(simulation.walkers.size());
        for (auto const& t : simulation.tasks) {
            cards.emplace_back(t.color, simulation.walkers[t.walker_idx]);
            cards.back().rank = i;
            cards.back().setPosition({card_margin, card_margin + i * (WalkerCard::height + card_margin)});
            cards.back().text.setString(t.name);
            ++i;
        }

        background.position = {-25.0f, -25.0f};

        hud_va[0].position = {0.0f  , 0.0f};
        hud_va[0].color = {0, 0, 0, 150};
        hud_va[1].position = {300.0f, 0.0f};
        hud_va[1].color = {0, 0, 0, 0};
        hud_va[2].position = {300.0f, 2000.0f};
        hud_va[2].color = {0, 0, 0, 0};
        hud_va[3].position = {0.0f  , 2000.0f};
        hud_va[3].color = {0, 0, 0, 150};
    }

    void render(pez::render::Context& context, float dt)
    {
        background.render(context);

        float const physic_scale = conf::maximum_distance / static_cast<float>(simulation.solver.grid.width);
        sf::RenderStates states;
        states.texture = &object_texture;
        states.transform.scale(physic_scale, physic_scale);
        updateParticlesVA();
        context.draw(objects_va, states);

        {
            std::vector<WalkTask const*> sorted_tasks;
            sorted_tasks.reserve(simulation.tasks.size());
            for (auto const& t : simulation.tasks) {
                sorted_tasks.push_back(&t);
            }
            std::sort(sorted_tasks.begin(), sorted_tasks.end(), [](WalkTask const* t1, WalkTask const* t2) {return t1->rank <  t2->rank;});

            for (auto const* t : sorted_tasks) {
                auto const target = simulation.targets[t->target_idx];
                walker_drawables[t->walker_idx].renderTarget(target, context);
                text.setString(toString(t->target_idx + 0));
                auto const size = text.getGlobalBounds().getSize();

                if (t->target_idx > 9) {
                    text.setOrigin(size.x * 4.0f, size.y * 6.0f);
                } else {
                    text.setOrigin(size.x * 5.0f, size.y * 6.0f);
                }

                text.setPosition(target);
                auto const t_color = t->color;
                text.setFillColor(sf::Color(0, 0, 0, 150));
                text.setScale(0.15f, 0.15f);

                context.draw(text);
            }
        }

        {
            uint32_t i{0};
            for (auto const& t : simulation.tasks) {
                sf::Transform transform;
                transform.translate(simulation.walkers[t.walker_idx].getJoint(4).position);
                context.draw(shadow_va, transform);
                walker_drawables[i].update(simulation.walkers[t.walker_idx], dt);
                walker_drawables[i].render(simulation.walkers[t.walker_idx], simulation.targets[t.target_idx], context);
                ++i;
            }
        }

        if (network_renderer.network) {
            network_out.renderHud(context);
            network_back.renderHud(context);
            network_renderer.update();
            network_renderer.render(context);
        }

        //context.drawDirect(hud_va);

        std::vector<WalkerCard*> sorted_cards;
        for (auto& c : cards) { sorted_cards.push_back(&c); }
        std::sort(sorted_cards.begin(), sorted_cards.end(), [](auto const* t1, auto const* t2) {return t1->progression > t2->progression;});

        {
            uint32_t i{0};
            for (auto const& t: simulation.tasks) {
                cards[i].render(context);
                ++i;
            }
        }

        {
            uint32_t i{0};
            for (auto* c : sorted_cards) {
                if (i != c->rank && c->position.isDone()) {
                    c->setPosition({card_margin, card_margin + i * (WalkerCard::height + card_margin)});
                    c->rank = i;
                }
                ++i;
            }
        }
    }

    void updateParticlesVA()
    {
        auto const& solver = simulation.solver;
        objects_va.resize(solver.objects.size() * 4);

        const float texture_size = 1024.0f;
        thread_pool.dispatch(to<uint32_t>(solver.objects.size()), [&](uint32_t start, uint32_t end) {
            for (uint32_t i{start}; i < end; ++i) {
                const PhysicObject& object = solver.objects.getData()[i];
                float const radius = object.radius;
                const uint32_t idx = i << 2;
                objects_va[idx + 0].position = object.position + Vec2{-radius, -radius};
                objects_va[idx + 1].position = object.position + Vec2{ radius, -radius};
                objects_va[idx + 2].position = object.position + Vec2{ radius,  radius};
                objects_va[idx + 3].position = object.position + Vec2{-radius,  radius};
                objects_va[idx + 0].texCoords = {0.0f        , 0.0f};
                objects_va[idx + 1].texCoords = {texture_size, 0.0f};
                objects_va[idx + 2].texCoords = {texture_size, texture_size};
                objects_va[idx + 3].texCoords = {0.0f        , texture_size};

                const sf::Color color(static_cast<uint8_t>(static_cast<float>(object.color.r) * object.current_ratio),
                                      static_cast<uint8_t>(static_cast<float>(object.color.g) * object.current_ratio),
                                      static_cast<uint8_t>(static_cast<float>(object.color.b) * object.current_ratio));

                objects_va[idx + 0].color = color;
                objects_va[idx + 1].color = color;
                objects_va[idx + 2].color = color;
                objects_va[idx + 3].color = color;
            }
        });
    }

    void setNetwork(uint32_t i)
    {
        auto const& t = simulation.tasks[i];
        Vec2 const padding{network_padding, network_padding};
        Vec2 const out = padding + Vec2{network_outline, network_outline};
        network_renderer.initialize(t.network);
        network_renderer.position = Vec2{conf::win::window_width - network_renderer.size.x - out.x - card_margin, card_margin + out.y};
        network_back = Card{network_renderer.size + 2.0f * padding, 20.0f, {50, 50, 50}};
        network_out  = Card{network_renderer.size + 2.0f * out, 20.0f + network_outline, t.color};
        network_back.position = network_renderer.position - padding;
        network_out.position = network_renderer.position - out;
    }
};
}
