#include "solarus/graphics/SurfaceImpl.h"

namespace Solarus {

SurfaceImpl::SurfaceImpl()
{

}

const Surface& SurfaceImpl::parent() const {
    return *_parent;
}

SurfaceImpl::~SurfaceImpl() {

}

}
