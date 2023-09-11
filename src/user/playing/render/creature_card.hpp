#pragma once
#include "user/common/render/card.hpp"

#include "user/playing/simulation/simulation.hpp"

#include "engine/common/racc.hpp"


struct WalkerCard
{
    static float constexpr corner_radius = 20.0f;
    static float constexpr width         = 500.0f;
    static float constexpr outline       = 5.0f;
    static float constexpr width_in      = width - 2.0f * corner_radius;
    static float constexpr width_out     = width + 2.0f * outline;
    static float constexpr height        = 160.0f;
    static float constexpr height_out    = height + 2.0f * outline;

    static uint8_t constexpr dark_alpha_light  = 100;
    static uint8_t constexpr dark_alpha_strong = 150;

    sf::Color    color;
    Card         background;
    Card         progress_back;
    Card         progress_bar;

    Card         outline_base;
    Card         outline_alpha;

    float        progression = 0.0f;
    SmoothVec2   position;
    Vec2         last_position;
    RMean<float> speed;
    uint32_t     frame_id = 0;
    uint32_t     rank = 0;

    sf::Font font;
    sf::Text text;
    sf::Text text_progress;
    sf::Text text_speed;
    sf::Text text_label_speed;

    explicit
    WalkerCard(sf::Color color, Walker const& creature)
        : background{{width, height}, corner_radius, color}
        , progress_back{{width_in, 10.0f}, 5.0f, {0, 0, 0, dark_alpha_light}}
        , progress_bar{{width_in, 10.0f}, 5.0f, {0, 0, 0, dark_alpha_light}}
        , outline_base{{width_out, height_out}, corner_radius + outline, color}
        , outline_alpha{{width_out, height_out}, corner_radius + outline, {0, 0, 0, dark_alpha_light}}
        , speed{100}
    {
        font.loadFromFile("res/font.ttf");
        text.setFont(font);
        text.setString("Creature 1");
        text.setCharacterSize(40);
        text.setFillColor({0, 0, 0, dark_alpha_light});

        text_progress.setFont(font);
        text_progress.setCharacterSize(30);
        text_progress.setFillColor({0, 0, 0, dark_alpha_light});

        text_speed.setFont(font);
        text_speed.setCharacterSize(30);
        text_speed.setFillColor({0, 0, 0, dark_alpha_light});

        text_label_speed.setFont(font);
        text_label_speed.setCharacterSize(20);
        text_label_speed.setFillColor({0, 0, 0, dark_alpha_light});
        text_label_speed.setString("Avg. speed");

        last_position = creature.getHeadPosition();

        position.setInterpolationFunction(Interpolation::EaseInOutQuint);
        position.setSpeed(2.0f);
    }

    /*void update(SimulationPlaying const& simulation, uint32_t target_id, Creature const& creature)
    {
        Vec2 const  last_target    = (target_id > 0) ? simulation.targets[target_id - 1] : conf::world_size * 0.5f;
        float const distance       = MathVec2::length(simulation.targets[target_id] - last_target);
        float const current_dist   = MathVec2::length(simulation.targets[target_id] - creature.getHeadPosition());
        float const target_percent = 1.0f / 30.0f;
        progression = std::min(1.0f, std::max(progression, target_percent * target_id + target_percent * std::max(0.0f, 1.0f - current_dist / distance)));

        if (frame_id % 20 == 0) {
            Vec2 const pos = creature.getHeadPosition();
            Vec2 const move = pos - last_position;
            last_position = pos;
            float const dt = 20.0f / 60.0f;
            speed.addValue(MathVec2::length(move) / dt * 0.1f);
        }
        ++frame_id;
    }*/

    void setPosition(Vec2 pos)
    {
        position = pos;
    }

    void render(pez::render::Context& context)
    {
        Vec2 const pos = position;

        text.setPosition(pos + Vec2{corner_radius * 0.85f, corner_radius * 0.5f});
        background.position    = pos;
        progress_back.position = pos + Vec2{corner_radius, 70.0f};
        progress_bar.position  = pos + Vec2{corner_radius, 70.0f};

        outline_base.position = pos - Vec2{outline, outline};
        outline_base.renderHud(context);

        outline_alpha.position = pos - Vec2{outline, outline};
        outline_alpha.renderHud(context);

        background.renderHud(context);
        progress_back.renderHud(context);
        progress_bar.setWidth(width_in * progression);

        progress_bar.renderHud(context);
        context.drawDirect(text);

        text_progress.setString(toString(int(progression * 100.0f)) + "%");
        text_progress.setPosition(pos.x + width - corner_radius - text_progress.getGlobalBounds().width, pos.y + 80.0f);
        context.drawDirect(text_progress);

        text_label_speed.setPosition(pos.x + corner_radius, pos.y + 90.0f);
        context.drawDirect(text_label_speed);

        text_speed.setString(toString(int(speed.get())) + " cm/s");
        text_speed.setPosition(pos.x + corner_radius, pos.y + 110.0f);
        context.drawDirect(text_speed);
    }
};