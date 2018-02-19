#include "solarus/graphics/SurfaceImpl.h"

namespace Solarus {

SurfaceImpl::SurfaceImpl()
{

}

SDL_Texture* SurfaceImpl::get_nonconst_texture() const {
  return const_cast<SDL_Texture*>(get_texture());
}

SDL_Surface* SurfaceImpl::get_nonconst_surface() const {
  return const_cast<SDL_Surface*>(get_surface());
}

const Surface& SurfaceImpl::parent() const {
    return *_parent;
}

SurfaceImpl::~SurfaceImpl() {

}

}
