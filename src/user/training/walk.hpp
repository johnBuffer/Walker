#pragma once
#include "engine/engine.hpp"


struct Walk : public pez::core::Entity
{
    pez::core::ID agent_id  = pez::core::EntityID::INVALID_ID;
    pez::core::ID genome_id = pez::core::EntityID::INVALID_ID;

    Walk() = default;

    explicit
    Walk(pez::core::EntityID id_)
        : pez::core::Entity{id_}
    {
    }

    void update(float dt)
    {

    }
};
