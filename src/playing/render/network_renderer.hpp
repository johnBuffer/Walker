#pragma once

#include <vector>

#include "engine/common/utils.hpp"
#include "common/neat_old/network.hpp"


struct NetworkRenderer
{
    Vec2  const node_spacing = {10.0f, 16.0f};
    float const node_radius  = 12.0f;

    struct DrawableNode
    {
        sf::Vector2f position;
        uint32_t     layer    = 0;
        SmoothFloat  value;

        DrawableNode()
        {
            value.setSpeed(10.0f);
            value.setInterpolationFunction(Interpolation::Linear);
            value.setValueInstant(0.0f);
        }
    };

    struct DrawableConnection
    {
        sf::Vector2f start;
        sf::Vector2f end;
        SmoothFloat  width;

        DrawableConnection()
        {
            width.setSpeed(5.0f);
            width.setInterpolationFunction(Interpolation::Linear);
        }
    };

    float margin = node_radius + 5.0f;
    nt::Network<9, 6> const* network{nullptr};
    std::vector<DrawableNode>       nodes;
    std::vector<DrawableConnection> connections;

    Vec2 size     = {};
    Vec2 position = {};

    sf::VertexArray connections_va;

    void initialize(nt::Network<9, 6> const& nw)
    {
        network = &nw;
        nodes.clear();
        connections.clear();

        // Create nodes
        std::vector<uint32_t> layers(getMaxDepth() + 1, 0);
        size.x = static_cast<float>(getMaxDepth() + 1) * (node_radius * 2.0f + node_spacing.x) - node_spacing.x + node_radius * 0.5f + 2.0f;
        for (auto const& n : nw.nodes) {
            auto& node = nodes.emplace_back();
            node.position.x = n.depth * (node_radius * 2.0f + node_spacing.x);
            node.position.y = layers[n.depth] * (node_radius * 2.0f + node_spacing.y);
            node.layer      = n.depth;
            ++layers[n.depth];
        }

        // Center layers
        auto const  layer_size = getMax<uint32_t>(layers, [](uint32_t x) {return x;});
        float const max_layer_height = getLayerHeight(layer_size) + 4.0f;
        size.y = max_layer_height;
        for (auto& n : nodes) {
            float const node_layer_height = getLayerHeight(layers[n.layer]);
            float const offset            = (max_layer_height - node_layer_height) * 0.5f;
            n.position.y += offset;
            n.position += sf::Vector2f{margin, margin};
        }

        // Create connections
        {
            uint32_t i{0};
            for (auto const &n: nw.nodes) {
                if (n.outs.size()) {
                    for (auto const &connection: n.outs) {
                        auto &c = connections.emplace_back();
                        c.start = nodes[i].position;
                        c.end = nodes[connection.id_to].position;
                    }
                }
                ++i;
            }
        }

        {
            connections_va = sf::VertexArray(sf::Quads, 4 * connections.size());
            uint32_t i{0};
            for (auto const& c: connections) {
                Utils::generateLine(connections_va, 4 * i, c.start, c.end, 2.0f, sf::Color::White);
                ++i;
            }
        }
    }

    void render(RenderContext& context)
    {
        sf::Transform transform;
        transform.translate(position);

        context.drawDirect(connections_va, transform);

        float const out_radius = node_radius + 3.0f;
        sf::CircleShape shape{out_radius};
        shape.setOrigin(out_radius, out_radius);
        shape.setOutlineColor(sf::Color::White);
        shape.setFillColor(sf::Color::Black);
        shape.setOutlineThickness(2.0f);
        for (auto const& n : nodes) {
            shape.setPosition(n.position);
            context.drawDirect(shape, transform);

            float const radius = std::min(n.value.get(), 1.0f) * node_radius;
            sf::CircleShape shape_in{radius};
            shape_in.setOrigin(radius, radius);
            sf::Color const color = (n.value.get() > 0.0f) ? sf::Color{188, 226, 158} : sf::Color{255, 135, 135};
            shape_in.setFillColor(color);
            shape_in.setPosition(n.position);

            context.drawDirect(shape_in, transform);
        }
    }

    void update()
    {
        {
            uint32_t i{0};
            for (auto const &n: network->nodes) {
                for (auto const &connection: n.outs) {
                    auto &c = connections[i];

                    c.width = connection.value * 20.0f;
                    float const sign = Math::sign(c.width.get());
                    float const width = std::max(1.0f, std::min(node_radius, std::abs(c.width.get())));

                    sf::Color const color = (sign > 0.0f) ? sf::Color{188, 226, 158} : sf::Color{255, 135, 135};
                    Utils::generateLine(connections_va, 4 * i, c.start, c.end, width, color);
                    ++i;
                }
            }
        }

        {
            uint32_t i{0};
            for (auto const& n : network->nodes) {
                nodes[i].value = n.getValue();
                ++i;
            }
        }
    }

    // Utils //////////////////////////

    [[nodiscard]]
    uint32_t getMaxDepth() const
    {
        return getMax<uint32_t>(network->nodes, [](auto const& n) {return n.depth;});
    }

    [[nodiscard]]
    float getLayerHeight(uint32_t height) const
    {
        return height * (2.0f * node_radius + node_spacing.y) - node_spacing.y + 0.5f * node_radius;
    }
};
