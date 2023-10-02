#pragma once
#include <cstdint>
#include "user/common/configuration.hpp"


struct TrainingState
{
    uint32_t iteration             = 0;
    uint32_t iteration_exploration = 0;
    float    iteration_best_score  = 0.0f;

    bool demo = false;

    void addIteration()
    {
        ++iteration;
        if (iteration % conf::demo_period == 0) {
            demo = true;
        }
    }

    void endDemo()
    {
        demo = false;
    }

    void newExploration()
    {
        iteration            = 0;
        iteration_best_score = 0.0f;
        ++iteration_exploration;
    }
};