#pragma once
#include <vector>

#include "activation.hpp"


namespace nt
{
struct Network
{

public: // Internal structs
    struct Info
    {
        uint32_t inputs  = 0;
        uint32_t outputs = 0;
        uint32_t hidden  = 0;

        [[nodiscard]]
        uint32_t getNodeCount() const
        {
            return inputs + hidden + outputs;
        }
    };

    struct Node
    {
        ActivationPtr activation       = ActivationFunction::none;
        float         sum              = 0.0f;
        float         bias             = 0.0f;
        uint32_t      connection_count = 0;
        uint32_t      depth            = 0;

        [[nodiscard]]
        float getValue() const
        {
            return activation(sum + bias);
        }
    };

    struct Connection
    {
        uint32_t to     = 0;
        float    weight = 0.0f;
        float    value  = 0.0f;
    };

    union Slot
    {
        Node       node;
        Connection connection;
    };

public: // Attributes
    std::vector<Slot>     slots;
    std::vector<uint32_t> order;
    std::vector<float>    output;

    Info     info;
    uint32_t connection_count = 0;

public: // Methods
    Network() = default;

    /// Initializes the slots vector
    void initialize(Info const& info_, uint32_t connection_count_)
    {
        info             = info_;
        connection_count = connection_count_;

        slots.reserve(info.getNodeCount() + connection_count);
        output.resize(info.outputs);
    }

    void setOrder(std::vector<uint32_t>& order_)
    {
        order = order_;
    }

    void setNode(uint32_t i, Activation activation, float bias)
    {
        slots[i].node.activation = ActivationFunction::getFunction(activation);
        slots[i].node.bias       = bias;
    }

    void setConnection(uint32_t i, uint32_t to, float weight)
    {
        Connection& connection = getConnection(i);
        connection.to     = to;
        connection.weight = weight;
    }

    [[nodiscard]]
    Connection const& getConnection(uint32_t i) const
    {
        return slots[info.getNodeCount() + i].connection;
    }

    Connection& getConnection(uint32_t i)
    {
        return slots[info.getNodeCount() + i].connection;
    }

    Node& getOutput(uint32_t i)
    {
        return slots[info.inputs + i].node;
    }

    bool execute(std::vector<float> const& input)
    {
        // Check compatibility
        if (input.size() != info.inputs) {
            std::cout << "Input size mismatch, aborting" << std::endl;
            return false;
        }

        // Initialize input
        for (uint32_t i{0}; i < info.inputs; ++i) {
            slots[i].node.sum = input[i];
        }

        // Execute network
        uint32_t current_connection = 0;
        for (uint32_t i : order) {
            Node const& node  = slots[i].node;
            float const value = node.getValue();
            for (uint32_t o{0}; o < node.connection_count; ++o) {
                Connection const& c = getConnection(current_connection++);
                slots[c.to].node.sum += value * c.weight;
            }
        }

        // Update output
        for (uint32_t i{0}; i < info.outputs; ++i) {
            output[i] = getOutput(i).getValue();
        }

        return true;
    }

    [[nodiscard]]
    std::vector<float> const& getResult() const
    {
        return output;
    }
};
}
