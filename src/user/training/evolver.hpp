#pragma once
#include "engine/common/utils.hpp"

#include "./selector.hpp"
#include "user/common/neat_old/mutator.hpp"
#include "user/common/neat_old/nn_utils.hpp"
#include "user/training/genome.hpp"


struct Evolver
{
    using GenomeVector = std::vector<Genome>;

    TrainingState& state;

    Selector selector;
    nt::Mutator<conf::input_count, conf::output_count> mutator;

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

    void createNewGeneration()
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
            for (const auto& g: old_generation) {
                selector.addEntry(i, g.score);
                ++i;
            }
        }
        selector.normalizeEntries();

        // Create new genomes
        while (new_generation.size() < conf::population_size) {
            const uint32_t genome_idx = selector.pick();
            new_generation.push_back(old_generation[genome_idx]);
            auto& new_genome = new_generation.back();
            // Mutate topology
            mutator.mutate(new_genome.genome);
            // Mutate weights
            nn::Utils::mutate(new_genome.genome);
        }
    }

    void fetchOldPopulation()
    {
        uint32_t i{0};
        pez::core::foreach<Genome>([&](Genome& genome) {
            old_generation[i] = genome;
            ++i;
        });
    }

    void updatePopulation()
    {
        uint32_t i{0};
        pez::core::foreach<Genome>([&](Genome& genome) {
            genome.genome = new_generation[i].genome;
            ++i;
        });
    }
};