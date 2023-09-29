#pragma once
#include <vector>

#include "dag.hpp"
#include "network.hpp"


namespace nt
{
/// Blueprint a network
struct Genome
{
public: // Internal structs
    /// Represents a node of the network
    struct Node
    {
        float      bias       = 0.0f;
        Activation activation = Activation::Sigm;
    };

    /// Represents a connection between two nodes
    struct Connection
    {
        uint32_t from   = 0;
        uint32_t to     = 0;
        float    weight = 0.0f;
    };

public: // Attributes
    /// Info
    nt::Network::Info       info;
    /// The nodes of the network
    std::vector<Node>       nodes;
    /// The connections
    std::vector<Connection> connections;
    /// A graph to create valid connections
    DAG                     graph;

public: // Methods
    Genome() = default;

    explicit
    Genome(nt::Network::Info const& info_)
        : info{info_}
    {
        // Create inputs
        for (uint32_t i{info.inputs + info.outputs}; i--;) {
            createNode(Activation::None, false);
        }
        // Create outputs
        for (uint32_t i{info.inputs + info.outputs}; i--;) {
            createNode(Activation::Sigm, false);
        }
    }

    uint32_t createNode(Activation activation, bool hidden = true)
    {
        nodes.emplace_back();
        nodes.back().activation = activation;
        nodes.back().bias       = 0.0f;

        graph.createNode();
        // Update info if needed
        if (hidden) {
            ++info.hidden;
        }
        // Return index of new node
        return static_cast<uint32_t>(nodes.size() - 1);
    }

    bool tryCreateConnection(uint32_t from, uint32_t to, float weight)
    {
        if (graph.createConnection(from, to)) {
            createConnection(from, to, weight);
            return true;
        }
        return false;
    }

    void createConnection(uint32_t from, uint32_t to, float weight)
    {
        connections.push_back({from, to, weight});
    }

    void splitConnection(uint32_t i)
    {
        if (i > connections.size()) {
            std::cout << "Invalid connection " << i << std::endl;
        }

        Connection const& c      = connections[i];
        uint32_t const    from   = c.from;
        uint32_t const    to     = c.to;
        float const       weight = c.weight;
        removeConnection(i);

        uint32_t const node_idx{createNode(Activation::Relu)};
        createConnection(from, node_idx, weight);
        createConnection(node_idx, to, 1.0f);
    }

    void removeConnection(uint32_t i)
    {
        std::swap(connections[i], connections.back());
        connections.pop_back();
    }

    [[nodiscard]]
    nt::Network generateNetwork() const
    {
        nt::Network network;
        network.initialize(info, static_cast<uint32_t>(connections.size()));

        uint32_t   conn_idx{0};
        auto const node_count = static_cast<uint32_t>(nodes.size());
        for (uint32_t node_idx{0}; node_idx < node_count; ++node_idx) {
            // Initialize node
            network.setNode(node_idx, nodes[node_idx].activation, nodes[node_idx].bias);

            // Create its connections
            for (auto const& c : connections) {
                if (c.from == node_idx) {
                    network.setConnection(conn_idx, c.to, c.weight);
                    ++conn_idx;
                }
            }
        }

        return network;
    }


};
}