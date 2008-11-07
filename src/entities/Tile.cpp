#include "entities/Tile.h"

/**
 * Constructor.
 * It is called by the subclasses. 
 * @param obstacle type of obstacle: OBSTACLE_NONE, OBSTACLE, OBSTACLE_TOP_RIGHT,
 * OBSTACLE_TOP_LEFT, OBSTACLE_BOTTOM_LEFT or OBSTACLE_BOTTOM_RIGHT
 * @param width width of the tile in pixels (must be a multiple of 8)
 * @param height height of the tile in pixels (must be a multiple of 8)
 */
Tile::Tile(MapEntity::Obstacle obstacle, int width, int height):
  obstacle(obstacle), width(width), height(height) {

  // check the width and the height
  if (width <= 0 || height <= 0 ||
      width % 8 != 0 || height % 8 != 0) {
    DIE("Invalid tile: the size is (" << width << "x" << height << ") but should be positive and multiple of 8 pixels");
  }

  // diagonal obstacle: check that the tile is square
  if (obstacle != MapEntity::OBSTACLE_NONE && obstacle != MapEntity::OBSTACLE
      && width != height) {
    DIE("Invalid tile: a tile with a diagonal obstacle must be square");
  }
}

/**
 * Destructor.
 */
Tile::~Tile(void) {

}
