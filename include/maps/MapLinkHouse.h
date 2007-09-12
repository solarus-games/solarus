#ifndef ZSDX_MAP_LINKHOUSE_H
#define ZSDX_MAP_LINKHOUSE_H

#include "Map.h"

/*
 * Map generated automatically by the tileset editor.
 */
class MapLinkHouse: public Map {

 public:
  MapInitialState initial_state_south;
  MapInitialState initial_state_east;

  MapLinkHouse(void);
  inline ~MapLinkHouse(void) { }

  void load(void);
};

#endif
