#include "solarus/graphics/RenderTargetAtlas.h"

#include "solarus/graphics/RenderTexture.h"


namespace Solarus {


constexpr unsigned atlas_size = 1024;
constexpr float inv_size = 1.f / atlas_size;

RenderTargetView::RenderTargetView(Atlas &atlas,
                                   const Rectangle& rect,
                                   Bin& bin) :
  rect(rect),
  bin(bin),
  atlas(atlas),
  tex_dirty(true)
{

}

int RenderTargetView::get_width() const {
  return rect.get_width();
}
int RenderTargetView::get_height() const {
  return rect.get_height();
}

void RenderTargetView::setup_viewport(SDL_Renderer* renderer) {
  SDL_SetRenderTarget(renderer,atlas.target.get());
  SDL_RenderSetViewport(renderer,rect);
}

void RenderTargetView::draw(SDL_Texture* texture, const Rectangle& src_rect, const Point& dst_position) {
  SDL_Renderer* renderer = Video::get_renderer();
  Rectangle dst_rect(dst_position,src_rect.get_size());
  setup_viewport(renderer);
  SDL_RenderCopy(renderer,texture,src_rect,dst_rect);
}

void RenderTargetView::draw(const VertexArray &array) {
  SDL_Renderer* renderer = Video::get_renderer();
  setup_viewport(renderer);
  //TODO
}

void RenderTargetView::draw_on(RenderTexture& target, const Rectangle& region, const Point& dst_position, uint8_t opacity) const {
  Rectangle true_region = region;
  true_region.add_xy(rect.get_xy());
  //TODO
}

SDL_Texture *RenderTargetView::get_texture() const {
  return atlas.target.get();
}

void RenderTargetView::clear(const Color& clearColor) {
  clear_sub(rect,clearColor);
}

void RenderTargetView::clear_sub(const Rectangle& where,const Color& clearColor) {
  SDL_Renderer* renderer = Video::get_renderer();
  SDL_SetRenderTarget(renderer,atlas.target.get());
  Uint8 r,g,b,a;
  clearColor.get_components(r,g,b,a);
  SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(renderer,r,g,b,a);
  SDL_RenderFillRect(renderer,where);
}

void RenderTargetView::clear(const Rectangle& where, const Color &clearColor) {
  Rectangle corrected = where;
  corrected.add_xy(rect.get_xy());
  corrected = corrected & rect;
  clear_sub(corrected,clearColor);
}


RenderTargetView::~RenderTargetView() {
  clear(Color::red); //TODO remove
  atlas.pack.unref(bin);
}


RenderTargetAtlas::RenderTargetAtlas()
{
}

RenderTargetViewPtr RenderTargetAtlas::create_view(Atlas& atlas,Bin& b) const {
  Rectangle rect(b.x,b.y,b.w,b.h);
  RenderTargetView* rtv = new RenderTargetView(atlas,rect,b);
  rtv->clear(); //Make sur target is cleaned
  return RenderTargetViewPtr(rtv);
}

RenderTargetViewPtr RenderTargetAtlas::create_view(int width, int height) {
  //Try to fit into the previous atlases in case there is free space
  for(const AtlasPtr& a : mAtlases) {
    Bin* b = a->pack.packOne(-1,width,height);
    if(b) {
      return create_view(*a,*b);
    }
  }
  //No space found
  expand();
  //Retry
  return create_view(width,height);
}

void RenderTargetAtlas::expand() {
  mAtlases.emplace_back(new Atlas(atlas_size,atlas_size));
}

void RenderTargetAtlas::repack() {
  //TODO
}

}
