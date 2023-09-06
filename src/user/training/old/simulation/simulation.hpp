#pragma once
#include <filesystem>

#include "engine/common/index_vector.hpp"


#include "user/common/neat_old/nn_utils.hpp"
#include "user/common/neat_old/mutator.hpp"
#include "user/common/thread_pool/thread_pool.hpp"

#include "user/training/old/stadium/selector.hpp"
#include "user/training/old/stadium/genetic_info.hpp"
#include "user/training/old/stadium/stadium.hpp"
#include "user/training/old/stadium/evolver.hpp"

#include "walk.hpp"


namespace training
{
struct Simulation
{
    enum class State : uint8_t
    {
        Evaluating     = 0,
        Demo           = 1,
    };

    State state = State::Demo;

    Vec2 world_size;

    static constexpr uint32_t group_size = 2000;
    float                     time       = 0.0f;

    std::vector<Walk>                              tasks;
    civ::IndexVector<Walk::Genetic>                genomes;
    Evolver<conf::input_count, conf::output_count> evolver;
    Stadium<Walk>                                  stadium;

    std::vector<Vec2> targets;
    std::vector<Vec2> targets_evaluation;

    Walk  demo_walk;
    float demo_max_time = 0.0f;

    uint32_t    seed = 7;
    std::string current_folder;
    float       best_score = 0.0f;

    std::string fine_tune_directory = "video_1";

    explicit
    Simulation()
        : world_size{conf::maximum_distance, conf::maximum_distance}
        , stadium{tasks}
    {
        demo_max_time = stadium.getTaskTime();
        // Create genomes
        restartExploration();
        // Create evaluation targets
        generateEvaluationTargets();
    }

    void restartExploration()
    {
        genomes = {};
        for (uint32_t i{group_size}; i--;) {
            auto id = genomes.emplace_back();
            //genomes[id].genome.loadFromFile(fine_tune_directory + "/base.bin");
            //genomes[id].genome.generateConnections();
        }

        targets.resize(1000);
        generateTargets();

        ++seed;
        RNGf::setSeed(seed);
        current_folder = "genomes_" + toString(seed);
        std::filesystem::create_directories(current_folder);

        for (uint32_t i{group_size}; i--;) {
            Walk& task = tasks.emplace_back();
            task.genome = genomes.createRefFromData(i);
            task.targets = targets;
        }

        evolver.reset();
        newGeneration();

        best_score = 0.0f;
    }

    void update(float dt)
    {
        if (state == State::Evaluating) {
            stadium.executeIteration(dt);
            // Create genomes for next generation
            newGeneration();
            // Switch to the demo
            time = 0.0f;
            //std::cout << "Iteration done" << std::endl;
            if (evolver.iteration % 5 == 0) {
                state = State::Demo;
                demo_walk.targets = targets_evaluation;
            }
        } else if (state == State::Demo) {
            // Demo
            demo_walk.update(dt);
            time += dt;
            if (time > demo_max_time) {
                // Switch to evaluation if demo is done
                state = State::Evaluating;
                best_score = std::max(demo_walk.genome->score, best_score);
                std::cout << "Evaluation DONE [" << evolver.iteration << "]: " << demo_walk.genome->score << std::endl;
                demo_walk.genome->score = 0.0f;
            }
        }
    }

    void newGeneration()
    {
        const auto new_genomes = evolver.createNewGeneration(genomes.getData());

        if (evolver.iteration % 50 == 0) {
            new_genomes[0].genome.writeToFile(current_folder + "/best_" + toString(evolver.iteration) + ".bin");
            //new_genomes[0].genome.writeToFile(fine_tune_directory + "/exp_" + toString(seed) + "_gen_" + toString(evolver.iteration) + "_" + toString(int(best_score)) + ".bin");
        }

        // Replace current genomes with the new ones
        genomes.getData() = new_genomes;

        // updateBestNetwork(new_genomes[0].network);
        //std::cout << "End of iteration " << evolver.iteration << " best: " << new_genomes[0].score << std::endl;

        // Reset genomes for next iteration
        for (auto& g : genomes) { g.reset(); }

        demo_walk.genome = genomes.createRefFromData(0);
        demo_walk.targets = targets;
        demo_walk.initialize();

        generateTargets();

        for (auto& t : tasks) {
            t.targets = targets;
        }

        if (evolver.iteration == 10001) {
            //std::filesystem::rename(current_folder, current_folder + "_" + toString(int(best_score)));
            restartExploration();
        }
    }

    void generateTargets()
    {
        targets.clear();
        for (uint32_t i{0}; i < 500; ++i) {
            targets.push_back(generateOneTarget(conf::maximum_distance));
        }
        targets[0].y = RNGf::getUnder(world_size.y * 0.5f);
    }

    void generateEvaluationTargets()
    {
        for (uint32_t i{0}; i < 500; ++i) {
            targets_evaluation.push_back(generateOneTarget(conf::maximum_distance));
        }
        targets_evaluation[0].y = RNGf::getUnder(world_size.y * 0.5f);
    }

    Vec2 generateOneTarget(float radius)
    {
        float const angle = RNGf::getUnder(Math::TwoPI);
        float const dist  = sqrt(RNGf::getUnder(radius * radius));
        return {conf::maximum_distance * 0.5f + dist * cos(angle), conf::maximum_distance * 0.5f + dist * sin(angle)};
    }

    Walk& getDemoTask()
    {
        return tasks.front();
    }
};
}
