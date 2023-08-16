#pragma once
#include "verlet_object.hpp"
#include "engine/common/math.hpp"


struct VerletLink
{
    uint32_t idx_1 = 0;
    uint32_t idx_2 = 0;

    float target_length  = 0.0f;
    float strength       = 1.0f;
    float current_length = 0.0f;

    bool is_muscle = false;

    VerletLink() = default;

    VerletLink(uint32_t idx_1_, uint32_t idx_2_, float target_length_)
        : idx_1{idx_1_}
        , idx_2{idx_2_}
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
    }

    void update(std::vector<VerletObject>& objects)
    {
        auto& object_1 = objects[idx_1];
        auto& object_2 = objects[idx_2];

        const Vec2  v      = object_1.position - object_2.position;
        const float length = MathVec2::length(v);
        const Vec2  n      = v / length;

        float const mass_total = object_1.getWFriction() + object_2.getWFriction();
        float const obj_1_mass_ratio = object_1.getWFriction() / mass_total;
        float const obj_2_mass_ratio = object_2.getWFriction() / mass_total;

        const float delta = target_length - length;
        float const base_delta = 0.5f * delta * strength;

        object_1.position += (base_delta * obj_1_mass_ratio) * n;
        object_2.position -= (base_delta * obj_2_mass_ratio) * n;

        current_length = length;
    }
};