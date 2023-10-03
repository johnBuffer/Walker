#pragma once
#include "engine/common/color_utils.hpp"

#include "user/common/render/utils.hpp"

#include "user/playing/render/target.hpp"
#include "user/common/walker.hpp"


struct WalkerDrawable
{
    static constexpr uint32_t bezier_pts = 32;

    sf::VertexArray link_va;
    sf::VertexArray side_1_va;
    sf::VertexArray side_2_va;
    sf::VertexArray side_3_va;
    sf::VertexArray side_4_va;

    sf::VertexArray muscle_1_va;
    sf::VertexArray muscle_2_va;

    std::vector<float>              pods_cooldown;
    std::vector<SmoothValue<float>> pods_radius;

    SmoothVec2 eye_right_position;
    SmoothVec2 eye_left_position;
    SmoothVec2 pupil_right_position;
    SmoothVec2 pupil_left_position;

    Vec3      base_color_vec = {100, 100, 155};
    sf::Color base_color     = {100, 100, 155};
    uint8_t   dark_tone      = 50;

    Target render_target;

    explicit
    WalkerDrawable(sf::Color color)
        : link_va(sf::PrimitiveType::Quads)
        , side_1_va(sf::PrimitiveType::TriangleFan, bezier_pts)
        , side_2_va(sf::PrimitiveType::TriangleFan, bezier_pts)
        , side_3_va(sf::PrimitiveType::TriangleFan, bezier_pts)
        , side_4_va(sf::PrimitiveType::TriangleFan, bezier_pts)
        , muscle_1_va(sf::PrimitiveType::TriangleFan, bezier_pts)
        , muscle_2_va(sf::PrimitiveType::TriangleFan, bezier_pts)
        , base_color{color}
    {
        const float pupil_speed = 5.0f;
        pupil_right_position.setInterpolationFunction(Interpolation::Linear);
        pupil_right_position.setSpeed(pupil_speed);
        pupil_left_position.setInterpolationFunction(Interpolation::Linear);
        pupil_left_position.setSpeed(pupil_speed);

        const float eye_speed = 15.0f;
        eye_right_position.setInterpolationFunction(Interpolation::Linear);
        eye_right_position.setSpeed(eye_speed);
        eye_left_position.setInterpolationFunction(Interpolation::Linear);
        eye_left_position.setSpeed(eye_speed);

        base_color_vec = Vec3(color.r, color.g, color.b);
    }

    void initialize(Walker const& creature)
    {
        link_va.resize(creature.getLinkCount() * 4);
        pods_cooldown.resize(creature.getPodCount());
        pods_radius.resize(creature.getPodCount());
        for (auto& c : pods_cooldown) {
            c = 0.0f;
        }

        for (auto& v : pods_radius) {
            v.setInterpolationFunction(Interpolation::Linear);
            v.setSpeed(50.0f);
        }

        eye_left_position.setValueInstant(creature.getHeadPosition());
        eye_right_position.setValueInstant(creature.getHeadPosition());
    }

    void update(Walker const& creature, float dt)
    {
        for (auto& c : pods_cooldown) {
            c -= dt;
        }

        render_target.update(dt);
    }

    void renderTarget(Vec2 target, pez::render::Context& context)
    {
        render_target.position = target;
        render_target.render(context);
    }

