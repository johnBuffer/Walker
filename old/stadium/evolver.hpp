#pragma once
#include "./selector.hpp"
#include "./genetic_info.hpp"


template<int TInputs, int TOutputs>
struct Evolver
{
    using Genetic      = GeneticInfo<TInputs, TOutputs>;
    using GenomeVector = std::vector<Genetic>;

    struct HallOfFame
    {
        GenomeVector best;
        uint32_t max_size;

        explicit
        HallOfFame(uint32_t count)
            : max_size{count}
        {}

        void addGenome(Genetic const& g)
        {
            if (best.empty()) {
                best.push_back(g);
            } else {
                for (auto it{best.begin()}; it != best.end(); it++) {
                    if (it->genetic_score.get() < g.genetic_score.get()) {
                        std::cout << "New best" << std::endl;
                        best.insert(it, g);
                        break;
                    }
                }
            }

            if (best.size() > max_size) {
                best.resize(max_size);
            }
        }
    };

    HallOfFame best;
    Selector selector;
    nt::Mutator<TInputs, TOutputs> mutator;

    uint32_t iteration = 0;

    const float elite_ratio = 0.2f;

    Evolver()
        : best(10)
    {}

    void reset()
    {
        mutator   = {};
        iteration = 0;
    }

    GenomeVector createNewGeneration(const GenomeVector& current_genomes)
    {
        ++iteration;
        const size_t population_size{current_genomes.size()};
        selector.clear();

        GenomeVector genomes_copy = current_genomes;
        GenomeVector new_genomes;

        std::sort(genomes_copy.begin(), genomes_copy.end(), [](const Genetic& g1, const Genetic& g2) {
            return g1.genetic_score.get() > g2.genetic_score.get();
        });

        std::cout << "Iteration best: " << genomes_copy[0].score << std::endl;
        best.addGenome(genomes_copy[0]);

        // Keep elite
        const auto  elite_count = to<uint32_t>(elite_ratio * to<float>(population_size));
        for (uint32_t i{0}; i < elite_count; ++i) {
            new_genomes.push_back(genomes_copy[i]);
        }

        // Reintroduce the best
        for (auto const& g : best.best) {
            //new_genomes.push_back(g);
        }

        // Discard bottom half and create new population
        genomes_copy.resize(population_size / 2);

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
};