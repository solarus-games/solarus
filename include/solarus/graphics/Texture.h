#pragma once

#include "solarus/graphics/SurfaceImpl.h"
#include "solarus/graphics/SDLPtrs.h"

namespace Solarus {

/**
 * @brief SurfaceImpl representing immutable surface data
 *
 * Textures are mainly created from image files
 */
class Texture : public SurfaceImpl
{
public:
    Texture(SDL_Surface* surface);
    SDL_Texture* get_texture() const override;
    SDL_Surface* get_surface() const override;

    int get_width() const override;
    int get_height() const override;

    RenderTexture* to_render_texture() override;
private:
    mutable SDL_Surface_UniquePtr surface; /**< cpu side pixels data */
    mutable SDL_Texture_UniquePtr texture; /**< gpu side pixels data */
};

}
