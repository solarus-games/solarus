#include "solarus/graphics/Texture.h"
#include "solarus/graphics/RenderTexture.h"
#include "solarus/core/Debug.h"
#include "solarus/graphics/Surface.h"

#include <SFML/Graphics/Sprite.hpp>

namespace Solarus {

Texture::Texture(SDL_Surface *surface)
    : surface(surface)
{
  SDL_Texture* tex = SDL_CreateTextureFromSurface(Video::get_renderer(),surface);
  Debug::check_assertion(tex != nullptr,
        std::string("Failed to convert surface to texture") + SDL_GetError());
  texture.reset(tex);
}

const SDL_Texture *Texture::get_texture() const {
    return texture.get();
}

const SDL_Surface *Texture::get_surface() const {
    return surface.get();
}

int Texture::get_width() const {
    return surface->w;
}

int Texture::get_height() const {
    return surface->h;
}

RenderTexture* Texture::to_render_texture() {
    RenderTexture* rt = new RenderTexture(get_width(),get_height());
    rt->draw_other(*this,Point(0,0));
    return rt;
}

}
