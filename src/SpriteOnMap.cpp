#include "SpriteOnMap.h"
#include "Sprite.h"

/**
 * Constructor.
 */
SpriteOnMap::SpriteOnMap(Layer layer, int x, int y):
  MapEntity(layer, x, y, 0, 0), sprite(NULL) {

}

/**
 * Constructor.
 */
SpriteOnMap::SpriteOnMap(SpriteAnimationsId id, Layer layer, int x, int y, int width, int height):
  MapEntity(layer, x, y, width, height), sprite(new Sprite(id)) {

}

/**
 * Destructor.
 */
SpriteOnMap::~SpriteOnMap(void) {

  delete sprite;
}

/**
 * Sets the sprite of this entity.
 */
void SpriteOnMap::set_sprite(SpriteAnimationsId id) {

  if (sprite != NULL) {
    delete sprite;
  }

  sprite = new Sprite(id);
}
