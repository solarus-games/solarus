#ifndef ZSDX_ENTRANCE_H
#define ZSDX_ENTRANCE_H

#include "Common.h"
#include "MapEntity.h"

/**
 * An entrance of the map.
 * An entrance is a possible positition of Link when he arrives on the map.
 */
class Entrance: public MapEntity {

 public:

  Entrance(string name, Layer layer, int link_x, int link_y, int link_direction);
  ~Entrance(void);

  void display_on_map(Map *map);
};

#endif
