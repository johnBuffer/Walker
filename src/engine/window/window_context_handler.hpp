#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "engine/common/vec.hpp"
#include "engine/common/event_manager.hpp"
#include "engine/engine.hpp"

namespace pez::render
{
class WindowContextHandler
{
public:
    WindowContextHandler(const std::string& window_name,
                         UVec2 window_size,
                         sf::ContextSettings settings,
                         uint32_t style = sf::Style::Default)
         : m_window{sf::VideoMode{window_size.x, window_size.y}, window_name, style, settings}
         , m_event_manager(m_window, true)
         , m_render_context(nullptr)
    {
        // Initialize Engine and its sub systems
        pez::core::createSystems();

        // Initialize events and render
        m_render_context = pez::core::GlobalInstance::instance->m_render_context;
        m_render_context->setWindow(m_window);
        // Initialize base shaders
        registerDefaultCallbacks(true);
    }

    ~WindowContextHandler() = default;

    void registerDefaultCallbacks(bool use_viewport_callbacks)
    {
        m_event_manager.addEventCallback(sf::Event::EventType::Closed, [&](sfev::CstEv) { exit(); });
        m_event_manager.addKeyPressedCallback(sf::Keyboard::Escape, [&](sfev::CstEv) { exit(); });
        m_render_context->registerCallbacks(m_event_manager, use_viewport_callbacks);
    }

    void exit()
    {
        m_running = false;
    }

    bool run()
    {
        m_event_manager.processEvents();
        return m_running;
    }

    sfev::EventManager& getEventManager()
    {
        return m_event_manager;
    }

    [[nodiscard]]
    Vec2 getWorldMousePosition() const
    {
        return m_render_context->m_viewport_handler.getMouseWorldPosition();
    }

    [[nodiscard]]
    Vec2 getMousePosition() const
    {
        return m_render_context->m_viewport_handler.state.mouse_position;
    }

    Context& getRenderContext()
    {
        return *m_render_context;
    }

    uint64_t target_frame_time = 16;

private:
    sf::RenderWindow   m_window;
    Context*           m_render_context = nullptr;
    bool               m_running        = true;
    sfev::EventManager m_event_manager;
    uint64_t           m_last_time      = 0;
};
}
