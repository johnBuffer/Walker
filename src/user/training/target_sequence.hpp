#pragma once
#include "engine/engine.hpp"


struct TargetSequence
{
    // A sequence of targets that will never change in order to evaluate global progression
    std::vector<Vec2> demo_targets;
    
    // The current target sequence
    std::vector<Vec2> targets;
};
