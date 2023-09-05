#pragma once
#include "engine/render/render_context.hpp"
#include "entity_manager.hpp"
#include "system.hpp"


namespace pez::core
{

struct EngineInstance
{
    EntityManager*        m_entity_manager = nullptr;
    pez::render::Context* m_render_context = nullptr;

    // Time related attributes
    float    time  = 0.0f;
    uint64_t tick  = 0;
    bool     pause = false;

    EngineInstance();

    void update(float dt);
    void quit();
    void render();
};

struct GlobalInstance
{
    // Singleton instance
    static core::EngineInstance* instance;
};

}
