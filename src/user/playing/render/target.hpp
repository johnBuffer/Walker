#pragma once

struct Target
{
    static constexpr float max_radius = 25.0f;
    static constexpr float min_radius = 20.0f;
    static constexpr float wave_scale = 1.5f;

    sf::Vector2f position;
    bool inflate = true;

    float radius      = min_radius;
    float wave_radius = min_radius;

    sf::Color color = sf::Color::Yellow;

    void update(float dt)
    {
        float const inflate_margin = 2.0f;
        float const deflate_margin = 0.01f;
        float const inflate_speed  = 10.0f;
        float const deflate_speed  = 3.0f;

        if (inflate) {
            if (radius < max_radius - inflate_margin) {
                radius +=  inflate_speed * dt;
            } else {
                inflate = false;
            }
        } else {
            if (radius > min_radius + deflate_margin) {
                radius += (min_radius - radius) * deflate_speed * dt;

                wave_radius += (wave_scale * max_radius - wave_radius) * deflate_speed * 2.0f * dt;
            } else {
                inflate = true;
                wave_radius = min_radius;
            }
        }
    }

    void render(pez::render::Context& context) const
    {
        sf::CircleShape wave(wave_radius);
        wave.setOrigin(wave_radius, wave_radius);
        wave.setPosition(position);
        float const ratio = (wave_radius / wave_scale - min_radius) / (max_radius - min_radius);
        auto const c = static_cast<uint8_t>(150.0f * (1.0f - ratio));
        wave.setFillColor({255, 255, 255, c});
        context.draw(wave);

        sf::CircleShape shape(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
        shape.setFillColor(color);
        context.draw(shape);
    }
};