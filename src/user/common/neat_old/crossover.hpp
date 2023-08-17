#pragma once
#include "genome.hpp"


namespace nt
{
template<int TInputs, int TOutputs>
struct Crossover
{
    using CompatibleGenome = Genome<TInputs, TOutputs>;

    CompatibleGenome apply()
    {

    }

    uint32_t getExcessCount(const CompatibleGenome& genome_1, const CompatibleGenome& genome_2) const
    {
        const uint32_t max_innovation_1 = genome_1.genes.back().innovation_id;
        const uint32_t max_innovation_2 = genome_2.genes.back().innovation_id;
    }

    uint32_t getDisjointCount(const CompatibleGenome& genome_1, const CompatibleGenome& genome_2) const
    {

    }
};
}