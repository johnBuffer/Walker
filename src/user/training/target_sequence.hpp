#pragma once
#include "engine/engine.hpp"
#include "engine/common/number_generator.hpp"


struct TargetSequence : pez::core::Entity
{
    // The current target sequence
    std::vector<Vec2> targets;

    explicit
    TargetSequence(pez::core::EntityID id_)
        : pez::core::Entity{id_}
    {
        generateNewTargets();
    }

    [[nodiscard]]
    Vec2 getTarget(uint32_t i) const
    {
        return targets[i];
    }

    void generateNewTargets()
    {
        uint32_t const targets_count = 1000;
        targets.clear();
        targets.reserve(targets_count);
        for (uint32_t i{targets_count}; i--;) {
            targets.push_back(generateOneTarget());
        }

        targets[0].y = conf::maximum_distance * 0.25f;
    }

    /**
     * Generates a random target contained in a circle of radius conf::maximum_distance / 2 and centered in a square
     * with side size of conf::maximum radius
     */
    static Vec2 generateOneTarget()
    {
        float const radius = 0.5f * conf::maximum_distance;
        float const angle = RNGf::getUnder(Math::TwoPI);
        float const dist  = sqrt(RNGf::getUnder(radius * radius));
        return Vec2{radius, radius} + dist * Vec2{cos(angle), sin(angle)};
    }
};
