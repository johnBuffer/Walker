#include "simulation.hpp"


void playing::Simulation::update(float dt)
{
    if (time < freeze_time && time > 0.0f) {
        time += dt;
        return;
    }
    time += dt;
    for (auto& t : tasks) {
        bool const reached = t.update(dt, creatures[t.creature_idx], targets[t.target_idx]);
        if (reached) {
            t.rank = target_remaining[t.target_idx - 1]--;
            createExplosion(t.target_idx - 1, t.color);
            //createExplosionCone(t.target_idx - 1, creatures[t.creature_idx], t.color);
        }
    }
    computeGroundCollision();
    solver.update(dt);
}