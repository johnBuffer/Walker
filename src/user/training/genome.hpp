#pragma once
#include "engine/engine.hpp"

#include "user/common/configuration.hpp"
#include "user/common/neat_old/genome.hpp"


struct Genome : public pez::core::Entity
{
    /// Attributes
    float score = 0.0f;
    nt::Genome<conf::input_count, conf::output_count> genome;

    /// Methods
    Genome() = default;

    explicit
    Genome(pez::core::EntityID id_)
        : pez::core::Entity{id_}
    {}

    void onRemove() override
    {}

    [[nodiscard]]
    nt::Network<conf::input_count, conf::output_count> generateNetwork() const
    {
        return genome.generateNetwork();
    }
};