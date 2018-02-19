#pragma once

#include "solarus/core/Rectangle.h"
#include "solarus/core/Point.h"

#include <SDL_render.h>

namespace Solarus {

class Texture;
class RenderTexture;
class Surface;

/**
 * @brief Abstract class for internal surface pixel representation and manipulation
 */
class SurfaceImpl
{
    friend class Surface;
public:
    SurfaceImpl();
    virtual const SDL_Texture* get_texture() const = 0;
    SDL_Texture* get_nonconst_texture() const;
    virtual const SDL_Surface* get_surface() const = 0;
    SDL_Surface *get_nonconst_surface() const;

    virtual int get_width() const = 0;
    virtual int get_height() const = 0;

    /*virtual void draw_region_other(const Rectangle& region, const SurfaceImpl& other, const Point& dst_position) = 0;
    void draw_other(const SurfaceImpl& other, const Point& dst_position = Point(0,0));

    virtual void draw(const sf::Drawable&,const sf::RenderStates& states=sf::RenderStates::Default) = 0;
    virtual void draw(const sf::Vertex* vertices, size_t vertexCount,sf::PrimitiveType type,const sf::RenderStates& states =sf::RenderStates::Default) = 0;

    virtual void clear() = 0;
    virtual void clear(const Rectangle& where) = 0;*/

    const Surface& parent() const;

    //virtual  Texture* to_texture() = 0;
    virtual ~SurfaceImpl();
    virtual  RenderTexture* to_render_texture() = 0;
private:
    Surface* _parent;
};

}
