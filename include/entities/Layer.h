#ifndef ZSDX_LAYER_H
#define ZSDX_LAYER_H

/**
 * Layer of a tile or a dynamic entity.
 */
enum Layer {
  LAYER_LOW,             /**< the entity is always below the hero (floor, walls, chests,
			      enemies and 99% of the tiles and active objects) */
  LAYER_INTERMEDIATE,    /**< the hero can be below or above the entity (platforms or objects on a plaform) */
  LAYER_HIGH,            /**< the entity is always above the hero (trees, top of doors...) */
  LAYER_NB               /**< number of layers */
};

#endif
