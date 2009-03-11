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

  TilePattern *tile_pattern;   /**< pattern of the tile: instance of SimpleTile or AnimatedTile */

 public:

  TileOnMap(TilePattern *tile_pattern, Layer layer, int x, int y, int width, int height);
  ~TileOnMap(void);

  EntityType get_type(void);
  void display_on_map(void);

};

#endif
