#pragma once
#include <array>

#include "user/training/old/stadium/task.hpp"
#include "user/common/creature.hpp"
#include "user/common/configuration.hpp"


struct Walk : public Task
{
    using Genetic = GeneticInfo<conf::input_count, conf::output_count>;
    civ::Ref<Genetic> genome;
    float dist_to_target = 0.0f;
    float time           = 0.0f;

    std::vector<Vec2> targets;
    uint32_t current_target = 0;

    Creature creature;
    float energy      = 0.0f;
    float target_time = 0.0f;

    float muscle_speed = 4.0f;

    /// Prepare for the task before its execution
    void initialize() override
    {
        time = 0.0f;
        creature = Creature(getCenter());
        current_target = 0;
        energy = 0.0f;
        target_time = 0.0f;
    }

    /// The update to perform at each timestep of the task's execution
    void update(float dt) override
    {
        time += dt;

        //creature.setMuscleRatio(1, sin(1.0f * time));
        //creature.setMuscleRatio(0, sin(2.0f * time));
        //creature.update(dt);
        //return;


        const Vec2  to_target       = targets[current_target] - creature.getHeadPosition();
        dist_to_target              = MathVec2::length(to_target);
        const Vec2  to_target_v     = to_target / dist_to_target;
        const float to_target_dot   = MathVec2::dot(to_target_v, creature.getHeadDirection());
        const float to_target_dot_n = MathVec2::dot(to_target_v, MathVec2::normal(creature.getHeadDirection()));
        const auto& output = genome->network.execute({
            dist_to_target / conf::maximum_distance, // Dist to target
            to_target_dot,  // Direction evaluation
            to_target_dot_n, // Direction normal evaluation
            creature.getPodFriction(0), // Pods friction
            creature.getPodFriction(1),
            creature.getPodFriction(2),
            creature.getPodFriction(3),

            creature.getMuscleRatio(0),
            creature.getMuscleRatio(1),
        });

        for (uint32_t i{0}; i<4; ++i) {
            creature.setPodFriction(i, 0.5f * (1.0f + output[i]));
        }

        for (uint32_t i{0}; i<2; ++i) {
            creature.setMuscleRatio(i, output[4 + i]);
        }

        creature.update(dt);
        auto current_contraction = Vec2();
        energy += std::abs(current_contraction.x - creature.last_contraction.x) + std::abs(current_contraction.y - creature.last_contraction.y);

        //float const forward = (1.0f + MathVec2::dot(to_target_v, creature.getHeadDirection()));

        target_time += dt;
        if (dist_to_target < conf::target_radius) {
            current_target++;
            genome->score += 100.0f;
            energy = 0.0f;
            target_time = 0.0f;
            creature.moveTo(getCenter());
        } else {
            genome->score += 0.1f / (1.0f + dist_to_target);
        }
    }

    /// Compute the resulting score
    void computeScore() override
    {
        genome->updateGeneticScore();
    }

    /// Check if the the task is finished
    [[nodiscard]]
    bool done() const override
    {
        return false;
    }

    static Vec2 getCenter()
    {
        return 0.5f * Vec2{conf::maximum_distance, conf::maximum_distance};
    }
};