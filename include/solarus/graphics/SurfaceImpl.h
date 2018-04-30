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
    /**
     * @brief get the underlying SDL_Texture
     * @return a valid SDL_Texture
     */
    virtual SDL_Texture* get_texture() const = 0;

    /**
     * @brief get the synchronised SDL_Surface
     *
     * The SDL_Texture alone is not convenient to access, for pixel
     * perfect collisions and pixel access, we keep a surface as a
     * cache.
     *
     * @return a valid SDL_Surface
     */
    virtual SDL_Surface* get_surface() const = 0;

    /**
     * @brief get texture width
     * @return width
     */
    virtual int get_width() const = 0;

    /**
     * @brief get texture height
     * @return height
     */
    virtual int get_height() const = 0;

    /**
     * @brief access SurfaceImpl parent
     * @return parent reference
     */
    const Surface& parent() const;

    /**
     * @brief upload potentially modified surface
     *
     * When modifying pixels of the Surface, we have
     * to upload it to the texture for changes to be reflected
     */
    void upload_surface();

    /**
     * @brief ~SurfaceImpl
     */
    virtual ~SurfaceImpl();

    /**
     * @brief return a, potentialy new, RenderTexture implementation
     * @return valid render texture
     */
    virtual  RenderTexture* to_render_texture() = 0;

    bool is_premultiplied() const;
    void set_premultiplied(bool a_premultiplied);
private:
    Surface* _parent; /**< pointer to owning surface */
    bool premultiplied = false;
};

}
