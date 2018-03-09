#pragma once

#include "solarus/graphics/SurfaceImpl.h"
#include "solarus/graphics/Color.h"
#include "solarus/graphics/SDLPtrs.h"
#include "solarus/graphics/Video.h"

namespace Solarus {

class RenderTexture : public SurfaceImpl
{
public:
    RenderTexture(int width,int height,bool depth_buffer = false);
    SDL_Texture* get_texture() const override;
    SDL_Surface* get_surface() const override;

    template<typename Func>
    void with_target(Func closure) const {
      surface_dirty = true;
      auto renderer = Video::get_renderer();
      SDL_SetRenderTarget(renderer,target.get());
      closure(renderer);
      //SDL_SetRenderTarget(renderer,nullptr);
    }

    int get_width() const override;
    int get_height() const override;

    void draw_other(const SurfaceImpl& texture, const Point& dst_position = Point(0,0));
    void draw_region_other(const Rectangle &src_rect, const SurfaceImpl &region, const Point& dst_position);

    RenderTexture* to_render_texture() override;

    void fill_with_color(const Color& color, const Rectangle& where, SDL_BlendMode mode = SDL_BLENDMODE_BLEND);

    void clear();
    void clear(const Rectangle& where);
    ~RenderTexture(){
    }
private:
    //static RenderTargetAtlas render_atlas;
    mutable bool surface_dirty = true;
    mutable SDL_Surface_UniquePtr surface;
    mutable SDL_Texture_UniquePtr target;
    int width,height;
    //RenderTargetViewPtr target;
};

}
