#pragma once
#include "common/physic/creature/verlet_system.hpp"


struct Creature
{
    VerletSystem system;

    float time = 0.0f;

    static constexpr float rest_size = 100.0f;

    std::vector<uint64_t> muscles;
    std::vector<uint64_t> pods;

    Creature() = default;

    explicit
    Creature(Vec2 position, float angle = 0.0f)
    {
        float const a = angle - Math::PI * 0.5f;
        Vec2 const d{cos(a), sin(a)};
        Vec2 const v = 0.5f * rest_size * d;
        Vec2 const n = MathVec2::normal(v);

        addPod(position - n + v);
        addPod(position + n + v);
        addPod(position + n - v);
        addPod(position - n - v);
        addJoint(position);                                                     // 4
        // Muscles
        addMuscle(0, 3);
        addMuscle(1, 2);
        // Bones
        addBone(0, 1);
        addBone(2, 3);
        addBone(0, 4);
        addBone(1, 4);
        addBone(2, 4);
        addBone(3, 4);
    }

    void update(float dt)
    {
        time += dt;
        system.update(dt);
    }

    void setMuscleSize(uint32_t idx, float size)
    {
        getMuscle(idx).target_length = rest_size * (1.0f + 0.5f * size);
    }

    void setPodFriction(uint32_t idx, float friction)
    {
        getPod(idx).friction = std::max(0.1f, friction);
    }

    [[nodiscard]]
    float getPodFriction(uint32_t idx) const
    {
        return getPodConst(idx).friction;
    }

    [[nodiscard]]
    float getMuscleSize(uint32_t idx) const
    {
        auto const& link = getMuscleConst(idx);
        return (link.current_length - link.rest_length) / link.rest_length * 2.0f;
    }

    [[nodiscard]]
    float getLinkSize(uint32_t idx) const
    {
        auto const& link = system.links[idx];
        return (link.current_length - link.rest_length) / link.rest_length * 2.0f;
    }

    [[nodiscard]]
    Vec2 getHeadPosition() const
    {
        return (system.objects[0].position + system.objects[1].position) * 0.5f;
    }

    [[nodiscard]]
    Vec2 getHeadDirection() const
    {
        const auto pos_1 = system.objects[0].position;
        const auto pos_2 = system.objects[1].position;
        return MathVec2::normalize(MathVec2::normal(pos_1 - pos_2));
    }

    void addBone(uint32_t joint_1, uint32_t joint_2)
    {
        const float strength = 0.2f;
        system.links.emplace_back(joint_1, joint_2, system.objects).strength = strength;
    }

    void addMuscle(uint32_t joint_1, uint32_t joint_2)
    {
        const float muscle_strength = 0.05f;
        muscles.push_back(system.links.size());
        auto& muscle = system.links.emplace_back(joint_1, joint_2, system.objects);
        muscle.is_muscle = true;
        muscle.strength  = muscle_strength;
    }

    void addJoint(Vec2 position)
    {
        system.objects.emplace_back(position);
    }

    void addPod(Vec2 position)
    {
        pods.push_back(system.objects.size());
        addJoint(position);
    }

    VerletLink& getMuscle(uint64_t idx)
    {
        return const_cast<VerletLink&>(getMuscleConst(idx));
    }

    [[nodiscard]]
    VerletLink const& getMuscleConst(uint64_t idx) const
    {
        auto const muscle_idx = muscles[idx];
        return system.links[muscle_idx];
    }

    VerletObject& getPod(uint64_t idx)
    {
        return const_cast<VerletObject&>(getPodConst(idx));
    }

    [[nodiscard]]
    VerletObject const& getPodConst(uint64_t idx) const
    {
        auto const pod_idx = pods[idx];
        return system.objects[pod_idx];
    }

    [[nodiscard]]
    uint64_t getJointCount() const
    {
        return system.objects.size();
    }

    [[nodiscard]]
    uint64_t getLinkCount() const
    {
        return system.links.size();
    }

    [[nodiscard]]
    uint64_t getPodsCount() const
    {
        return pods.size();
    }

    [[nodiscard]]
    VerletObject const& getJoint(uint64_t i) const
    {
        return system.objects[i];
    }

    [[nodiscard]]
    VerletLink const& getLink(uint64_t i) const
    {
        return system.links[i];
    }
};

