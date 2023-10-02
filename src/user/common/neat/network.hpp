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

        Info() = default;

        Info(uint32_t inputs_, uint32_t outputs_)
            : inputs{inputs_}
            , outputs{outputs_}
            , hidden{0}
        {}

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
        // By default, slot is initialized as a Node, just to allow resizing
        Node       node       = {};
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

        // Resize doesn't work because Node has a "non-trivial constructor"
        slots.resize(info.getNodeCount() + connection_count);
        output.resize(info.outputs);
    }

    void setOrder(std::vector<uint32_t> const& order_)
    {
        order = order_;
    }

    void setNode(uint32_t i, Activation activation, float bias, uint32_t connection_count_)
    {
        getNode(i).activation       = ActivationFunction::getFunction(activation);
        getNode(i).bias             = bias;
        getNode(i).connection_count = connection_count_;
    }

    void setNodeDepth(uint32_t i, uint32_t depth)
    {
        slots[i].node.depth = depth;
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

    Node& getNode(uint32_t i)
    {
        return slots[i].node;
    }

    [[nodiscard]]
    Node const& getNode(uint32_t i) const
    {
        return slots[i].node;
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

        // Reset nodes
        foreachNode([](Node& n, uint32_t) {
            n.sum = 0.0f;
        });

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
                Connection& c = getConnection(current_connection++);
                c.value = value * c.weight;
                getNode(c.to).sum += c.value;
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

    template<typename TCallback>
    void foreachNode(TCallback&& callback)
    {
        uint32_t const node_count = info.getNodeCount();
        for (uint32_t i{0}; i < node_count; ++i) {
            callback(slots[i].node, i);
        }
    }

    template<typename TCallback>
    void foreachNode(TCallback&& callback) const
    {
        uint32_t const node_count = info.getNodeCount();
        for (uint32_t i{0}; i < node_count; ++i) {
            callback(slots[i].node, i);
        }
    }

    template<typename TCallback>
    void foreachConnection(TCallback&& callback) const
    {
        for (uint32_t i{0}; i < connection_count; ++i) {
            callback(getConnection(i), i);
        }
    }

    /// Returns the depth of the network
    [[nodiscard]]
    uint32_t getDepth() const
    {
        return slots[order.back()].node.depth;
    }
};
}
