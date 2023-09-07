#pragma once
#include "user/common/neat_old/genome.hpp"
#include "engine/common/racc.hpp"


template<int TInputs, int TOutputs>
struct GeneticInfo
{
    using CompatGenome  = nt::Genome<TInputs, TOutputs>;
    using CompatNetwork = nt::Network<TInputs, TOutputs>;

    CompatGenome  genome;
    CompatNetwork network;

    float        score         = 0.0f;
    RMean<float> genetic_score = 0.0f;

    GeneticInfo()
        : genetic_score{10}
    {
        reset();
    }

    void reset()
    {
        score   = 0.0f;
        network = genome.generateNetwork();
    }

    void updateGeneticScore()
    {
        genetic_score.addValue(score);
    }
};
