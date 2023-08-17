#pragma once
#include "genome.hpp"
#include "network.hpp"
#include <vector>


namespace nt
{

struct Mutation
{
    enum class Type
    {
        None            = 0,
        NewConnection   = 1,
        NewNode         = 2
    };

    ID         innovation_id = ID{0};
    Type       type          = Type::None;
    ElementRef ref_1;
    ElementRef ref_2;

    Mutation() = default;
    Mutation(Type type_, ID node_1, ID node_2 = ID{0}, ID id_ = ID{0})
        : innovation_id{id_}
        , type{type_}
        , ref_1{0, node_1}
        , ref_2{0, node_2}
    {}

    [[nodiscard]]
    bool operator==(const Mutation& other) const
    {
        return (other.type == type) && (ref_1.id == other.ref_1.id) && (ref_2.id == other.ref_2.id);
    }

    [[nodiscard]]
    bool isValid() const
    {
        return type != Type::None;
    }

    void print() const
    {
        std::cout << "MUT ";
        switch (type) {
            case Type::None:
                std::cout << "None";
                break;
            case Type::NewConnection:
                std::cout << "New connection " << ref_1.id.value << " -> " << ref_2.id.value;
                break;
            case Type::NewNode:
                std::cout << "New node, split " << ref_1.id.value;
                break;
        }
        std::cout << std::endl;
    }
};

template<int Inputs, int Outputs>
struct Mutator
{
    using CompatGenome = Genome<Inputs, Outputs>;

    std::vector<Mutation> history;
    ID                    global_innovation_id = ID{0};
    ID                    global_node_id       = ID{Inputs + Outputs};
    float                 add_connection_proba = 0.8f;
    float                 add_node_proba       = 0.1f;

    void mutate(CompatGenome& genome)
    {
        //std::cout << "Try mutate" << std::endl;
        const auto mutation_type = getRandomMutationType();
        if (mutation_type == Mutation::Type::NewConnection) {
            createRandomConnection(genome);
        } else if (mutation_type == Mutation::Type::NewNode) {
            splitRandomConnection(genome);
        }

        // Sort genome only if mutation occurred
        if (mutation_type != Mutation::Type::None) {
            // Keep genes ordered
            std::sort(genome.genes.begin(), genome.genes.end(), [](const Gene& g_1, const Gene& g_2) {
                return g_1.innovation_id.value < g_2.innovation_id.value;
            });
        }
    }

    void createRandomConnection(CompatGenome& genome)
    {
        const auto ref_1 = genome.pickRandomNeuron(true, false);
        const auto ref_2 = genome.pickRandomNeuron(false, true);
        // std::cout << "Trying new connection " << ref_1.id.value << " -> " << ref_2.id.value << std::endl;
        if (genome.isConnectionValid(ref_1.id, ref_2.id)) {
            const auto id = createNewConnection(ref_1.id, ref_2.id);
            genome.createGene(ref_1.id, ref_2.id, 0.0f, id);
        }
    }

    void splitRandomConnection(CompatGenome& genome)
    {
        if (genome.getConnectionCount() > 0) {
            const auto gene_ref = genome.pickRandomGene();
            if (genome.isGeneActive(gene_ref.index)) {
                const Gene* gene = genome.getGeneByID(gene_ref.id);
                // Create new Node
                Mutation mutation;
                mutation.type = Mutation::Type::NewNode;
                mutation.ref_1 = gene_ref;
                const auto node_id = getInnovationID(mutation);
                // Create new connections
                const ID gene_1_id = createNewConnection(gene->id_from, node_id);
                const ID gene_2_id = createNewConnection(node_id      , gene->id_to);
                genome.splitConnection(gene_ref.index, node_id, gene_1_id, gene_2_id);
            }
        }
    }

    ID getInnovationID(const Mutation& mutation)
    {
        if (!mutation.isValid()) {
            return ID{0};
        }
        const auto id = addNewMutation(mutation);
        //std::cout << "[NEW] ID " << id.value << std::endl;
        //mutation.print();
        return ID{id};
    }

    ID addNewMutation(const Mutation& mutation)
    {
        const auto mutation_id = [&](){
            // If mutation is new connection
            if (mutation.type == Mutation::Type::NewConnection) {
                return global_innovation_id.value++;
            }
            // If mutation is new node
            return global_node_id.value++;
        }();

        history.push_back(mutation);
        history.back().innovation_id.value = mutation_id;
        return ID{mutation_id};
    }

    [[nodiscard]]
    Mutation::Type getRandomMutationType() const
    {
        const float rand_value  = RNGf::getUnder(1.0f);
        if (rand_value < add_connection_proba) {
            return Mutation::Type::NewConnection;
        } else if (rand_value < add_connection_proba + add_node_proba) {
            return  Mutation::Type::NewNode;
        }
        return Mutation::Type::None;
    }

    Mutation createMutation(Mutation::Type type, ID id_1, ID id_2 = ID{0})
    {
        Mutation mutation{type, id_1, id_2};
        mutation.innovation_id = getInnovationID(mutation);
        return mutation;
    }

    ID createNewConnection(ID node_1, ID node_2)
    {
        Mutation mutation;
        mutation.type     = Mutation::Type::NewConnection;
        mutation.ref_1.id = node_1;
        mutation.ref_2.id = node_2;
        mutation.innovation_id = getInnovationID(mutation);
        return mutation.innovation_id;
    }
};

}
