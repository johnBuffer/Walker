#pragma once
#include "genome.hpp"


namespace nn
{
struct Utils
{
    template<int I, int O>
    static void mutateFine(nt::Genome<I, O>& genome)
    {
        if (RNGf::proba(0.7f)) {
            genome.mutateConnectionsFine();
            genome.mutateNodes();
        }
    }

    template<int I, int O>
    static void mutate(nt::Genome<I, O>& genome)
    {
        if (RNGf::proba(0.5f)) {
            genome.mutateNodes();
        } else {
            if (RNGf::proba(0.8f)) {
                genome.mutateConnections();
            } else {
                genome.mutateConnectionsFine();
            }
        }
    }
};
}
