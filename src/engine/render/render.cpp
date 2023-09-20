#include "./render.hpp"

#include "../engine.hpp"


namespace pez::render
{
    pez::render::Context* getContext()
    {
        return pez::core::GlobalInstance::instance->m_render_context;
    }

    void setFocus(Vec2 focus)
    {
        pez::core::GlobalInstance::instance->m_render_context->setFocus(focus);
    }

    void setZoom(float zoom)
    {
        pez::core::GlobalInstance::instance->m_render_context->setZoom(zoom);
    }

    void clear(sf::Color color)
    {
        pez::core::GlobalInstance::instance->m_render_context->clear(color);
    }
}