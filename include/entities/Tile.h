#ifndef ZSDX_TILE_H
#define ZSDX_TILE_H

#include <istream>
#include "Common.h"
#include "MapEntity.h"

/**
 * A tile is a small fixed piece of the map background.
 * It is composed of a tile pattern that can be repeated.
 */
class Tile: public MapEntity {

 private:

  TilePattern *tile_pattern;   /**< pattern of the tile: instance of SimpleTile or AnimatedTile */

 public:

  Tile(TilePattern *tile_pattern, Layer layer, int x, int y, int width, int height);
  ~Tile(void);

  EntityType get_type(void);
  void display_on_map(void);

};

#endif
