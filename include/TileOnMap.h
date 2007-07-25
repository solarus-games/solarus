/**
 * This module defines the class TileOnMap.
 */

#ifndef ZSDX_TILE_ON_MAP_H
#define ZSDX_TILE_ON_MAP_H

#include "MapEntity.h"
#include "Tile.h"

/**
 * A tile on a map is a small fixed piece of the map background.
 * It is composed of a tile (SimpleTile or AnimatedTile)
 * and how it is placed on the map: its position and an x or y repetition of the pattern.
 */
class TileOnMap: public MapEntity {

 private:

  /**
   * Pattern of the tile.
   * It can be an instance of SimpleTile or AnimatedTile.
   */
  Tile *tile;

  /**
   * Layer of the tile: LAYER_BELOW, LAYER_INTERMEDIATE or LAYER_ABOVE.
   * TODO: move into MapEntity because of the active objects?
   */
  const Layer layer;

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
   * Creates a new tile on the map without repeating its pattern.
   * It is equivalent to TileOnMap(tile, position_in_map, 1, 1).
   * @param tile_image image of the tile (SimpleTile or AnimatedTile)
   * @param position_in_map position of the tile on the map
   * @param layer layer of the tile
   */
  TileOnMap(Tile *tile, SDL_Rect &position_in_map, Layer layer);

  /**
   * Creates a new tile.
   * @param tile image of the tile (SimpleTile or AnimatedTile)
   * @param position_in_map position of the tile on the map
   * @param layer layer of the tile
   * @param repeat_x number of times the pattern is repeated on x
   * @param repeat_y number of times the pattern is repeated on y
   */
  TileOnMap(Tile *tile, SDL_Rect &position_in_map, Layer layer, int repeat_x, int repeat_y);

  /**
   * Destructor.
   */
  inline ~TileOnMap() { }

  /**
   * Displays the tile on the map.
   * @param map the map
   */
  void display_on_map(Map *map);
};

#endif
