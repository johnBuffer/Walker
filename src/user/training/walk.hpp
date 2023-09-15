#pragma once
#include "engine/engine.hpp"
#include "engine/common/racc.hpp"

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
    /// Struct to store the state of the walker at a given point
    struct State
    {
        float pod[4];
        float muscle[2];

        void setPodState(uint32_t i, float s)
        {
            pod[i] = s;
        }

        void setMuscleState(uint32_t i, float s)
        {
            muscle[i] = s;
        }
    };

    /// ===== Attributes =====
    /// The genome of this agent
    pez::core::ID genome_id          = pez::core::EntityID::INVALID_ID;
    pez::core::ID target_sequence_id = pez::core::EntityID::INVALID_ID;
    /// The network generated by the genome
    nt::Network<conf::input_count, conf::output_count> network;
    /// The walker that will be controlled by this agent
    Walker walker;

    uint32_t current_target = 0;

    float time = 0.0f;

    RAccBase<State> state;

    explicit
    Walk(pez::core::EntityID id_, pez::core::ID genome_id_, pez::core::ID target_sequence_id_)
        : Task{id_}
        , genome_id{genome_id_}
        , target_sequence_id{target_sequence_id_}
        , state{10}
    {}

    void initialize() override
    {
        walker = Walker{conf::world_size * 0.5f};
        current_target = 0;

        auto& genome = getGenome();
        // Update the network
        network = genome.generateNetwork();
        genome.score = 0.0f;
    }

    [[nodiscard]]
    Vec2 getCurrentTarget() const
    {
        return pez::core::get<TargetSequence>(target_sequence_id).getTarget(current_target);
    }

    void update(float dt) override
    {
        auto& genome = getGenome();
        // Get the current target to reach
        Vec2 const target = getCurrentTarget();

        /*walker.setMuscleRatio(0, sin(time));
        walker.update(dt);
        time += dt;
        return;*/

        // Update state delay
        State current_state;
        for (uint32_t i{0}; i < 4; ++i) {
            current_state.setPodState(i, walker.getPodFriction(i));
        }
        for (uint32_t i{0}; i < 2; ++i) {
            current_state.setMuscleState(i, walker.getMuscleRatio(i));
        }
        state.addValueBase(current_state);

        // Update AI
        updateAI(walker, target);

        // Update physic
        walker.update(dt);

        // Check if target is reached
        const Vec2  to_target       = target - walker.getHeadPosition();
        float const dist_to_target  = MathVec2::length(to_target);
        if (dist_to_target < conf::target_radius) {
            ++current_target;
            genome.score += conf::target_reward;
        }

        // Update score
        getGenome().score += 1.0f / (1.0f + dist_to_target) * dt;
    }

    void updateAI(Walker& creature, Vec2 target)
    {
        State const state_delay = state.get();

        const Vec2  to_target       = target - creature.getHeadPosition();
        float const dist_to_target  = MathVec2::length(to_target);
        const float to_target_dot   = MathVec2::dot(to_target / dist_to_target, creature.getHeadDirection());
        const float to_target_dot_n = MathVec2::dot(to_target / dist_to_target, MathVec2::normal(creature.getHeadDirection()));
        const auto& output = network.execute({
            dist_to_target / conf::maximum_distance, // Distance to target
            to_target_dot,                           // Direction evaluation
            to_target_dot_n,                         // Direction normal evaluation
            state_delay.pod[0],              // Pods state
            state_delay.pod[1],
            state_delay.pod[2],
            state_delay.pod[3],
            state_delay.muscle[0],               // Muscles state
            state_delay.muscle[1],
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

    Genome& getGenome()
    {
        return pez::core::get<Genome>(genome_id);
    }
};
}
