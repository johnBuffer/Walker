#pragma once
#include "engine/common/vec.hpp"

namespace conf
{
namespace win
{
    constexpr uint32_t window_width = 1600;
    constexpr uint32_t window_height = 900;
}


constexpr uint32_t input_count  = 15;
constexpr uint32_t output_count = 12;

constexpr float maximum_distance = 1000.0f;
const Vec2      world_size       = {maximum_distance, maximum_distance};
constexpr float target_radius    = 20.0f;

constexpr uint32_t population_size    = 2000;
constexpr uint32_t demo_period        = 10;
constexpr float    max_iteration_time = 100.0f;
constexpr float    elite_ratio        = 0.2f;
constexpr float    target_reward      = 100.0f;


namespace mut
{
    constexpr float new_node_proba = 0.05f;
    constexpr float new_conn_proba = 0.2f;

    constexpr float new_value_proba     = 0.1f;
    constexpr float weight_range        = 5.0f;
    constexpr float weight_small_range  = 0.1f;

    constexpr float offset_weight_proba = 0.7f;
    constexpr float offset_bias_proba   = 0.8f;
}

namespace exp
{
    constexpr uint32_t seed_offset        = 22;
    constexpr uint32_t best_save_period   = 50;
    constexpr uint32_t exploration_period = 1000;
}

}
