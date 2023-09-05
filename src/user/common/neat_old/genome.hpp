#pragma once
#include <fstream>
#include <cassert>

#include "engine/common/number_generator.hpp"
#include "network.hpp"
#include "graph.hpp"
#include "gene.hpp"
#include "neat_id.hpp"


namespace nt
{

struct ElementRef
{
    uint32_t index = 0;
    ID       id    = ID{0};
};

// Stores a sequence of genes
template<int TInputs, int TOutputs>
struct Genome
{
    uint32_t               hidden = 0;
    std::vector<Gene>      genes;
    Graph                  graph;
    constexpr static float weight_range = 2.0f;

    Genome()
    {
        graph.createNodes(TOutputs + TInputs);
    }

    [[nodiscard]]
    uint64_t getSize() const
    {
        return genes.size();
    }

    [[nodiscard]]
    const Gene& get(uint32_t i) const
    {
        return genes[i];
    }

    [[nodiscard]]
    const Gene* getConstGeneByID(ID id) const
    {
        for (const auto& g : genes) {
            if (g.innovation_id == id) {
                return &g;
            }
        }
        return nullptr;
    }

    [[nodiscard]]
    Gene* getGeneByID(ID id)
    {
        return const_cast<Gene*>(getConstGeneByID(id));
    }

    [[nodiscard]]
    const Gene& rget(uint32_t i) const
    {
        return genes[getSize() - 1 - i];
    }

    void splitConnection(uint32_t gene_idx, ID new_node_id, ID gene_1_id, ID gene_2_id)
    {
        // Add new node
        createNode(new_node_id);
        // Disable old connection
        Gene old_gene = genes[gene_idx];
        deactivateGene(gene_idx);
        // Create new in connection
        createGene(old_gene.id_from, new_node_id, 1.0f, gene_1_id);
        // Create new out connection
        createGene(new_node_id, old_gene.id_to, old_gene.weight, gene_2_id);
        // Update node's parents for rendering
        Graph::Node* new_node = graph.getNode(new_node_id);
        new_node->parent_1 = old_gene.id_from;
        new_node->parent_2 = old_gene.id_to;
    }

    [[nodiscard]]
    bool isGeneActive(uint32_t idx) const
    {
        return genes[idx].active;
    }

    void createNode(ID new_node_id)
    {
        graph.addNode(new_node_id);
        ++hidden;
    }

    void deactivateGene(uint32_t gene_idx)
    {
        graph.disconnect(genes[gene_idx].id_from, genes[gene_idx].id_to);
        genes[gene_idx].active = false;
    }

    void createGene(ID from, ID to, float weight, ID innovation_id)
    {
        // If the gene already exists and is disabled, we re enable it
        if (auto* g = getGeneByID(innovation_id)) {
            // Just to be sure
            if (!(g->id_from == from && g->id_to == to)) {
                //std::cout << "strange" << std::endl;
            }
            g->active = true;
        } else {
            // else we create it
            genes.emplace_back(from, to, weight, innovation_id);
        }
        graph.connect(from, to);
    }

    void initialize()
    {
        for (const Gene& g : genes) {
            expressGene(g);
        }
    }

    void expressGene(const Gene& g)
    {
        if (graph.isConnectionValid(g.id_from, g.id_to)) {
            graph.connect(g.id_from, g.id_to);
            genes.push_back(g);
        }
    }

    [[nodiscard]]
    bool isConnectionValid(ID node_1, ID node_2) const
    {
        return graph.isConnectionValid(node_1, node_2);
    }

    void mutateConnectionsFine()
    {
        const float reduced_range = 0.01f;
        for (Gene& g : genes) {
            const float perturb_proba = 0.1f;
            if (RNGf::proba(perturb_proba)) {
                g.weight += RNGf::getFullRange(reduced_range);
            }
        }
    }

    void mutateConnections()
    {
        const float reduced_range = 0.1f * weight_range;
        for (Gene& g : genes) {
            const float perturb_proba = 0.9f;
            if (RNGf::proba(perturb_proba)) {
                g.weight += RNGf::getFullRange(reduced_range);
            } else {
                g.weight = RNGf::getFullRange(weight_range);
            }
        }
    }

    void mutateNodes()
    {
        for (Graph::Node& node : graph.nodes) {
            if (RNGf::proba(0.1f)) {
                node.bias += RNGf::getFullRange(1.5f * weight_range);
            }
        }
    }

    // Returns a random gene from the network
    [[nodiscard]]
    ElementRef pickRandomNeuron(bool include_inputs, bool include_outputs) const
    {
        const uint32_t count = hidden + include_outputs * TOutputs + include_inputs * TInputs;
        uint32_t picked = RNGu32::getUnder(count - 1);
        if (!include_inputs) {
            picked += TInputs;
        }
        if (!include_outputs) {
            if (picked >= TInputs && picked < (TInputs + TOutputs)) {
                picked += TOutputs;
            }
        }
        return {picked, graph.nodes[picked].id};
    }

    [[nodiscard]]
    uint32_t getConnectionCount() const
    {
        uint32_t count = 0;
        for (const Gene& gene : genes) {
            if (gene.active) {
                count++;
            }
        }
        return count;
    }

    [[nodiscard]]
    ElementRef pickRandomGene() const
    {
        const uint32_t random_idx = RNGu32::getUnder(static_cast<uint32_t>(genes.size())-1);
        return {random_idx, genes[random_idx].innovation_id};
    }

