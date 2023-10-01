#pragma once
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
        }

        // Create tasks
        for (uint32_t i{0}; i < conf::population_size; ++i) {
            // The 1 is to use training target sequence (as opposed to the constant one for demo, 0)
            pez::core::create<training::Walk>(i, 1);
        }
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

    void saveBest() const
    {
        if ((state.iteration % conf::util::best_save_period) == 0) {
            pez::core::get<Genome>(0).genome.writeToFile("best_" + toString(state.iteration) + ".bin");
        }
    }
};
