#pragma once
#include <memory>
#include "entity_container.hpp"
#include "entity.hpp"
#include "engine/render/render_context.hpp"


namespace pez::core
{

struct ISystem
{
    virtual void stop() {}
};

struct IProcessor : public ISystem
{
    virtual void update(float dt) {}
};

struct IRenderer : public ISystem
{
    virtual void render(RenderContext& context) {}
};

template<typename T>
struct System
{
    static std::unique_ptr<T> instance;

    template<typename... Arg>
    static void create(Arg&&... args)
    {
        instance = std::make_unique<T>(std::forward<Arg>(args)...);
    }

    static T& get()
    {
        return *instance;
    }

    static void stop()
    {
        instance->stop();
    }

    static void clear()
    {
        instance = nullptr;
    }

    static bool isRegistered()
    {
        return instance != nullptr;
    }
};

template<typename T>
std::unique_ptr<T> System<T>::instance = nullptr;


template<typename T>
struct Processor
{
    static void update(float dt)
    {
        System<T>::instance->update(dt);
    }
};


template<typename T>
struct Renderer
{
    static void render(RenderContext& context)
    {
        System<T>::instance->render(context);
    }
};

template<typename T, typename TCallback>
void foreach(TCallback&& callback) {
    static_assert(std::is_convertible<T*, Entity*>::value, "Can only iterate on Entity derived objects");
    std::vector<T>& data = core::EntityContainer<T>::data.getData();
    const uint64_t count = core::EntityContainer<T>::data.size();
    for (uint64_t i{0}; i<count; ++i) {
        if (!data[i].isRemoved()) {
            callback(data[i]);
        }
    }
}

template<typename T, typename TCallback>
void foreachAbort(TCallback&& callback) {
    static_assert(std::is_convertible<T*, Entity*>::value, "Can only iterate on Entity derived objects");
    std::vector<T>& data = core::EntityContainer<T>::data.getData();
    const uint64_t count = core::EntityContainer<T>::data.size();
    for (uint64_t i{0}; i<count; ++i) {
        if (!data[i].isRemoved()) {
            if (callback(data[i])) {
                return;
            }
        }
    }
}


}

