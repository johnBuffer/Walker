#pragma once
#include "game/simulation/simulation.hpp"
#include "engine/render/text/text.hpp"


struct CreatureRenderer : public pez::core::IRenderer
{
    VertexArray objects_va;
    VertexArray links_va;
    VertexArray target_va;
    VertexArray arena_va;

    Texture     object_texture;
    Text        time_text;

    constexpr static int32_t circle_quality = 128;

    CreatureRenderer()
        : objects_va{GeometryType::Quads}
        , links_va{GeometryType::Quads}
        , target_va{GeometryType::Quads, 4}
        , arena_va{GeometryType::LinesStrip, 128}
        , object_texture{pez::resources::getTexture("circle")}
    {
        time_text.setFont(pez::resources::getFont("font"));

        float const da = Math::TwoPI / static_cast<float>(circle_quality - 1);
        for (int32_t i{0}; i < circle_quality; ++i) {
            float const angle = i * da;
            arena_va[i].position = conf::maximum_distance * Vec2{ cos(angle) + 0.5f, sin(angle) + 0.5f };
        }
        arena_va.updateGeometry();
    }

    void render(pez::render::Context& context) override
    {
        context.render(arena_va, Mat4(1.0f), pez::render::Context::Mode::Normal);

        const auto& simulation = pez::core::getProcessor<Simulation>();
        Creature const& creature = simulation.demo_walk.creature;
        auto const& objects = creature.system.objects;
        auto const& links   = creature.system.links;

        time_text.setString(toString(simulation.demo_walk.time));
        context.renderToHUD(time_text, pez::render::Context::Mode::Normal);

        // Render links
        links_va.resize(4 * links.size() + 4);
        constexpr float width = 4.0f;
        {
            uint32_t i{0};
            for (auto const& o: links) {
                 const auto pos_1 = objects[o.idx_1].position;
                 const auto pos_2 = objects[o.idx_2].position;
                 const auto v     = MathVec2::normalize(pos_1 - pos_2);
                 const auto n     = MathVec2::normal(v);

                links_va[4 * i + 0].position = pos_1 + n * 0.5f * width;
                links_va[4 * i + 1].position = pos_2 + n * 0.5f * width;
                links_va[4 * i + 2].position = pos_2 - n * 0.5f * width;
                links_va[4 * i + 3].position = pos_1 - n * 0.5f * width;

                Color const color = o.is_muscle ? Color::Red : Color::Yellow;

                links_va[4 * i + 0].color = color;
                links_va[4 * i + 1].color = color;
                links_va[4 * i + 2].color = color;
                links_va[4 * i + 3].color = color;

                ++i;
            }

            const Vec2  head_position = creature.getHeadPosition();
            const Vec2  direction     = creature.getHeadDirection();
            const Vec2  normal        = MathVec2::normal(direction);
            const float length        = 50.0f;

            links_va[4 * i + 0].position = head_position + normal * width;
            links_va[4 * i + 1].position = head_position + direction * length + normal * width;
            links_va[4 * i + 2].position = head_position + direction * length - normal * width;
            links_va[4 * i + 3].position = head_position - normal * width;

            links_va[4 * i + 0].color = Color::Blue;
            links_va[4 * i + 1].color = Color::Blue;
            links_va[4 * i + 2].color = Color::Blue;
            links_va[4 * i + 3].color = Color::Blue;
        }
        links_va.updateGeometry();
        context.render(links_va, Mat4(1.0f), pez::render::Context::Mode::Normal);

        // Render bodies
        objects_va.resize(4 * objects.size());
        {
            uint32_t i{0};
            for (auto const& o: objects) {
                const float radius = 10.0f + 10.0f * o.friction;
                objects_va[4 * i + 0].position = {o.position.x - radius, o.position.y - radius};
                objects_va[4 * i + 1].position = {o.position.x + radius, o.position.y - radius};
                objects_va[4 * i + 2].position = {o.position.x + radius, o.position.y + radius};
                objects_va[4 * i + 3].position = {o.position.x - radius, o.position.y + radius};

                objects_va[4 * i + 0].tex_coords = {0.0f, 0.0f};
                objects_va[4 * i + 1].tex_coords = {1.0f, 0.0f};
                objects_va[4 * i + 2].tex_coords = {1.0f, 1.0f};
                objects_va[4 * i + 3].tex_coords = {0.0f, 1.0f};
                ++i;
            }
        }
        objects_va.updateGeometry();
        context.render(objects_va, object_texture.handle, Mat4(1.0f), pez::render::Context::Mode::Normal);

        // RenderTarget
        const float target_radius = 20.0f;
        const auto target_pos = simulation.demo_walk.targets[simulation.demo_walk.current_target];
        target_va[0].position = {target_pos.x - target_radius, target_pos.y - target_radius};
        target_va[1].position = {target_pos.x + target_radius, target_pos.y - target_radius};
        target_va[2].position = {target_pos.x + target_radius, target_pos.y + target_radius};
        target_va[3].position = {target_pos.x - target_radius, target_pos.y + target_radius};
        target_va[0].tex_coords = {0.0f, 0.0f};
        target_va[1].tex_coords = {1.0f, 0.0f};
        target_va[2].tex_coords = {1.0f, 1.0f};
        target_va[3].tex_coords = {0.0f, 1.0f};
        target_va.updateGeometry();
        context.render(target_va, object_texture.handle, Mat4(1.0f), pez::render::Context::Mode::Normal);
    }
};