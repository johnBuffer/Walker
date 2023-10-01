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
    {
        resetGenome();
    }

    void onRemove() override
    {}

    [[nodiscard]]
    nt::Network generateNetwork()
    {
        return genome.generateNetwork();
    }

    void resetGenome()
    {
        genome = nt::Genome{conf::input_count, conf::output_count};
    }
};