#pragma once
#include "verlet_object.hpp"
#include "engine/common/math.hpp"


struct VerletLink
{
    uint32_t idx_1 = 0;
    uint32_t idx_2 = 0;

    float rest_length    = 0.0f;
    float target_length  = 0.0f;
    float strength       = 1.0f;
    float current_length = 0.0f;

    bool is_muscle = false;

    VerletLink() = default;

    VerletLink(uint32_t idx_1_, uint32_t idx_2_, float target_length_)
        : idx_1{idx_1_}
        , idx_2{idx_2_}
        , rest_length{target_length_}
        , target_length{target_length_}
    {
    }

    VerletLink(uint32_t idx_1_, uint32_t idx_2_, std::vector<VerletObject>& objects)
        : idx_1{idx_1_}
        , idx_2{idx_2_}
    {
        auto& object_1 = objects[idx_1];
        auto& object_2 = objects[idx_2];
        target_length  = MathVec2::length(object_1.position - object_2.position);
        rest_length    = target_length;
    }

    void update(std::vector<VerletObject>& objects)
    {
        auto& object_1 = objects[idx_1];
        auto& object_2 = objects[idx_2];

        const Vec2  v      = object_1.position - object_2.position;
        const float length = MathVec2::length(v);
        const Vec2  n      = v / length;

        const float delta = target_length - length;
        object_1.position += (0.5f * delta * strength) * n;
        object_2.position -= (0.5f * delta * strength) * n;

        current_length = length;
    }
};