#pragma once
#include <cstdint>
#include "user/common/configuration.hpp"


struct TrainingState
{
    uint32_t iteration = 0;

    bool demo = true;

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
};