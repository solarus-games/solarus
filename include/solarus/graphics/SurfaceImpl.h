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
    virtual SDL_Texture* get_texture() const = 0;
    virtual SDL_Surface* get_surface() const = 0;

    virtual int get_width() const = 0;
    virtual int get_height() const = 0;

    const Surface& parent() const;

    //virtual  Texture* to_texture() = 0;
    virtual ~SurfaceImpl();
    virtual  RenderTexture* to_render_texture() = 0;
private:
    Surface* _parent;
};

}
