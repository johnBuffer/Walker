#pragma once
#include "verlet_object.hpp"
#include "verlet_link.hpp"


struct VerletSystem
{
    std::vector<VerletObject> objects;
    std::vector<VerletLink>   links;

    bool current_order = true;

    void update(float dt)
    {
        constexpr uint32_t iteration_count{1};
        float const        sub_dt{dt/static_cast<float>(iteration_count)};
        for (uint32_t i{iteration_count}; i--;) {
            // Swap update order each frame to limit biases
            if (current_order) {
                for (auto& l : links) { l.update(objects); }
            } else {
                for (auto it = links.rbegin(); it != links.rend(); ++it) { it->update(objects); }
            }
            current_order = !current_order;

            for (auto& o : objects) {
                o.update(sub_dt);
            }
        }
    }

};