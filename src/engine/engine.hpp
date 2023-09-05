#pragma once
#include <cstdint>
#include "engine/core/instance.hpp"
#include "engine/core/timer.hpp"
#include "./render/render.hpp"


namespace pez::core
{

void     createSystems();
void     quit();
void     update(float dt);
void     render();
uint64_t getTick();
float    getTime();
void     setPause(bool pause);
void     togglePause();
bool     isRunning();

template<typename T>
uint32_t getClassID()
{
    return EntityContainer<T>::class_id;
}

template<typename T>
static civ::IndexVector<T>& getData()
{
    return core::EntityContainer<T>::data;
}

template<typename T, typename... Arg>
static ID create(Arg&&... args)
{
    return core::EntityContainer<T>::create(std::forward<Arg>(args)...);
}

template<typename T>
static T& get(civ::ID id)
{
    return core::EntityContainer<T>::data[id];
}

template<typename T>
civ::Ref<T> getRef(civ::ID id)
{
    return core::EntityContainer<T>::data.createRef(id);
}

template<typename T>
T& getProcessor()
{
    return *core::System<T>::instance;
}

template<typename T>
T& getRenderer()
{
    return *core::System<T>::instance;
}

template<typename T>
T& getSingleton()
{
    return *core::System<T>::instance;
}

template<typename T, typename... Arg>
civ::Ref<T> createGetRef(Arg&&... args)
{
    const civ::ID id = core::EntityContainer<T>::create(std::forward<Arg>(args)...);
    return core::EntityContainer<T>::data.createRef(id);
}

template<typename T>
bool isValid(const core::EntityRef& ref)
{
    return GlobalInstance::instance->m_entity_manager->isValid<T>(ref);
}

bool isValidRef(const core::EntityRef& ref);

// This name isn't nice because of civ::Ref
template<typename T>
core::EntityRef createEntityRef(core::EntityID id)
{
    return {id.class_id, id.instance_id, core::EntityContainer<T>::data.getValidityID(id.instance_id)};
}

template<typename T>
core::EntityRef createEntityRef(core::ID id)
{
    return {EntityContainer<T>::class_id, id, core::EntityContainer<T>::data.getValidityID(id)};
}

template<typename T>
T& get(const core::EntityRef& ref)
{
    return get<T>(ref.id.instance_id);
}

template<typename T>
bool isInstanceOf(const core::EntityRef& ref)
{
    return core::EntityContainer<T>::class_id == ref.id.class_id;
}

template<typename T>
bool isInstanceOf(const core::EntityID& id)
{
    return core::EntityContainer<T>::class_id == id.class_id;
}

template<typename T>
void registerEntity()
{
    core::GlobalInstance::instance->m_entity_manager->registerEntity<T>();
}

template<typename T>
void registerDataEntity()
{
    core::GlobalInstance::instance->m_entity_manager->registerDataEntity<T>();
}

template<typename T, typename... TArg>
void registerProcessor(TArg&&... args)
{
    core::GlobalInstance::instance->m_entity_manager->registerProcessor<T>(std::forward<TArg>(args)...);
}

template<typename T, typename... TArg>
static void registerRenderer(TArg&&... args)
{
    core::GlobalInstance::instance->m_entity_manager->registerRenderer<T>(std::forward<TArg>(args)...);
}

template<typename T, typename... TArg>
static void registerSingleton(TArg&&... args)
{
    core::GlobalInstance::instance->m_entity_manager->registerSingleton<T>(std::forward<TArg>(args)...);
}

template<typename T>
void remove(ID id)
{
    EntityContainer<T>::data.erase(id);
}

}
