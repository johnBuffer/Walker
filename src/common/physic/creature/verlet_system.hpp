#pragma once
#include "verlet_object.hpp"
#include "verlet_link.hpp"


struct VerletSystem
{
    std::vector<VerletObject> objects;
    std::vector<VerletLink>   links;

    void update(float dt)
    {
        for (auto& l : links) {
            l.update(objects);
        }

        for (auto& o : objects) {
            o.update(dt);
        }
    }

};