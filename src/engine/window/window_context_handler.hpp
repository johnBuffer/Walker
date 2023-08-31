#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "engine/common/vec.hpp"
#include "engine/common/event_manager.hpp"
#include "engine/engine.hpp"


class WindowContextHandler
{
public:
    WindowContextHandler(const std::string& window_name,
                         UVec2 window_size,
                         bool use_viewport_callbacks = true)
         : m_window{sf::VideoMode{window_size.x, window_size.y}, window_name}
         , m_event_manager(m_window, true)
         , m_render_context(nullptr)
    {
        // Initialize Engine and its sub systems
        pez::core::createSystems();

        // Initialize events and render
        //m_event_manager.initialize(m_window);
        //m_render_context->initialize(m_window, window_size);
        // Initialize base shaders
        registerDefaultCallbacks(use_viewport_callbacks);
    }

    ~WindowContextHandler()
    {
    }

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

    uint64_t target_frame_time = 16;

private:
    sf::RenderWindow   m_window;
    RenderContext*     m_render_context = nullptr;
    bool               m_running        = true;
    sfev::EventManager m_event_manager;
    uint64_t           m_last_time      = 0;
};
