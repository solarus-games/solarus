#pragma once

#include "solarus/graphics/SurfaceImpl.h"
#include "solarus/graphics/RenderTargetAtlas.h"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace Solarus {

class RenderTexture : public SurfaceImpl
{
public:
    RenderTexture(int width,int height,bool depth_buffer = false);
    const SDL_Texture* get_texture() const override;
    const SDL_Surface* get_surface() const override;

    int get_width() const override;
    int get_height() const override;

    void draw_other(const SurfaceImpl& texture, const Point& dst_position = Point(0,0));
    void draw_region_other(const Rectangle &src_rect, const SurfaceImpl &region, const Point& dst_position);

    RenderTexture* to_render_texture() override;

    void fill_with_color(const Color& color, const Rectangle& where);

    void clear();
    void clear(const Rectangle& where);
    ~RenderTexture(){
    }
private:
    static RenderTargetAtlas render_atlas;
    mutable bool surface_dirty = true;
    SDL_Surface_UniquePtr surface;
    SDL_Texture_UniquePtr target;
    int width,height;
    //RenderTargetViewPtr target;
};

}
