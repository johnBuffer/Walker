#pragma once
#include "engine/engine.hpp"

#include "user/common/configuration.hpp"
#include "user/common/walker.hpp"
#include "user/common/neat_old/network.hpp"

#include "user/training/task.hpp"
#include "user/training/genome.hpp"
#include "user/training/target_sequence.hpp"


namespace training
{
struct Walk : public training::Task
{
    /// ===== Attributes =====
    /// The genome of this agent
    pez::core::ID genome_id          = pez::core::EntityID::INVALID_ID;
    pez::core::ID target_sequence_id = pez::core::EntityID::INVALID_ID;
    /// The network generated by the genome
    nt::Network<conf::input_count, conf::output_count> network;
    /// The walker that will be controlled by this agent
    Walker walker;

    uint32_t current_target = 0;

    explicit
    Walk(pez::core::EntityID id_, pez::core::ID genome_id_, pez::core::ID target_sequence_id_)
        : Task{id_}
        , genome_id{genome_id_}
        , target_sequence_id{target_sequence_id_}
    {}

    void initialize() override
    {
        current_target = 0;

        // Update the network
        network = pez::core::get<Genome>(genome_id).genome.generateNetwork();
    }

    void update(float dt) override
    {
        std::cout << "Update task " << target_sequence_id << std::endl;

        // Get the current target to reach
        Vec2 const target = pez::core::get<TargetSequence>(target_sequence_id).getTarget(current_target);

        // Update AI
        updateAI(walker, target);

        /*
        // Update physic
        walker.update(dt);

        // Check if target is reached
        const Vec2  to_target       = target - walker.getHeadPosition();
        float const dist_to_target  = MathVec2::length(to_target);
        if (dist_to_target < conf::target_radius) {
            ++current_target;
        }
         */
    }

    void updateAI(Walker& creature, Vec2 target)
    {
        const Vec2  to_target       = target - creature.getHeadPosition();
        float const dist_to_target  = MathVec2::length(to_target);
        const float to_target_dot   = MathVec2::dot(to_target / dist_to_target, creature.getHeadDirection());
        const float to_target_dot_n = MathVec2::dot(to_target / dist_to_target, MathVec2::normal(creature.getHeadDirection()));
        const auto& output = network.execute({
            dist_to_target / conf::maximum_distance, // Distance to target
            to_target_dot,                           // Direction evaluation
            to_target_dot_n,                         // Direction normal evaluation
            creature.getPodFriction(0),              // Pods state
            creature.getPodFriction(1),
            creature.getPodFriction(2),
            creature.getPodFriction(3),
            creature.getMuscleRatio(0),               // Muscles state
            creature.getMuscleRatio(1),
        });

        for (uint32_t i{0}; i<4; ++i) {
            creature.setPodFriction(i, 0.5f * (1.0f + output[i]));
        }

        for (uint32_t i{0}; i<2; ++i) {
            creature.setMuscleRatio(i, output[4 + i]);
        }
    }

    [[nodiscard]]
    bool done() const override
    {
        return false;
    }
};
}
