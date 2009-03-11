#include "entities/TilePattern.h"
#include "entities/Tileset.h"
#include "Map.h"

/**
 * Constructor.
 * It is called by the subclasses. 
 * @param obstacle type of obstacle
 * @param width width of the tile in pixels (must be a multiple of 8)
 * @param height height of the tile in pixels (must be a multiple of 8)
 */
TilePattern::TilePattern(MapEntity::Obstacle obstacle, int width, int height):
  obstacle(obstacle), width(width), height(height) {

  // check the width and the height
  if (width <= 0 || height <= 0 ||
      width % 8 != 0 || height % 8 != 0) {
    DIE("Invalid tile pattern: the size is (" << width << "x" << height <<
	") but should be positive and multiple of 8 pixels");
  }

  // diagonal obstacle: check that the tile is square
  if (obstacle >= MapEntity::OBSTACLE_TOP_RIGHT && obstacle <= MapEntity::OBSTACLE_BOTTOM_RIGHT
      && width != height) {
    DIE("Invalid tile pattern: a tile pattern with a diagonal obstacle must be square");
  }
}

/**
 * Destructor.
 */
TilePattern::~TilePattern(void) {

}

/**
 * Returns the width of the tile.
 * @return the width of the tile 
 */
int TilePattern::get_width(void) const {
  return width;
}

/**
 * Returns the height of the tile.
 * @return the height of the tile 
 */
int TilePattern::get_height(void) const {
  return height;
}

/**
 * Returns the obstacle property of the tile.
 * @return the obstacle property of the tile
 */
MapEntity::Obstacle TilePattern::get_obstacle(void) const {
  return obstacle;
}

/**
 * Displays the tile pattern on the map, repeating it to fit the specified size.
 * @param map the map
 * @param position_in_map location and size of the tile on the map
 */
void TilePattern::display_on_map(Map *map, SDL_Rect &position_in_map) {

  SDL_Surface *map_surface = map->get_visible_surface();
  SDL_Surface *tileset_image = map->get_tileset()->get_image();

  SDL_Rect dst;
  SDL_Rect *camera_position = map->get_camera_position();

  dst.w = get_width();
  dst.h = get_height();

  int limit_x = position_in_map.x - camera_position->x + position_in_map.w;
  int limit_y = position_in_map.y - camera_position->y + position_in_map.h;

  for (dst.y = position_in_map.y - camera_position->y; dst.y < limit_y; dst.y += dst.h) {

    if (dst.y <= 240 && dst.y + dst.h > 0) {

      for (dst.x = position_in_map.x - camera_position->x; dst.x < limit_x; dst.x += dst.w) {

	if (dst.x <= 320 && dst.x + dst.w > 0) {
	  display(map_surface, dst, tileset_image);
	}
      }
    }
  }
}
