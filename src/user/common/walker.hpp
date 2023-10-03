#pragma once
#include "user/common/physic/verlet_system.hpp"

struct Muscle
{
    uint64_t link_idx          = 0;
    float    rest_size         = 0.0f;
    float    contraction_ratio = 0.0f;
    float    extension_ratio   = 0.0f;
    float    current_ratio     = 0.0f;
    float    target_ratio      = 0.0f;
    float    speed             = 8.0f;

    Muscle() = default;
    Muscle(uint64_t idx, float size, float contraction, float extension)
        : link_idx{idx}
        , rest_size{size}
        , contraction_ratio{contraction}
        , extension_ratio{extension}
    {}

    void update(VerletLink& link, float dt)
    {
        current_ratio += (target_ratio - current_ratio) * speed * dt;
        link.target_length = getCurrentSize();
    }

    [[nodiscard]]
    float getTargetSize() const
    {
        if (target_ratio > 0.0f) {
            return rest_size * (1.0f + extension_ratio * target_ratio);
        } else {
            return rest_size * (1.0f + contraction_ratio * target_ratio);
        }
    }

    [[nodiscard]]
    float getCurrentSize() const
    {
        if (target_ratio > 0.0f) {
            return rest_size * (1.0f + extension_ratio * current_ratio);
        } else {
            return rest_size * (1.0f + contraction_ratio * current_ratio);
        }
    }

    [[nodiscard]]
    float getCurrentRatio(VerletLink const& link) const
    {
        float const delta = link.current_length - rest_size;
        if (delta < 0.0f) {
            float const contraction_size = rest_size * contraction_ratio;
            return delta / contraction_size;
        }
        float const extension_size = rest_size * extension_ratio;
        return delta / extension_size;
    }
};

struct Pod
{
    uint64_t object_idx       = 0;
    float    speed            = 30.0f;
    float    current_friction = 0.0f;
    float    target_friction  = 0.0f;

    explicit
    Pod(uint64_t idx)
        : object_idx{idx}
    {}

    void update(VerletObject& object, float dt)
    {
        current_friction += (target_friction - current_friction) * speed * dt;
        object.friction = current_friction;
    }

    [[nodiscard]]
    float getFriction() const
    {
        return current_friction;
    }
};

struct Walker
{
    struct Info
    {
        size_t pod_count    = 0;
        size_t muscle_count = 0;
    };

    VerletSystem system;

    float time = 0.0f;

    std::vector<Muscle> muscles;
    std::vector<Pod>    pods;

    Walker() = default;

    explicit
    Walker(Vec2 position)
    {
        float const base = 50.0f;
        float const core_ratio = 0.7f;
        // Joints
        addJoint({position.x - core_ratio * base, position.y - core_ratio * base}); // 0
        addJoint({position.x + core_ratio * base, position.y - core_ratio * base}); // 1
        addJoint({position.x + core_ratio * base, position.y + core_ratio * base}); // 2
        addJoint({position.x - core_ratio * base, position.y + core_ratio * base}); // 3
        // Bones
        addBone(0, 1);
        addBone(1, 2);
        addBone(2, 3);
        addBone(3, 0);
        addBone(0, 2);
        addBone(1, 3);

        // Pods
        addPod({position.x - 2.0f * base, position.y - base}); // 4
        addPod({position.x + 2.0f * base, position.y - base}); // 5
        addPod({position.x + 2.0f * base, position.y + base}); // 6
        addPod({position.x - 2.0f * base, position.y + base}); // 7

        // Muscles
        float const ratio = 0.15f;
        addMuscle(0, 4, ratio, ratio);
        addMuscle(3, 4, ratio, ratio);
        addMuscle(1, 5, ratio, ratio);
        addMuscle(2, 5, ratio, ratio);
        addMuscle(1, 6, ratio, ratio);
        addMuscle(2, 6, ratio, ratio);
        addMuscle(0, 7, ratio, ratio);
        addMuscle(3, 7, ratio, ratio);
    }

    void update(float dt)
    {
        time += dt;
        for (auto& m : muscles) {
            m.update(system.links[m.link_idx], dt);
        }
        for (auto& p : pods) {
            p.update(system.objects[p.object_idx], dt);
        }
        system.update(dt);
    }

    void setMuscleRatio(uint32_t idx, float size)
    {
        muscles[idx].target_ratio = size;
    }

    void setPodFriction(uint32_t idx, float friction)
    {
        //getPod(idx).friction = std::max(0.1f, friction);
        pods[idx].target_friction = std::max(0.1f, friction);
    }

    [[nodiscard]]
    float getPodFriction(uint32_t idx) const
    {
        return pods[idx].getFriction();
    }

    [[nodiscard]]
    float getMuscleRatio(uint32_t idx) const
    {
        return muscles[idx].getCurrentRatio(system.links[muscles[idx].link_idx]);
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
        const float strength = 0.5f;
        system.links.emplace_back(joint_1, joint_2, system.objects).strength = strength;
    }

    void addMuscle(uint32_t joint_1, uint32_t joint_2, float contraction, float extension)
    {
        const float muscle_strength = 0.5f;

        auto& muscle = system.links.emplace_back(joint_1, joint_2, system.objects);
        muscle.is_muscle = true;
        muscle.strength  = muscle_strength;

        muscles.emplace_back(system.links.size() - 1, muscle.target_length, contraction, extension);
    }

    void addJoint(Vec2 position, float mass = 1.0f)
    {
        auto& joint = system.objects.emplace_back(position);
        joint.setMass(mass);
    }

    void addPod(Vec2 position)
    {
        pods.emplace_back(system.objects.size());
        addJoint(position, 10.0f);
    }

    VerletLink& getMuscle(uint64_t idx)
    {
        return const_cast<VerletLink&>(getMuscleConst(idx));
    }

    [[nodiscard]]
    VerletLink const& getMuscleConst(uint64_t idx) const
    {
        auto const& muscle = muscles[idx];
        return system.links[muscle.link_idx];
    }

    VerletObject& getPod(uint64_t idx)
    {
        return const_cast<VerletObject&>(getPodConst(idx));
    }

    [[nodiscard]]
    VerletObject const& getPodConst(uint64_t idx) const
    {
        auto const& pod = pods[idx];
        return system.objects[pod.object_idx];
    }

    void moveTo(Vec2 position)
    {
        Vec2 const to_pos = position - getHeadPosition();
        for (auto& o : system.objects) {
            o.position      += to_pos;
            o.position_last += to_pos;
        }
    }

    [[nodiscard]]
    uint64_t getLinkCount() const
    {
        return system.links.size();
    }

    [[nodiscard]]
    uint64_t getPodCount() const
    {
        return pods.size();
    }

    [[nodiscard]]
    VerletObject const& getJoint(uint32_t idx) const
    {
        return system.objects[idx];
    }

    [[nodiscard]]
    Info getInfo() const
    {
        return {pods.size(), muscles.size()};
    }
};

