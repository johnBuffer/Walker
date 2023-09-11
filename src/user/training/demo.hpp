#pragma once
#include "engine/engine.hpp"
#include "user/training/training_state.hpp"


namespace training
{

struct Demo : public pez::core::IProcessor
{
    TrainingState&  state;

    std::vector<Vec2> targets;
    Walk              task;


    float time = 0.0f;

    Demo()
        : state{pez::core::getSingleton<TrainingState>()}
    {
        initialize();
    }

    void initialize()
    {
        time = 0.0f;
        task.initialize();
    }

    void update(float dt) override
    {
        if (!state.demo) {
            return;
        }

        time += dt;

        task.update(dt);

        // When demo is over, prepare the next one
        if (time >= conf::max_iteration_time) {
            initialize();
            // Switch back to training
            state.demo = false;
        }
    }
};

}
