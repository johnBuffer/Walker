#pragma once
#include <array>
#include <SFML/Graphics.hpp>

#include "engine/common/math.hpp"

#include "user/common/walker.hpp"
#include "user/common/configuration.hpp"
#include "user/common/neat/genome.hpp"


struct WalkTask
{
    uint64_t walker_idx = {0};
    uint64_t target_idx = {0};
    nt::Genome  genome;
    nt::Network network;

    sf::Color color;

    uint64_t    rank = 0;
    std::string name;

    explicit
    WalkTask(sf::Color color_)
        : color{color_}
    {}

    bool update(float dt, Walker& walker, Vec2 target)
    {
        // Update AI
        updateAI(walker, target);

        // Update physic
        walker.update(dt);

        // Check target
        const Vec2  to_target       = target - walker.getHeadPosition();
        float const dist_to_target  = MathVec2::length(to_target);
        if (dist_to_target < conf::target_radius) {
            ++target_idx;
            return true;
        }
        return false;
    }

    void updateAI(Walker& walker, Vec2 target)
    {
        const Vec2  to_target       = target - walker.getHeadPosition();
        float const dist_to_target              = MathVec2::length(to_target);
        const float to_target_dot   = MathVec2::dot(to_target / dist_to_target, walker.getHeadDirection());
        const float to_target_dot_n = MathVec2::dot(to_target / dist_to_target, MathVec2::normal(walker.getHeadDirection()));
        bool const success = network.execute({
            dist_to_target / conf::maximum_distance, // Distance to target
            to_target_dot,                           // Direction evaluation
            to_target_dot_n,                         // Direction normal evaluation
            walker.getPodFriction(0),                // Pods state
            walker.getPodFriction(1),
            walker.getPodFriction(2),
            walker.getPodFriction(3),
            walker.getMuscleRatio(0),                // Muscles state
            walker.getMuscleRatio(1),
        });

        if (success) {
            auto const& output = network.getResult();
            for (uint32_t i{0}; i<4; ++i) {
                walker.setPodFriction(i, 0.5f * (1.0f + output[i]));
            }

            for (uint32_t i{0}; i<2; ++i) {
                walker.setMuscleRatio(i, output[4 + i]);
            }
        }

    }

    void loadGenome(std::string const& filename)
    {
        genome.loadFromFile(filename);
        network = genome.generateNetwork();
    }
};