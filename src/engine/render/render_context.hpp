#pragma once
#include "engine/common/vec.hpp"
#include "engine/common/event_manager.hpp"
#include <SFML/Graphics.hpp>

#include "viewport_handler.hpp"


class RenderContext
{
public:
    RenderContext()  = default;
    ~RenderContext() = default;

    IVec2 m_size = {};
    float scale  = 1.0f;

    void registerCallbacks(sfev::EventManager& event_manager, bool use_viewport_callbacks)
    {
        event_manager.addEventCallback(sf::Event::MouseMoved, [&](sfev::CstEv) {
            m_viewport_handler.setMousePosition(event_manager.getFloatMousePosition());
        });
        if (use_viewport_callbacks) {
            event_manager.addMousePressedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
                m_viewport_handler.click(event_manager.getFloatMousePosition());
            });
            event_manager.addMouseReleasedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
                m_viewport_handler.unclick();
            });
            event_manager.addEventCallback(sf::Event::MouseWheelMoved, [&](sfev::CstEv e) {
                m_viewport_handler.wheelZoom(e.mouseWheelScroll.delta);
            });
        }
    }

    void setFocus(Vec2 focus)
    {
        m_viewport_handler.setFocus(focus);
    }

    void setZoom(float zoom)
    {
        m_viewport_handler.setZoom(zoom);
    }

    void clear()
    {
        m_window->clear(sf::Color::Black);
    }

    void display();

    [[nodiscard]]
    IVec2 getRenderSize() const
    {
        return m_render_size;
    }

    [[nodiscard]]
    Vec2 getCameraPosition() const
    {
        return m_viewport_handler.state.offset;
    }

    [[nodiscard]]
    float getCameraZoom() const
    {
        return m_viewport_handler.state.zoom;
    }

    void draw(sf::Drawable& drawable)
    {

    }

private:

    IVec2             m_render_size       = {};
    ViewportHandler   m_viewport_handler;
    sf::RenderWindow* m_window            = nullptr;

    friend class WindowContextHandler;
};
