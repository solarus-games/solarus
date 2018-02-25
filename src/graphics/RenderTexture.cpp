#include "solarus/graphics/RenderTexture.h"
#include "solarus/graphics/Surface.h"

#include "solarus/core/Debug.h"

namespace Solarus {

//RenderTargetAtlas RenderTexture::render_atlas;

RenderTexture::RenderTexture(int width, int height, bool depth_buffer):
  width(width),height(height)
{
  SDL_Texture* tex = SDL_CreateTexture(Video::get_renderer(),
                                       Video::get_pixel_format()->format,
                                       SDL_TEXTUREACCESS_TARGET,
                                       width,height);
  Debug::check_assertion(tex!=nullptr,
                         std::string("Failed to create render texture : ") + SDL_GetError());
  target.reset(tex);
  clear();
  //fill_with_color(Color::black,Rectangle(0,0,width,height));
}

int RenderTexture::get_width() const {
  return width;
}

int RenderTexture::get_height() const {
  return height;
}

const SDL_Texture* RenderTexture::get_texture() const {
  return target.get();
}


void RenderTexture::draw_other(const SurfaceImpl& texture, const Point& dst_position) {
  draw_region_other(Rectangle(0,0,texture.get_width(),texture.get_height()),texture,dst_position);
}

void RenderTexture::draw_region_other(const Rectangle& src_rect, const SurfaceImpl& texture, const Point& dst_position) {
  draw_env({
   Rectangle dst_rect(dst_position,src_rect.get_size());
   SDL_SetTextureAlphaMod(texture.get_nonconst_texture(),texture.parent().get_opacity());
   SDL_SetTextureBlendMode(texture.get_nonconst_texture(),texture.parent().get_sdl_blend_mode());
   SDL_RenderCopy(renderer,texture.get_nonconst_texture(),src_rect,dst_rect);
 })
}

const SDL_Surface *RenderTexture::get_surface() const {
  if (surface_dirty) {
    //TODO SDL black magic here
    //image = get_texture().copyToImage(); //Slow but functionnal
    surface_dirty = false;
  }
  return surface.get();
}

RenderTexture* RenderTexture::to_render_texture() {
  return this;
}

void RenderTexture::fill_with_color(const Color& color, const Rectangle& where, SDL_BlendMode mode) {
  const SDL_Rect* rect = where;
  draw_env({
    Uint8 r,g,b,a;
    color.get_components(r,g,b,a);
    SDL_SetRenderDrawColor(renderer,r,g,b,a);
    SDL_SetRenderDrawBlendMode(renderer,mode);
    SDL_RenderFillRect(renderer,rect);
  })
}

void RenderTexture::clear() {
  draw_env({
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_SetTextureBlendMode(target.get(),SDL_BLENDMODE_BLEND);
    SDL_RenderClear(renderer);
  })
}

void RenderTexture::clear(const Rectangle& where) {
  fill_with_color(Color::transparent,where,SDL_BLENDMODE_NONE);
}

}