    void render(Walker const& creature, Vec2 target, pez::render::Context& context)
    {
        // Bezier
        sf::Color const body_color = ColorUtils::createColor(base_color_vec); //{140, 106, 70};
        auto const mid_joint = creature.getJoint(4).position;
        // Muscles
        float const muscle_contraction_size = 10.0f;

        float const offset_muscle = 10.0f;
        {
            float const contraction = creature.getMuscleRatio(0);
            auto const color = getMuscleColor(contraction);
            common::Utils::generateBezier(muscle_1_va, creature.getJoint(0).position, mid_joint, creature.getJoint(3).position, bezier_pts, color, -offset_muscle + (1.0f - contraction) * muscle_contraction_size);
        }

        {
            float const contraction = creature.getMuscleRatio(1);
            auto const color = getMuscleColor(contraction);
            common::Utils::generateBezier(muscle_2_va, creature.getJoint(1).position, mid_joint, creature.getJoint(2).position, bezier_pts, color, -offset_muscle + (1.0f - contraction) * muscle_contraction_size);
        }

        // Rest
        common::Utils::generateBezier(side_1_va, creature.getJoint(0).position, mid_joint, creature.getJoint(3).position, bezier_pts, body_color, -20.0f);
        common::Utils::generateBezier(side_2_va, creature.getJoint(1).position, mid_joint, creature.getJoint(2).position, bezier_pts, body_color, -20.0f);
        common::Utils::generateBezier(side_3_va, creature.getJoint(0).position, mid_joint, creature.getJoint(1).position, bezier_pts, body_color, 5.0f);
        common::Utils::generateBezier(side_4_va, creature.getJoint(3).position, mid_joint, creature.getJoint(2).position, bezier_pts, body_color, 5.0f);

        context.draw(muscle_1_va);
        context.draw(muscle_2_va);

        // Draw links (bones and muscles)
        auto const& objects = creature.system.objects;
        float const width   = 4.0f;
        common::Utils::generateLine(link_va,  0, objects[0].position, objects[4].position, width, base_color);
        common::Utils::generateLine(link_va,  4, objects[1].position, objects[4].position, width, base_color);
        common::Utils::generateLine(link_va,  8, objects[2].position, objects[4].position, width, base_color);
        common::Utils::generateLine(link_va, 12, objects[3].position, objects[4].position, width, base_color);
        context.draw(link_va);
        common::Utils::setVertexArrayColor(link_va, {0, 0, 0, dark_tone});
        context.draw(link_va);

        context.draw(side_1_va);
        context.draw(side_2_va);
        context.draw(side_3_va);
        context.draw(side_4_va);

        // Draw pods
        for (uint32_t i{0}; i < 4; ++i) {
            float const friction    = creature.getPodFriction(i);
            float const radius_base = 6.0f;
            float const radius      = radius_base + radius_base * friction;

            pods_radius[i] = radius;

            float const outline_width = 4.0f;

            float const smooth_radius = radius_base + pods_radius[i];
            sf::CircleShape pod{smooth_radius};
            pod.setOrigin(smooth_radius, smooth_radius);

            pod.setFillColor(base_color);
            auto const& p = creature.getPodConst(i);
            pod.setPosition(p.position);
            context.draw(pod);

            float outline_radius = smooth_radius - outline_width;
            pod.setRadius(outline_radius);
            pod.setOrigin(outline_radius, outline_radius);
            pod.setFillColor({0, 0, 0, 0});
            pod.setOutlineThickness(outline_width);
            pod.setOutlineColor({255, 255, 255, dark_tone});
            context.draw(pod);
        }

        // Draw eyes
        float const eye_radius  = 12.0f;
        float const eye_spacing = 14.0f;
        float const eye_dist    = 10.0f;

        sf::CircleShape eye(eye_radius);
        eye.setOrigin(eye_radius, eye_radius);

        auto const head_position  = creature.getHeadPosition();
        auto const head_direction = creature.getHeadDirection();
        auto const head_normal    = MathVec2::normal(head_direction);
        auto const eye_center     = head_position + head_direction * eye_dist;

        // Right
        eye_right_position = eye_center + head_direction + head_normal * eye_spacing;
        eye.setPosition(eye_right_position);
        context.draw(eye);

        // Left
        eye_left_position = eye_center + head_direction - head_normal * eye_spacing;
        eye.setPosition(eye_left_position);
        context.draw(eye);

        // Pupil
        float const pupil_radius    = 6.0f;
        float const pupil_max_dist  = eye_radius - pupil_radius;

        sf::CircleShape pupil(pupil_radius);
        pupil.setOrigin(pupil_radius, pupil_radius);
        pupil.setFillColor(sf::Color::Black);

        // Right
        auto const sight_direction_right = MathVec2::normalize(target - eye_right_position.get());
        pupil_right_position = sight_direction_right * pupil_max_dist;
        pupil.setPosition(eye_right_position.get() + pupil_right_position.get());
        context.draw(pupil);

        // Left
        auto const sight_direction_left = MathVec2::normalize(target - eye_left_position.get());
        pupil_left_position = sight_direction_left * pupil_max_dist;
        pupil.setPosition(eye_left_position.get() + pupil_left_position.get());
        context.draw(pupil);
    }

    static sf::Color getMuscleColor(float contraction)
    {
        auto const v{static_cast<uint8_t>(std::min(255.0f, std::min(255.0f, std::max(0.0f, 128.0f * (1.0f + contraction)))))};
        return {255, v, v};
    }
};