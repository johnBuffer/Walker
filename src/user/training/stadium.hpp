#pragma once
#include "engine/engine.hpp"

#include "user/training/walk.hpp"
#include "user/training/training_state.hpp"
#include "user/training/evolver.hpp"


struct Stadium : public pez::core::IProcessor
{
    TrainingState&  state;
    tp::ThreadPool& thread_pool;
    Evolver<conf::input_count, conf::output_count> evolver;

    Stadium()
        : state{pez::core::getSingleton<TrainingState>()}
        , thread_pool{pez::core::getSingleton<tp::ThreadPool>()}
    {}

    void update(float dt) override
    {
        // Check if we are in the demo, if yes, just skip
        if (state.demo) {
            return;
        }

        state.addIteration();

        executeTasks(dt);

        evolver.createNewGeneration();
    }

    void executeTasks(float dt)
    {
        /*uint32_t const tasks_count = pez::core::getCount<Walk>();
        auto&          tasks       = pez::core::getData<Walk>().getData();
        //initializeIteration();
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
        });*/
    }
};
