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
    cout << "Invalid tile: the size must be positive and multiple of 8 pixels\n";
    exit(1);
  }

  // diagonal obstacle: check that the tile is square
  if (obstacle != MapEntity::OBSTACLE_NONE && obstacle != MapEntity::OBSTACLE
      && width != height) {
    cout << "Invalid tile: the size of a tile with a diagonal obstacle must be square\n";
    exit(1);
  }
}

/**
 * Destructor.
 */
Tile::~Tile(void) {

}
