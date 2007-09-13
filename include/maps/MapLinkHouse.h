#ifndef ZSDX_MAP_LINKHOUSE_H
#define ZSDX_MAP_LINKHOUSE_H

#include "Map.h"

enum {
  MAP_LINKHOUSE_INITIAL_STATE_SOUTH = 0,
  MAP_LINKHOUSE_INITIAL_STATE_EAST  = 1,
};

/*
 * Map generated automatically by the tileset editor.
 */
class MapLinkHouse: public Map {

 public:
  MapLinkHouse(void);
  inline ~MapLinkHouse(void) { }

  void load(void);
};

#endif
