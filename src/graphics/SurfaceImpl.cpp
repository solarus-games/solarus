#include "solarus/graphics/SurfaceImpl.h"

namespace Solarus {

void SurfaceImpl::upload_surface() {
  Rectangle rect(0,0,get_width(),get_height());
  SDL_Surface* surface = get_surface();
  SDL_UpdateTexture(get_texture(),
                    rect,
                    surface->pixels,
                    surface->pitch
                    );
}

const Surface& SurfaceImpl::parent() const {
    return *_parent;
}

SurfaceImpl::~SurfaceImpl() {

}

}
