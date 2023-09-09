#include "engine.hpp"


void pez::core::createSystems()
{
    GlobalInstance::instance = new core::EngineInstance();
    // Create singletons provided by default by the engine
    createDefaultSingletons();
}

void pez::core::quit()
{
    GlobalInstance::instance->quit();
}

void pez::core::render()
{
    pez::render::Context& context = *(GlobalInstance::instance->m_render_context);
    context.clear();
    GlobalInstance::instance->m_entity_manager.render(context);
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
    return pez::core::GlobalInstance::instance->m_entity_manager.validity_callbacks[ref.id.class_id](ref);
}

bool pez::core::isRunning()
{
    return !core::GlobalInstance::instance->pause;
}

void pez::core::createDefaultSingletons()
{
    auto const core_count = std::thread::hardware_concurrency();
    if (core_count < 2) {
        std::cout << "Cannot detect core count, disabling multithreading." << std::endl;
        pez::core::registerSingleton<tp::ThreadPool>(1);
    } else {
        std::cout << "Using " << core_count << " cores for multithreading." << std::endl;
        // Minus one for the main thread
        pez::core::registerSingleton<tp::ThreadPool>(core_count - 1);
    }

}
