#ifndef ZSDX_SPRITE_ON_MAP_H
#define ZSDX_SPRITE_ON_MAP_H

#include "Common.h"
#include "MapEntity.h"
#include "Layer.h"

/**
 * A map entity displayed by a sprite.
 */
class SpriteOnMap: public MapEntity {

 protected:

  /**
   * The sprite displayed.
   */
  Sprite *sprite;

  void set_sprite(SpriteAnimationsId id);

 public:

  SpriteOnMap(Layer layer, int x, int y);
  SpriteOnMap(SpriteAnimationsId id, Layer layer, int x, int y, int width, int height);
  ~SpriteOnMap(void);
};

#endif