    // Creates the network associated with this genome
    [[nodiscard]]
    Network<TInputs, TOutputs> generateNetwork() const
    {
        Network<TInputs, TOutputs> network(hidden);
        for (const Gene& g : genes) {
            if (g.active) {
                // Convert all IDs to indexes when building the network
                const uint32_t idx_from = graph.getNodeIdx(g.id_from);
                const uint32_t idx_to   = graph.getNodeIdx(g.id_to);
                network.addConnection(idx_from, idx_to, g.weight);
            }
        }

        uint32_t max_depth = 0;
        for (const auto& n : graph.nodes) {
            max_depth = std::max(max_depth, n.depth);
        }
        max_depth += (max_depth == 0);

        uint32_t i(0);
        for (const Graph::Node& node : graph.nodes) {
            network.nodes[i].bias = node.bias;
            network.nodes[i].id   = static_cast<uint32_t>(node.id);
            // If max depth is 0, set output's depth to 1 to separate them from inputs
            if ((i >= TInputs) && (i < TOutputs + TInputs)) {
                network.nodes[i].depth = max_depth;
            } else {
                network.nodes[i].depth = node.depth;
            }
            ++i;
        }

        network.setOrder(graph.getOrder());
        return network;
    }

    void writeToFile(const std::string& filename) const
    {
        std::ofstream outfile(filename, std::istream::out | std::ios::binary);
        const uint64_t connections_count = getConnectionCount();
        const uint32_t inputs  = TInputs;
        const uint32_t outputs = TOutputs;

        outfile.write((char*)&inputs,  sizeof(inputs));
        outfile.write((char*)&outputs, sizeof(outputs));
        outfile.write((char*)&hidden,  sizeof(hidden));
        outfile.write((char*)&connections_count, sizeof(connections_count));
        for (const Gene& g : genes) {
            if (g.active) {
                outfile.write((char*)&g.id_from      , sizeof(g.id_from));
                outfile.write((char*)&g.id_to        , sizeof(g.id_to));
                outfile.write((char*)&g.weight       , sizeof(g.weight));
                outfile.write((char*)&g.innovation_id, sizeof(g.innovation_id));
            }
        }
        for (uint32_t i(0); i<TOutputs + hidden; ++i) {
            const auto& n = graph.nodes[TInputs + i];
            outfile.write((char*)&n.bias, sizeof(n.bias));
            outfile.write((char*)&n.id  , sizeof(n.id));
        }
        outfile.close();
    }

    void loadFromFile(const std::string& filename)
    {
        genes.clear();
        graph.nodes.clear();

        const uint32_t inputs  = TInputs;
        const uint32_t outputs = TOutputs;

        std::ifstream infile(filename, std::ios_base::binary);
        infile.read((char*)&inputs, sizeof(inputs));
        infile.read((char*)&outputs, sizeof(outputs));
        infile.read((char*)&hidden,  sizeof(hidden));

        assert(inputs == TInputs);
        assert(outputs == TOutputs);

        uint64_t connection_count = 0;
        infile.read((char*)&connection_count, sizeof(connection_count));
        graph.createNodes(getNodeCount());

        std::vector<Gene> loaded_genes;
        for (uint64_t i(connection_count); i--;) {
            Gene g{};
            infile.read((char*)&g.id_from,       sizeof(g.id_from));
            infile.read((char*)&g.id_to,         sizeof(g.id_to));
            infile.read((char*)&g.weight,        sizeof(g.weight));
            infile.read((char*)&g.innovation_id, sizeof(g.innovation_id));
            loaded_genes.push_back(g);
        }
        for (uint64_t i(0); i<TOutputs + hidden; ++i) {
            infile.read((char*)&graph.nodes[TInputs + i].bias, sizeof(float));
            infile.read((char*)&graph.nodes[TInputs + i].id  , sizeof(nt::ID));
        }

        for (const auto& g : loaded_genes) {
            createGene(g.id_from, g.id_to, g.weight, g.innovation_id);
        }

        const uint32_t io_count = TInputs + TOutputs;
        for (uint32_t i(io_count); i<getNodeCount(); ++i) {
            graph.nodes[i].parent_1 = graph.nodes[i].ins.front();
            graph.nodes[i].parent_2 = graph.nodes[i].outs.front();
        }
    }

    void printInfo() const
    {
        std::cout << "TInputs: " << TInputs << ", TOutputs: " << TOutputs << ", Hidden: " << hidden << std::endl;
        std::cout << "Connection count: " << getConnectionCount() << std::endl;
    }

    void simplify(float threshold)
    {
        uint32_t removed_links = 0;
        for (Gene& g : genes) {
            if (std::abs(g.weight) < threshold) {
                g.active = false;
                ++removed_links;
            }
        }
        std::cout << "Removed " << removed_links << " links" << std::endl;
    }

    [[nodiscard]]
    uint32_t getNodeCount() const
    {
        return hidden + TInputs + TOutputs;
    }

    [[nodiscard]]
    uint32_t getInput(uint32_t i) const
    {
        return i;
    }

    [[nodiscard]]
    uint32_t getOutput(uint32_t i) const
    {
        return i + TInputs;
    }

    void generateConnections()
    {
        for (uint32_t input{0}; input < TInputs; ++input) {
            for (uint32_t output{0}; output < TOutputs; ++output) {
                createGene(nt::ID(input), nt::ID(output + TInputs), RNGf::getFullRange(2.0f), nt::ID(0));
            }
        }
    }
};
}
