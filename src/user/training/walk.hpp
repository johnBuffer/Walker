#pragma once
#include "engine/engine.hpp"

#include "user/training/task.hpp"

struct Walk : public training::Task
{
    Walk() = default;

    explicit
    Walk(pez::core::EntityID id_)
        : Task{id_}
    {}

    void initialize() override
    {

    }

    void update(float dt) override
    {

    }

    [[nodiscard]]
    bool done() const override
    {
        return false;
    }
};