#include "solarus/graphics/RenderTexture.h"
#include "solarus/graphics/Surface.h"

#include "solarus/core/Debug.h"

namespace Solarus {

//RenderTargetAtlas RenderTexture::render_atlas;
/**
 * @brief RenderTexture::RenderTexture
 * @param width width of the render texture
 * @param height height og the render texture
 */
RenderTexture::RenderTexture(int width, int height)
{
  auto renderer = Video::get_renderer();
  auto tex = SDL_CreateTexture(renderer,
                                       Video::get_rgba_format()->format,
                                       SDL_TEXTUREACCESS_TARGET,
                                       width,height);
  Debug::check_assertion(tex!=nullptr,
                         std::string("Failed to create render texture : ") + SDL_GetError());
  target.reset(tex);

  auto format = Video::get_rgba_format();
  auto surf_ptr = SDL_CreateRGBSurface(0,
                                    width,
                                    height,
                                    32,
                                    format->Rmask,
                                       format->Gmask,
                                       format->Bmask,
                                       format->Amask);
  Debug::check_assertion(surf_ptr!=nullptr,
                         std::string("Failed to create backup surface ") + SDL_GetError());
  surface.reset(surf_ptr);
}

/**
 * \copydoc SurfaceImpl::get_width
 */
int RenderTexture::get_width() const {
  return surface->w;
}

/**
 * \copydoc SurfaceImpl::get_height
 */
int RenderTexture::get_height() const {
  return surface->h;
}

/**
 * \copydoc SurfaceImpl::get_texture
 */
SDL_Texture* RenderTexture::get_texture() const {
  return target.get();
}

/**
 * @brief draw another surface implementation on this one
 * @param texture other surface
 * @param dst_position position where to draw the surface
 */
void RenderTexture::draw_other(const SurfaceImpl& texture, const Point& dst_position) {
  draw_region_other(Rectangle(0,0,texture.get_width(),texture.get_height()),texture,dst_position);
}

/**
 * @brief draw region of another surfaceimpl on this one
 * @param src_rect source region to draw
 * @param texture source surface
 * @param dst_position position where to draw
 */
void RenderTexture::draw_region_other(const Rectangle& src_rect, const SurfaceImpl& texture, const Point& dst_position) {
  with_target([&](SDL_Renderer* renderer){
   Rectangle dst_rect(dst_position,src_rect.get_size());
   SDL_SetTextureAlphaMod(texture.get_texture(),texture.parent().get_opacity());
   SDL_SetTextureBlendMode(texture.get_texture(),texture.parent().get_sdl_blend_mode());
   SDL_RenderCopy(renderer,texture.get_texture(),src_rect,dst_rect);
 });
}

/**
 * \copydoc SurfaceImpl::get_surface
 */
SDL_Surface *RenderTexture::get_surface() const {
  if (surface_dirty) {
    with_target([&](SDL_Renderer* renderer){
      Rectangle rect(0,0,get_width(),get_height());
      SDL_RenderReadPixels(renderer,
                           rect,
                           Video::get_rgba_format()->format,
                           surface->pixels,
                           surface->pitch
                           );
    });
    surface_dirty = false;
  }
  return surface.get();
}

/**
 * \copydoc SurfaceImpl::to_render_texture
 */
RenderTexture* RenderTexture::to_render_texture() {
  return this;
}

/**
 * @brief fill a region of this surface with a color
 * @param color color
 * @param where region to fill
 * @param mode blend mode
 */
void RenderTexture::fill_with_color(const Color& color, const Rectangle& where, SDL_BlendMode mode) {
  const SDL_Rect* rect = where;
  with_target([&](SDL_Renderer* renderer){
    Uint8 r,g,b,a;
    color.get_components(r,g,b,a);
    SDL_SetRenderDrawColor(renderer,r,g,b,a);
    SDL_SetRenderDrawBlendMode(renderer,mode);
    SDL_RenderFillRect(renderer,rect);
  });
}

/**
 * @brief RenderTexture::clear
 */
void RenderTexture::clear() {
  with_target([&](SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_SetTextureBlendMode(target.get(),SDL_BLENDMODE_BLEND);
    SDL_RenderClear(renderer);
  });
}

/**
 * @brief RenderTexture::clear
 * @param where region to clear
 */
void RenderTexture::clear(const Rectangle& where) {
  fill_with_color(Color::transparent,where,SDL_BLENDMODE_NONE);
}

}
