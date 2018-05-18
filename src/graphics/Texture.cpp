#include "solarus/graphics/Texture.h"
#include "solarus/graphics/RenderTexture.h"
#include "solarus/core/Debug.h"
#include "solarus/graphics/Surface.h"

namespace Solarus {

/**
 * @brief Texture::Texture
 * @param surface valid sdl surface, ownership is taken by the texture
 */
Texture::Texture(SDL_Surface *surface)
    : surface(surface)
{
  SDL_Texture* tex = SDL_CreateTextureFromSurface(Video::get_renderer(),surface);
  Debug::check_assertion(tex != nullptr,
        std::string("Failed to convert surface to texture") + SDL_GetError());
  texture.reset(tex);
}

/**
 * \copydoc SurfaceImpl::get_texture
 */
SDL_Texture *Texture::get_texture() const {
    return texture.get();
}

/**
 * \copydoc SurfaceImpl::get_surface
 */
SDL_Surface *Texture::get_surface() const {
    return surface.get();
}

/**
 * \copydoc SurfaceImpl::get_width
 */
int Texture::get_width() const {
    return surface->w;
}

/**
 * \copydoc SurfaceImpl::get_height
 */
int Texture::get_height() const {
    return surface->h;
}

/**
 * \copydoc SurfaceImpl::to_render_texture
 */
RenderTexture* Texture::to_render_texture() {
    RenderTexture* rt = new RenderTexture(get_width(),get_height());
    rt->draw_other(*this,DrawInfos(Rectangle(Point(),Size(get_width(),get_height())),
                                   Point(),
                                   BlendMode::NONE,255,Surface::draw_proxy));
    return rt;
}

}
