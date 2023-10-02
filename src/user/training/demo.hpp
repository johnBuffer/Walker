#pragma once
#include "engine/engine.hpp"
#include "user/training/training_state.hpp"
#include "user/training/walk.hpp"


namespace training
{

struct Demo : public pez::core::IProcessor
{
    TrainingState&  state;

    Walk task;

    float time     = 0.0f;
    bool need_init = true;

    Demo()
        : state{pez::core::getSingleton<TrainingState>()}
        , task{{}, 0, 1}
    {
        task.initialize();
    }

    void initialize();

    void update(float dt) override
    {
        if (!state.demo) {
            return;
        }

        if (need_init) {
            initialize();
        }

        time += dt;
        task.update(dt);

        // Switch back to training when demo is over
        if (time >= conf::max_iteration_time) {
            state.endDemo();
            need_init  = true;
            std::cout << "Demo score: " << task.getGenome().score << std::endl;
            state.iteration_best_score = task.getGenome().score;
        }
    }
};

}
