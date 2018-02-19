#pragma once

#include "solarus/core/Rectangle.h"

#include "solarus/third_party/shelf-pack.hpp"
#include "solarus/graphics/SurfaceImpl.h"
#include "solarus/graphics/Video.h"
#include "solarus/graphics/VertexArray.h"
#include "solarus/graphics/SDLPtrs.h"

#include <memory>

namespace Solarus {

using Pack = mapbox::ShelfPack;
using Bin = mapbox::Bin;

class Atlas;



class RenderTargetView {
public:
    RenderTargetView(Atlas& atlas,
                     const Rectangle& rect,
                     Bin& bi);
    void setup_viewport(SDL_Renderer *renderer);
    void draw(SDL_Texture *texture, const Rectangle& src_rect, const Point& dst_position);
    void draw(const VertexArray& array);
    int get_width() const;
    int get_height() const;
    SDL_Texture* get_texture() const;
    void draw_on(RenderTexture &target, const Rectangle& region, const Point& dst_position, uint8_t opacity = 255) const;
    void clear(const Color &clearColor = Color::transparent);
    void clear(const Rectangle& where,const Color& clearColor = Color::transparent);
    ~RenderTargetView();
private:
    void clear_sub(const Rectangle& where, const Color &clearColor);
    Rectangle rect;
    Bin& bin;
    Atlas& atlas;
    mutable bool tex_dirty;
};

using RenderTargetViewPtr = std::unique_ptr<RenderTargetView>;

struct Atlas {
    Pack pack;
    SDL_Texture_UniquePtr target;

    Atlas(unsigned width,unsigned height) : pack(width,height) {
      SDL_Texture* t = SDL_CreateTexture(Video::get_renderer(),
                                         Video::get_pixel_format()->format,
                                         SDL_TEXTUREACCESS_TARGET,
                                         width,height);
      target.reset(t);
    }
    void flush_to_front() {
        //TODO
    }
};

using AtlasPtr = std::unique_ptr<Atlas>;

class RenderTargetAtlas
{
public:
    RenderTargetAtlas();
    RenderTargetViewPtr create_view(int width, int height);
private:
    RenderTargetViewPtr create_view(Atlas&,Bin&) const;
    void expand();
    void repack();
    std::vector<AtlasPtr> mAtlases;
};

}
