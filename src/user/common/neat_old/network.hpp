#pragma once
#include <cmath>
#include <vector>
#include <array>
#include <list>
#include <array>
#include "network_info.hpp"


namespace nt
{
struct Utils
{
    // Returns a sigmoid with range [-1, 1]
    static float sigm(float x)
    {
        return 1.0f / (1.0f + exp(-4.9f * x));
    }

    static float soft(float x)
    {
        return 0.5f * (x / (1.0f + std::abs(x)) + 1.0f);
    }
};

template<int N>
using Array = std::array<float, N>;

template<int Inputs, int Outputs>
struct Network
{
    struct Connection
    {
        uint32_t id_to;
        float weight;
        float value;

        Connection() = default;
        Connection(uint32_t to, float w)
            : id_to(to)
            , weight(w)
            , value(0.0f)
        {}
    };

    struct Node
    {
        uint32_t id;
        uint32_t depth;
        float sum;
        float bias;
        uint32_t incoming_count;
        std::vector<Connection> outs;

        Node()
            : depth(0)
            , sum(0.0f)
            , bias(0.0f)
            , incoming_count(0)
        {}

        void reset()
        {
            sum = 0.0f;
        }

        void addValue(float f)
        {
            sum += f;
        }

        [[nodiscard]]
        float getValue() const
        {
            // If input, just send sum
            return incoming_count ? tanh(4.0f * (sum + bias)) : sum;
        }
    };

    uint32_t hidden = 0;
    std::vector<Node> nodes;
    std::vector<uint32_t> order;

    Network() = default;

    explicit
    Network(uint32_t hidden_count)
        : hidden(hidden_count)
    {
        nodes.resize(Inputs + Outputs + hidden_count);
    }

    // Connects two nodes together
    void addConnection(uint32_t from, uint32_t to, float weight)
    {
        Node& node_1 = nodes[from];
        Node& node_2 = nodes[to];
        // Create connection
        node_1.outs.emplace_back(to, weight);
        node_2.incoming_count += 1;
        // Update depth
        if (node_1.depth + 1 > node_2.depth) {
            node_2.depth = node_1.depth + 1;
        }
    }

    void sendValue(uint32_t id)
    {
        const float value = nodes[id].getValue();
        for (Connection& c : nodes[id].outs) {
            c.value = value * c.weight;
            nodes[c.id_to].addValue(c.value);
        }
    }

    void resetOutputs()
    {
        for (Node& node : nodes) {
            node.sum = 0.0f;
        }
    }

    void setOrder(const std::vector<uint32_t>& order_)
    {
        order = order_;
    }

    Array<Outputs> execute(const Array<Inputs>& inputs)
    {
        // Flush outputs
        resetOutputs();
        // Inject inputs
        uint32_t i(0);
        for (float f : inputs) {
            nodes[i++].addValue(f);
        }
        // Execute
        for (uint32_t id : order) {
            sendValue(id);
        }
        Array<Outputs> outputs;
        for (i = 0; i < Outputs; ++i) {
            outputs[i] = nodes[i + Inputs].getValue();
        }
        return outputs;
    }

    [[nodiscard]]
    uint32_t getNodeCount() const
    {
        return Inputs + Outputs + hidden;
    }

    [[nodiscard]]
    uint32_t getConnectionsCount() const
    {
        uint32_t result = 0;

        for (const auto& n : nodes) {
            result += static_cast<uint32_t>(n.outs.size());
        }

        return result;
    }

    [[nodiscard]]
    constexpr uint32_t getInputCount() const
    {
        return Inputs;
    }

    [[nodiscard]]
    constexpr uint32_t getOutputCount() const
    {
        return Outputs;
    }
};
}
