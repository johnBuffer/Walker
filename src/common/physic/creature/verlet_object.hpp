#pragma once
#include "engine/common/vec.hpp"


struct VerletObject
{
    Vec2 position      = {0.0f, 0.0f};
    Vec2 position_last = {0.0f, 0.0f};
    Vec2 acceleration  = {0.0f, 0.0f};

    float friction = 0.0f;

    VerletObject() = default;

    explicit
    VerletObject(Vec2 position_)
        : position{position_}
        , position_last{position_}
    {

    }

    void update(float dt)
    {
        const Vec2 frame_move = position - position_last;
        position_last = position;
        position      = position + (1.0f - friction) * (frame_move + (acceleration - frame_move * 100.0f) * (dt * dt));
        acceleration  = {0.0f, 0.0f};
    }
};