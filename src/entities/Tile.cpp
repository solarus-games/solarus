#include "entities/Tile.h"

/**
 * Constructor.
 * It is called by the subclasses. 
 * @param obstacle type of obstacle
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

/**
 * Returns the width of the tile.
 * @return the width of the tile 
 */
int Tile::get_width(void) const {
  return width;
}

/**
 * Returns the height of the tile.
 * @return the height of the tile 
 */
int Tile::get_height(void) const {
  return height;
}

/**
 * Returns the obstacle property of the tile.
 * @return the obstacle property of the tile
 */
MapEntity::Obstacle Tile::get_obstacle(void) const {
  return obstacle;
}
