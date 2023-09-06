#pragma once
#include "engine/engine.hpp"


struct Agent : public pez::core::Entity
{
    /// Attributes
    pez::core::EntityID genome_id;

    /// Methods
    Agent() = default;

    explicit
    Agent(pez::core::EntityID id_)
        : pez::core::Entity{id_}
    {}
};
