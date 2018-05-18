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


SurfaceImpl::~SurfaceImpl() {

}

/**
 * @brief is_premultiplied
 * @return
 */
bool SurfaceImpl::is_premultiplied() const {
  return premultiplied;
}

/**
 * @brief set_premultiplied
 */
void SurfaceImpl::set_premultiplied(bool a_premultiplied) {
  premultiplied = a_premultiplied;
}
}
