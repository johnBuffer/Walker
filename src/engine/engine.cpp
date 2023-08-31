#include "engine.hpp"


void pez::core::createSystems()
{
    GlobalInstance::instance = new core::EngineInstance();
}

void pez::core::quit()
{
    GlobalInstance::instance->quit();
}

void pez::core::render()
{
    RenderContext& context = *(GlobalInstance::instance->m_render_context);
    context.clear();
    GlobalInstance::instance->m_entity_manager->render(context);
    context.display();
}

void pez::core::update(float dt)
{
    GlobalInstance::instance->update(dt);
}

uint64_t pez::core::getTick()
{
    return GlobalInstance::instance->tick;
}

float pez::core::getTime()
{
    return GlobalInstance::instance->time;
}

void pez::core::setPause(bool pause)
{
    GlobalInstance::instance->pause = pause;
}

void pez::core::togglePause()
{
    GlobalInstance::instance->pause = !GlobalInstance::instance->pause;
}

bool pez::core::isValidRef(const pez::core::EntityRef& ref)
{
    if (ref.id.class_id == pez::core::EntityID::INVALID_ID) {
        return false;
    }
    return pez::core::GlobalInstance::instance->m_entity_manager->validity_callbacks[ref.id.class_id](ref);
}

bool pez::core::isRunning()
{
    return !core::GlobalInstance::instance->pause;
}
