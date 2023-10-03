#pragma once

#include "engine/engine.hpp"
#include "engine/common/number_generator.hpp"

#include "./task.hpp"
#include "user/common/configuration.hpp"
#include "user/playing/sand/physics.hpp"
#include "engine/common/color_utils.hpp"


namespace playing
{

struct Simulation : public pez::core::IProcessor
{
    PhysicSolver& solver;

    std::vector<Walker>   walkers;
    std::vector<WalkTask> tasks;
    std::vector<Vec2>     targets;
    std::vector<uint64_t> target_remaining;

    float time = 0.0f;
    float const freeze_time = 0.0f;

    Simulation()
        : solver{pez::core::getProcessor<PhysicSolver>()}
    {
        createTargets();
        createBackground();

        createWalker("res/genomes/example_1.bin", {30, 148, 96}, "Solution 1");

        target_remaining.resize(targets.size());
        for (auto& t : target_remaining) {
            t = tasks.size();
        }
    }

    void update(float dt) override;

    void createWalker(std::string const& genome_filename, sf::Color color, std::string const& name)
    {
        walkers.emplace_back(conf::world_size * 0.5f);
        tasks.emplace_back(color);
        tasks.back().walker_idx = walkers.size() - 1;
        tasks.back().loadGenome(genome_filename);
        tasks.back().name = name;
    }

    void createTargets()
    {
        float const target_margin = 0.05f;
        float const target_max    = (1.0f - target_margin * 2.0f);
        targets.clear();
        for (uint32_t i{1000}; i--;) {
            targets.emplace_back(conf::world_size.x * target_margin + RNGf::getUnder(conf::world_size.x * target_max),
                                 conf::world_size.y * target_margin + RNGf::getUnder(conf::world_size.y * target_max));
        }
    }

    void createBackground()
    {
        float const target_margin = 0.0f;
        float const target_max    = (1.0f - target_margin * 2.0f);

        float const solver_size{static_cast<float>(solver.grid.width)};
        for (uint32_t i{0}; i < 120000; ++i) {
            auto const id  = solver.createObject({solver_size * target_margin + RNGf::getUnder(solver_size * target_max),
                                                  solver_size * target_margin + RNGf::getUnder(solver_size * target_max)});
            auto&      obj = solver.objects[id];
            obj.color_ratio = RNGf::getRange(0.4f, 0.6f);
            obj.current_ratio = obj.color_ratio;
            obj.color       = sf::Color::White;
        }
    }

    void computeGroundCollision()
    {
        float const physic_scale = conf::maximum_distance / static_cast<float>(solver.grid.width);
        for (auto& creature : walkers) {
            for (auto& obj: solver.objects) {
                for (uint32_t i{0}; i < 4; ++i) {
                    auto const& pod     = creature.getPod(i);
                    Vec2 const  pod_pos = pod.position / physic_scale;
                    Vec2 v = obj.position - pod_pos;
                    float const dist = MathVec2::length(v);
                    float const pod_radius = 8.0f * (pod.friction);
                    if (dist < pod_radius) {
                        obj.position += (pod_radius - dist) * MathVec2::normalize(v) * 0.25f;
                    }
                }
            }
        }

        for (auto& obj : solver.objects) {
            obj.slowdown(0.2f);
        }
    }

    void createExplosion(uint64_t target_id, sf::Color color)
    {
        float const physic_scale = conf::maximum_distance / static_cast<float>(solver.grid.width);

        auto const& position     = targets[target_id];
        auto const  world_pos    = position / physic_scale;
        float const radius       = 50.0f;
        float const world_radius = radius / physic_scale;
        for (auto& obj: solver.objects) {
            Vec2 v = obj.position - world_pos;
            float const dist = MathVec2::length2(v);
            if (dist < world_radius * world_radius) {
                obj.position += (world_radius - sqrt(dist)) * MathVec2::normalize(v) * 0.4f;
                obj.color = color;
                obj.radius = 1.0f;
                obj.current_ratio = 0.75f;
            }
        }
    }
};
}
