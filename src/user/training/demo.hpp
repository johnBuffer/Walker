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

    float time = 0.0f;

    Demo()
        : state{pez::core::getSingleton<TrainingState>()}
        , task{pez::core::EntityID{}, 0, 0}
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
