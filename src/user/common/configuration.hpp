#pragma once
#include "engine/common/vec.hpp"

namespace conf
{
constexpr uint32_t window_width  = 1600;
constexpr uint32_t window_height = 900;

constexpr uint32_t input_count  = 13;
constexpr uint32_t output_count = 8;

constexpr float maximum_distance = 1000.0f;
const Vec2      world_size       = {maximum_distance, maximum_distance};
constexpr float target_radius    = 60.0f;
}
