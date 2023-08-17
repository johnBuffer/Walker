#pragma once
#include <SFML/Graphics.hpp>
#include "engine/common/math.hpp"

#include "user/common/creature.hpp"
#include "user/common/configuration.hpp"
#include "user/common/neat_old/genome.hpp"


struct WalkTask
{
    uint32_t creature_idx = {0};
    uint32_t target_idx   = {0};
    nt::Genome<9, 6>  genome;
    nt::Network<9, 6> network;

    sf::Color color;

    uint32_t rank = 0;
    std::string name;

    explicit
    WalkTask(sf::Color color_)
        : color{color_}
    {}

    bool update(float dt, Creature& creature, Vec2 target)
    {
        // Update AI
        updateAI(creature, target);

        // Update physic
        creature.update(dt);

        // Check target
        const Vec2  to_target       = target - creature.getHeadPosition();
        float const dist_to_target  = MathVec2::length(to_target);
        if (dist_to_target < conf::target_radius) {
            ++target_idx;
            return true;
        }
        return false;
    }

    void updateAI(Creature& creature, Vec2 target)
    {
        const Vec2  to_target       = target - creature.getHeadPosition();
        float const dist_to_target              = MathVec2::length(to_target);
        const float to_target_dot   = MathVec2::dot(to_target / dist_to_target, creature.getHeadDirection());
        const float to_target_dot_n = MathVec2::dot(to_target / dist_to_target, MathVec2::normal(creature.getHeadDirection()));
        const auto& output = network.execute({
            dist_to_target / conf::maximum_distance, // Distance to target
            to_target_dot,                       // Direction evaluation
            to_target_dot_n,                     // Direction normal evaluation
            creature.getPodFriction(0),          // Pods state
            creature.getPodFriction(1),
            creature.getPodFriction(2),
            creature.getPodFriction(3),
            creature.getMuscleRatio(0),           // Muscles state
            creature.getMuscleRatio(1),
        });

        for (uint32_t i{0}; i<4; ++i) {
            creature.setPodFriction(i, 0.5f * (1.0f + output[i]));
        }

        for (uint32_t i{0}; i<2; ++i) {
            creature.setMuscleRatio(i, output[4 + i]);
        }
    }

    void loadGenome(std::string const& filename)
    {
        genome.loadFromFile(filename);
        network = genome.generateNetwork();
    }
};