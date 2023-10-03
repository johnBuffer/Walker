#pragma once
#include <filesystem>

#include "engine/engine.hpp"

#include "user/training/walk.hpp"
#include "user/training/training_state.hpp"
#include "user/training/evolver.hpp"


struct Stadium : public pez::core::IProcessor
{
    TrainingState&  state;
    tp::ThreadPool& thread_pool;
    Evolver         evolver;

    Stadium()
        : state{pez::core::getSingleton<TrainingState>()}
        , thread_pool{pez::core::getSingleton<tp::ThreadPool>()}
    {
        RNGf::setSeed(2);

        // Create target sequences for demo and training
        pez::core::create<TargetSequence>();
        pez::core::create<TargetSequence>();

        // Create genomes
        for (uint32_t i{0}; i < conf::population_size; ++i) {
            auto const id = pez::core::create<Genome>();
            pez::core::get<Genome>(id).genome.loadFromFile("genomes_2_3201/best_1000.bin");
        }

        // Create tasks
        for (uint32_t i{0}; i < conf::population_size; ++i) {
            // The 1 is to use training target sequence (as opposed to the constant one for demo, 0)
            pez::core::create<training::Walk>(i, 1);
        }

        restartExploration();

        loadExistingGenome("genomes_2_3201/best_1000.bin");
    }

    void loadExistingGenome(std::string const& filename)
    {
        // Load the genome once
        nt::Genome genome{conf::input_count, conf::output_count};
        genome.loadFromFile(filename);
        // and copy it to all other
        pez::core::foreach<Genome>([&genome](Genome& g) {
            g.genome = genome;
        });
    }

    void update(float dt) override
    {
        // Check if we are in the demo, if yes, just skip
        if (state.demo) {
            return;
        }
        // Update state, increases iteration counter and automatically switches to demo mode if needed
        state.addIteration();
        // Run all tasks
        executeTasks(dt);
        // After all tasks has been completed, create the next generation
        evolver.createNewGeneration();
        // Depending on the configuration, dump the best genome to a file
        saveBest();
        // Check if we need to restart exploration
        if (needRestartExploration()) {
            restartExploration();
        }
    }

    /// Initializes the iteration
    void initializeIteration() const
    {
        pez::core::get<TargetSequence>(1).generateNewTargets();
        pez::core::parallelForeach<training::Walk>([&](training::Walk& walk) {
            walk.initialize();
        });
    }

    void executeTasks(float dt)
    {
        initializeIteration();

        uint32_t const tasks_count = pez::core::getCount<training::Walk>();
        auto&          tasks       = pez::core::getData<training::Walk>().getData();
        thread_pool.dispatch(tasks_count, [&](uint32_t start, uint32_t end) {
            float t = 0.0f;
            while (t < conf::max_iteration_time) {
                bool done = true;
                for (uint32_t i{start}; i < end; ++i) {
                    if (!tasks[i].done()) {
                        tasks[i].update(dt);
                        done = false;
                    }
                }
                if (done) {
                    break;
                }
                t += dt;
            }
        });
    }

    void saveBest(bool force = false) const
    {
        if ((state.iteration % conf::exp::best_save_period) == 0 || force) {
            pez::core::get<Genome>(0).genome.writeToFile(getCurrentFolder() + "/best_" + toString(state.iteration) + ".bin");
        }
    }

    [[nodiscard]]
    bool needRestartExploration() const
    {
        return state.iteration > conf::exp::exploration_period;
    }

    void restartExploration()
    {
        if (state.iteration_exploration) {
            std::filesystem::rename(getCurrentFolder(), getCurrentFolder() + "_" + toString(state.iteration_best_score, 0));
        }
        // Reset state
        state.newExploration();
        saveBest(true);
        // Change the seed of the RNG
        RNGf::setSeed(state.iteration_exploration + conf::exp::seed_offset);
        //RNGf::setSeed(10);
        // Create the folder to save genomes
        std::filesystem::create_directories(getCurrentFolder());
        // Reset genomes
        pez::core::foreach<Genome>([](Genome& g) {
            g.resetGenome();
        });
    }

    [[nodiscard]]
    std::string getCurrentFolder() const
    {
        return "genomes_" + toString(state.iteration_exploration);
    }
};