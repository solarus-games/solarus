#ifndef ZSDX_TILE_ON_MAP_H
#define ZSDX_TILE_ON_MAP_H

#include <istream>
#include "Common.h"
#include "MapEntity.h"

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
   * Number of times the pattern is repeated on x.
   */
  const int repeat_x;

  /**
   * Number of times the pattern is repeated on y.
   */
  const int repeat_y;

 public:

  TileOnMap(Tile *tile, Layer layer, int x, int y, int repeat_x, int repeat_y);
  ~TileOnMap(void);

  void display_on_map(void);

};

#endif
