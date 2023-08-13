#pragma once

#include "engine/common/number_generator.hpp"

#include "./task.hpp"
#include "./configuration.hpp"
#include "common/physic/background/physics.hpp"
#include "engine/common/color_utils.hpp"


struct Simulation
{
    std::vector<Creature> creatures;
    std::vector<Task>     tasks;
    std::vector<Vec2>     targets;
    std::vector<uint32_t> target_remaining;

    tp::ThreadPool thread_pool;
    PhysicSolver   solver;

    float time = 0.0f;
    float const freeze_time = 0.0f;

    Simulation()
            : thread_pool{16}
            , solver(IVec2(480, 480), thread_pool)
    {
        createTargets();
        createBackground();

        //createCreature("res/progress/best_10.bin"  , sf::Color{200, 200, 200}, 10);
        //createCreature("res/progress/best_20.bin"  , sf::Color{200, 200, 200}, 20);
        //createCreature("res/progress/best_50.bin"  , sf::Color{200, 200, 200}, 50);
        //createCreature("res/progress/best_100.bin" , sf::Color{200, 200, 200}, 70); // 70
        //createCreature("res/progress/best_200.bin" , sf::Color{200, 200, 200}, 100); // 100
        //createCreature("res/progress/best_500.bin" , sf::Color{200, 200, 200}, 200); // 200
        //createCreature("res/progress/best_1000.bin", sf::Color{200, 200, 200}, 500); // 500
        //createCreature("res/progress/fine_tune/ft_750.bin", {200, 200, 200}, 750);
        createCreature("res/04.bin", {121, 123, 255}, "Solution 1");

        createCreature("res/4_pods_12.bin", {30, 148, 96}, "Solution 4");
        //createCreature("res/Video/4_pods_12.bin", {255, 133, 81}, "Solution 2");
        //createCreature("res/Video/dna.bin", {128, 85, 140}, "Solution 3");


        //createCreature("res/best_2000.bin", sf::Color{100, 250, 100});
        //createCreature("res/best_1050.bin", sf::Color{250, 150, 100});
        //createCreature("res/best_440.bin", sf::Color{250, 250, 100});
        //createCreature("res/best_220.bin", sf::Color{100, 100, 250});

        target_remaining.resize(targets.size());
        for (auto& t : target_remaining) {
            t = tasks.size();
        }
    }

    void update(float dt);

    void createCreature(std::string const& genome_filename, sf::Color color, std::string const& name)
    {
        creatures.emplace_back(conf::world_size * 0.5f);
        tasks.emplace_back(color);
        tasks.back().creature_idx = creatures.size() - 1;
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
        float const physic_scale = conf::max_distance / static_cast<float>(solver.grid.width);
        for (auto& creature : creatures) {
            for (auto& obj: solver.objects) {
                for (uint32_t i{0}; i < 4; ++i) {
                    auto const& pod     = creature.getJoint(i);
                    Vec2 const  pod_pos = pod.position / physic_scale;
                    Vec2 v = obj.position - pod_pos;
                    float const dist = MathVec2::length(v);
                    float const pod_radius = 5.0f * (pod.friction);
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

    void createExplosion(uint32_t target_id, sf::Color color)
    {
        float const physic_scale = conf::max_distance / static_cast<float>(solver.grid.width);

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

    void createExplosionCone(uint32_t target_id, Creature const& creature, sf::Color color)
    {
        float const physic_scale = conf::max_distance / static_cast<float>(solver.grid.width);

        auto const& position     = targets[target_id];
        auto const  world_pos    = position / physic_scale;
        float const radius       = 50.0f;
        float const world_radius = radius / physic_scale;
        for (auto& obj: solver.objects) {
            Vec2 const v = obj.position - world_pos;
            float const d = MathVec2::length(v);
            Vec2 const  n = v / d;
            float const dot = MathVec2::dot(n, creature.getHeadDirection());
            if (dot > 0.6f) {
                if (d < world_radius) {
                    obj.position += (world_radius - d) * MathVec2::normalize(v) * 0.6f * dot;
                    obj.color = color;
                    obj.radius = 1.0f;
                    obj.current_ratio = 0.75f;
                }
            }
        }
    }
};
