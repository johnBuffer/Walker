#pragma once
#include "engine/common/vec.hpp"

namespace conf
{
constexpr uint32_t window_width  = 1600;
constexpr uint32_t window_height = 900;

constexpr uint32_t input_count  = 9;
constexpr uint32_t output_count = 6;

constexpr float maximum_distance = 1000.0f;
const Vec2      world_size       = {maximum_distance, maximum_distance};
constexpr float target_radius    = 60.0f;

constexpr uint32_t population_size    = 5000;
constexpr uint32_t demo_period        = 5;
constexpr float    max_iteration_time = 100.0f;
constexpr float    elite_ratio        = 0.2f;
constexpr float    target_reward      = 100.0f;
}
