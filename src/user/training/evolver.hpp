#pragma once
#include "engine/common/utils.hpp"

#include "./selector.hpp"
#include "user/common/neat_old/mutator.hpp"
#include "user/training/genome.hpp"


template<int TInputs, int TOutputs>
struct Evolver
{
    using CompatGenome = nt::Genome<TInputs, TOutputs>;
    using GenomeVector = std::vector<CompatGenome>;

    TrainingState& state;

    Selector selector;
    nt::Mutator<TInputs, TOutputs> mutator;

    GenomeVector old_generation;
    GenomeVector new_generation;

    Evolver()
        : state{pez::core::getSingleton<TrainingState>()}
    {
        old_generation.resize(conf::population_size);
        new_generation.resize(conf::population_size);
    }

    void reset()
    {
        mutator = {};
    }

    GenomeVector createNewGeneration(const GenomeVector& current_genomes)
    {
        fetchOldPopulation();
        new_generation.clear();
        selector.clear();

        std::sort(old_generation.begin(), old_generation.end(), [](const Genome& g1, const Genome& g2) {
            return g1.score > g2.score;
        });

        std::cout << "Iteration best: " << old_generation[0].score << std::endl;

        // Keep elite
        const auto elite_count = to<uint32_t>(conf::elite_ratio * to<float>(conf::population_size));
        for (uint32_t i{0}; i < elite_count; ++i) {
            new_generation.push_back(old_generation[i]);
        }

        {
            uint32_t i{0};
            for (const auto& g: genomes_copy) {
                selector.addEntry(i, g.score);
                ++i;
            }
        }
        selector.normalizeEntries();

        // Create new genomes
        while (new_genomes.size() < population_size) {
            const uint32_t genome_idx = selector.pick();
            new_genomes.push_back(genomes_copy[genome_idx]);
            auto& new_gen = new_genomes.back();
            // Mutate topology
            mutator.mutate(new_gen.genome);
            // Mutate weights
            nn::Utils::mutate(new_gen.genome);
        }

        return new_genomes;
    }

    void fetchOldPopulation()
    {
        uint32_t i{0};
        pez::core::foreach<Genome>([&](Genome& genome) {
            old_generation[i] = genome;
            ++i;
        });
    }
};