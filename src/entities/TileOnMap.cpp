#include "entities/TileOnMap.h"
#include "entities/Tileset.h"
#include "entities/Tile.h"
#include "Map.h"

/**
 * Creates a new tile on a map.
 * @param tile the tile in the tileset (SimpleTile or AnimatedTile)
 * @param layer layer of the tile
 * @param x x position of the tile on the map
 * @param y y position of the tile on the map
 * @param repeat_x number of times the pattern is repeated on x
 * @param repeat_y number of times the pattern is repeated on y
 */
TileOnMap::TileOnMap(Tile *tile, Layer layer, int x, int y, int repeat_x, int repeat_y):
  MapEntity(layer, x, y, tile->get_width() * repeat_x, tile->get_height() * repeat_y),
  tile(tile), repeat_x(repeat_x), repeat_y(repeat_y) {

}

/**
 * Destructor.
 */
TileOnMap::~TileOnMap(void) {

}

/**
 * Displays the tile on the map.
 */
void TileOnMap::display_on_map(void) {
  SDL_Surface *map_surface = map->get_visible_surface();
  SDL_Surface *tileset_image = map->get_tileset()->get_image();

  SDL_Rect dst;
  SDL_Rect *screen_position = map->get_screen_position();

  dst.w = tile->get_width();
  dst.h = tile->get_height();

  int limit_x = position_in_map.x - screen_position->x + position_in_map.w;
  int limit_y = position_in_map.y - screen_position->y + position_in_map.h;

  for (dst.y = position_in_map.y - screen_position->y; dst.y < limit_y; dst.y += dst.h) {

    if (dst.y <= 240 && dst.y + dst.h > 0) {

      for (dst.x = position_in_map.x - screen_position->x; dst.x < limit_x; dst.x += dst.w) {

	if (dst.x <= 320 && dst.x + dst.w > 0) {
	  tile->display(map_surface, dst, tileset_image);
	}
      }
    }
  }
}
