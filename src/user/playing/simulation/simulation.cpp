#include "simulation.hpp"


void playing::Simulation::update(float dt)
{
    time += dt;
    for (auto& t : tasks) {
        bool const reached = t.update(dt, creatures[t.creature_idx], targets[t.target_idx]);
        if (reached) {
            t.rank = target_remaining[t.target_idx - 1]--;
            createExplosion(t.target_idx - 1, t.color);
        }
    }
    computeGroundCollision();
}