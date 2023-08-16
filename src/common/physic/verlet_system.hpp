#pragma once
#include "verlet_object.hpp"
#include "verlet_link.hpp"


struct VerletSystem
{
    std::vector<VerletObject> objects;
    std::vector<VerletLink>   links;

    void update(float dt)
    {
        for (auto& o : objects) {
            //o.acceleration = {0.0f, 1000.0f};
        }

        constexpr uint32_t iteration_count{1};
        float const        sub_dt{dt/static_cast<float>(iteration_count)};
        for (uint32_t i{iteration_count}; i--;) {
            for (auto &l : links) {
              l.update(objects);
            }

            for (auto &o : objects) {
              o.update(sub_dt);
            }
        }
    }

};