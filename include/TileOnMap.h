/**
 * This module defines the class Tile.
 */

#ifndef ZSDX_TILE_H
#define ZSDX_TILE_H

#include "map_entity.h"
#include "tile_image.h"

/**
 * A tile is a small fixed piece of the map background.
 * It is composed of an image (SimpleTileImage or AnimatedTileImage)
 * and a position on the map, possibly with an x or y repetition of the pattern.
 */
class Tile: public MapEntity {

 private:

  /**
   * Image of the tile.
   * It can be an instance of SimpleTileImage or AnimatedTileImage
   */
  TileImage *tile_image;

  /**
   * Number of times the pattern is repeated on x.
   */
  const int repeat_x;

  /**
   * Number of times the pattern is repeated on y.
   */
  const int repeat_y;

 public:

  /**
   * Creates a new tile without repeating its pattern.
   * It is equivalent to Tile(tile_image, position_in_map, 1, 1).
   * @param tile_image image of the tile (SimpleTileImage or AnimatedTileImage)
   * @param position_in_map position of the tile on the map
   */
  Tile(TileImage *tile_image, SDL_Rect &position_in_map);

  /**
   * Creates a new tile.
   * @param tile_image image of the tile (SimpleTileImage or AnimatedTileImage)
   * @param position_in_map position of the tile on the map
   * @param repeat_x number of times the pattern is repeated on x
   * @param repeat_y number of times the pattern is repeated on y
   */
  Tile(TileImage *tile_image, SDL_Rect &position_in_map, int repeat_x, int repeat_y);

  /**
   * Destructor.
   */
  inline ~Tile() { }

  /**
   * Displays the tile on the map.
   * @param map the map
   */
  void display_on_map(Map *map);
};

#endif
