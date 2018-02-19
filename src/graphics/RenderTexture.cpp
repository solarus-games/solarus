#include "solarus/graphics/RenderTexture.h"
#include "solarus/graphics/Surface.h"

#include "solarus/core/Debug.h"

namespace Solarus {

RenderTargetAtlas RenderTexture::render_atlas;

RenderTexture::RenderTexture(int width, int height, bool depth_buffer):
  width(width),height(height)
{
  /*if(!target.create(width,height,depth_buffer)) {
        Solarus::Debug::error("Could not create render target");
    }*/
  //target = render_atlas.create_view(width,height);
  //sprite.setTexture(target.getTexture());
  //target->clear();
  SDL_Texture* tex = SDL_CreateTexture(Video::get_renderer(),
                                       Video::get_pixel_format()->format,
                                       SDL_TEXTUREACCESS_TARGET,
                                       width,height);
  Debug::check_assertion(tex!=nullptr,
                         std::string("Failed to create render texture : ") + SDL_GetError());
  target.reset(tex);

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
  surface_dirty = true;
  Rectangle dst_rect(dst_position,Size(texture.get_width(),texture.get_height()));
  SDL_Renderer* renderer = Video::get_renderer();
  SDL_SetRenderTarget(renderer,target.get());
  SDL_SetRenderDrawBlendMode(renderer,texture.parent().get_sdl_blend_mode());
  SDL_RenderCopy(renderer,texture.get_nonconst_texture(),src_rect,dst_rect);
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

void RenderTexture::fill_with_color(const Color& color, const Rectangle& where) {
  auto renderer = Video::get_renderer();
  Uint8 r,g,b,a;
  color.get_components(r,g,b,a);
  SDL_SetRenderDrawColor(renderer,r,g,b,a);
  SDL_RenderFillRect(renderer,where);
}

void RenderTexture::clear() {
  //target->clear()M
  SDL_Renderer* renderer = Video::get_renderer();
  SDL_SetRenderTarget(renderer,target.get());
  SDL_RenderClear(renderer);
}

void RenderTexture::clear(const Rectangle& where) {
  //target->clear(where);
  //TODO
}

}
