#pragma once

#include "solarus/graphics/SurfaceImpl.h"
#include "solarus/graphics/SDLPtrs.h"

namespace Solarus {

class Texture : public SurfaceImpl
{
public:
    Texture(SDL_Surface* surface);
    const SDL_Texture* get_texture() const override;
    const SDL_Surface* get_surface() const override;

    int get_width() const override;
    int get_height() const override;

    RenderTexture* to_render_texture() override;
private:
    SDL_Surface_UniquePtr surface;
    SDL_Texture_UniquePtr texture;
};

}
