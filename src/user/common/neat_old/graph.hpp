#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include "neat_id.hpp"


struct Graph
{
    enum class Mark : uint8_t
    {
        None = 0,
        Temp = 1,
        Perm = 2
    };

	struct Node
	{
		nt::ID id;
		uint32_t  depth;
		std::vector<nt::ID> ins;
		std::vector<nt::ID> outs;
        float  bias     = 0.0f;
        nt::ID parent_1 = nt::ID{0};
        nt::ID parent_2 = nt::ID{0};
        Mark   mark     = Mark::None;

        explicit
		Node(nt::ID id_)
			: id(id_)
			, depth(0)
		{}

        /// Checks that the node has no incoming connections
        [[nodiscard]]
        bool isStart() const
        {
            return ins.empty();
        }
	};

	std::vector<Node> nodes;
    std::list<nt::ID> order;

	void addNode(nt::ID node_id)
	{
		nodes.emplace_back(node_id);
	}

	void createNodes(uint32_t count)
	{
		for (uint32_t i(0); i < count; ++i) {
			addNode(nt::ID{i});
		}
	}

    [[nodiscard]]
	bool isConnected(nt::ID from, nt::ID to) const
	{
        const Node* node = getConstNode(from);
        if (!node) {
            return false;
        }
        return std::any_of(node->outs.begin(), node->outs.end(), [to](nt::ID id) { return id == to; });
	}

    [[nodiscard]]
    const Node* getConstNode(nt::ID id) const
    {
        for (const Node& n : nodes) {
            if (n.id == id) {
                return &n;
            }
        }
        return nullptr;
    }

    Node* getNode(nt::ID id)
    {
        return const_cast<Node*>(getConstNode(id));
    }

    [[nodiscard]]
	bool isAncestor(nt::ID node_1, nt::ID node_2) const
	{
		if (node_1 == node_2) {
			return true;
        }
        const Node* n = getConstNode(node_1);
        return std::any_of(n->outs.begin(), n->outs.end(), [&](nt::ID id) { return isAncestor(id, node_2); });
	}

    [[nodiscard]]
	bool isConnectionValid(nt::ID from, nt::ID to) const
	{
		return !isAncestor(to, from) && !isConnected(from, to);
	}

	void connect(nt::ID from, nt::ID to)
	{
		if (isConnectionValid(from, to)) {
            getNode(to)->ins.push_back(from);
            getNode(from)->outs.push_back(to);
			computeDepths();
		}
		else {
			//std::cout << "Invalid connection: " << from.value << " -> " << to.value << std::endl;
		}
	}

	void disconnect(nt::ID from, nt::ID to)
	{
		auto& outs = getNode(from)->outs;
		outs.erase(std::remove(outs.begin(), outs.end(), to), outs.end());

		auto& ins = getNode(to)->ins;
		ins.erase(std::remove(ins.begin(), ins.end(), from), ins.end());
        computeDepths();
	}

    void computeDepths()
    {
        resetNodes();
        while (auto n = getUnvisitedNode()) {
            visitNode(*n);
        }

        // Iterate on all nodes and update depths
        for (const nt::ID o : order) {
            auto& n = *getNode(o);
            // std::cout << "Processing " << o.value << std::endl;
            // If no incoming connection (meaning no dependency) depth is 0
            if (n.ins.empty()) {
                n.depth = 0;
            } else {
                n.depth = 0;
                // Else the node should be processed after all its parents
                // meaning that its depth is equal to its deepest parent + 1
                // std::cout << "INs: ";
                for (nt::ID i : n.ins) {
                    // std::cout << i.value << " (" << getNode(i)->depth << ") ";
                    n.depth = std::max(n.depth, getNode(i)->depth + 1);
                }
                // std::cout << std::endl;
            }
        }
    }

    Node* getUnvisitedNode()
    {
        for (auto& n : nodes) {
            if (n.mark == Mark::None) {
                return &n;
            }
        }
        return nullptr;
    }

    void resetNodes()
    {
        order.clear();
        for (auto& n : nodes) {
            n.mark = Mark::None;
        }
    }

    void visitNode(Node& n)
    {
        // If already visited, don't visit again
        if (n.mark == Mark::Perm) {
            return;
        }
        // If has a temp mark we are in a cycle, abort
        if (n.mark == Mark::Temp) {
            std::cout << "Cycle detected, aborting." << std::endl;
            return;
        }
        // Mark the node with temp mark to detect cycles
        n.mark = Mark::Temp;
        // Visit all children of current node
        for (nt::ID o : n.outs) {
            visitNode(*getNode(o));
        }
        // Mark current node as visited and add it to the front of the list
        n.mark = Mark::Perm;
        order.push_front(n.id);
    }

    [[nodiscard]]
    uint32_t getNodeIdx(nt::ID id) const
    {
        uint32_t i{0};
        for (const auto& n : nodes) {
            if (n.id == id) {
                return i;
            }
            ++i;
        }
        // Should not happen
        return 0;
    }

    [[nodiscard]]
	std::vector<uint32_t> getOrder() const
	{
		std::vector<uint32_t> result(nodes.size());
        uint32_t i{0};
        for (nt::ID o : order) {
            result[i++] = getNodeIdx(o);
        }
		return result;
	}
};
