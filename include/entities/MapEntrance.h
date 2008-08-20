#ifndef ZSDX_MAP_ENTRANCE_H
#define ZSDX_MAP_ENTRANCE_H

#include "Common.h"
#include "MapEntity.h"

/**
 * Describes an entrance of a map, i.e. a possible state
 * of the map when it is loaded.
 */
class MapEntrance: public MapEntity {

 public:

  MapEntrance(string name, Layer layer, int link_x, int link_y, int link_direction);
  ~MapEntrance(void);

  void display_on_map(Map *map);
};

#endif
