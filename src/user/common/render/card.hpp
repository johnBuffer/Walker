#pragma once
#include <SFML/Graphics.hpp>
#include "engine/common/vec.hpp"
#include "engine/common/math.hpp"
#include "engine/render/render_context.hpp"


struct Card
{
    // Struct to share configuration between create and update functions
    struct Conf
    {
        uint32_t quality = 64;
        float    da      = Math::PI * 0.5f / static_cast<float>(quality - 1);
    };

    sf::VertexArray va;
    Vec2            position;
    Vec2            size;
    float           corner_radius;

    Card(Vec2 size_, float corner_radius_, sf::Color color)
        : va{sf::PrimitiveType::TriangleFan}
        , size{size_}
        , corner_radius{corner_radius_}
    {
        updateCard();
        setColor(color);
    }

    void setColor(sf::Color color)
    {
        uint64_t const vertex_count = va.getVertexCount();
        for (uint64_t i{vertex_count}; i--;) {
            va[i].color = color;
        }
    }

    void updateCard()
    {
        const Conf conf;

        va.resize(4 * conf.quality + 2);
        uint32_t global_index = 0;

        const float radius = std::min(std::min(corner_radius, size.x * 0.5f), size.y * 0.5f);

        va[global_index++].position = size * 0.5f;

        // Top left
        {
            const Vec2 center = Vec2{radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::PI + fi * conf.da;
                va[global_index++].position = center + radius * Vec2{cos(angle), sin(angle)};
            }
        }
        // Top right
        {
            const Vec2 center = Vec2{size.x - radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::PI * 1.5f + fi * conf.da;
                //va[global_index++].position = size * 0.5f;
                va[global_index++].position = center + radius * Vec2{cos(angle), sin(angle)};
            }
        }
        // Bottom right
        {
            const Vec2 center = Vec2{size.x, size.y} - Vec2{radius, radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::PI * 2.0f + fi * conf.da;
                //va[global_index++].position = size * 0.5f;
                va[global_index++].position = center + radius * Vec2{cos(angle), sin(angle)};
            }
        }
        // Bottom left
        {
            const Vec2 center = Vec2{radius, size.y - radius};
            for (uint32_t i(0); i < conf.quality; ++i) {
                const auto fi = static_cast<float>(i);
                const float angle = Math::PI * 2.5f + fi * conf.da;
                //va[global_index++].position = size * 0.5f;
                va[global_index++].position = center + radius * Vec2{cos(angle), sin(angle)};
            }
        }

        va[global_index++].position = Vec2{0.0f, radius};
    }

    void renderHud(pez::render::Context& context)
    {
        sf::Transform transform;
        transform.translate(position);
        context.drawDirect(va, transform);
    }

    void render(pez::render::Context& context)
    {
        sf::Transform transform;
        transform.translate(position);
        context.draw(va, transform);
    }

    void setWidth(float width)
    {
        size.x = width;
        updateCard();
    }
};
