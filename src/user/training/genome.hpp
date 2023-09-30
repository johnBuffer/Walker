#pragma once
#include "engine/engine.hpp"

#include "user/common/configuration.hpp"
#include "user/common/neat/genome.hpp"


struct Genome : public pez::core::Entity
{
    /// Attributes
    float score = 0.0f;
    nt::Genome genome;

    /// Methods
    Genome() = default;

    explicit
    Genome(pez::core::EntityID id_)
        : pez::core::Entity{id_}
        , genome{conf::input_count, conf::output_count}
    {}

    void onRemove() override
    {}

    [[nodiscard]]
    nt::Network generateNetwork()
    {
        return genome.generateNetwork();
    }
};