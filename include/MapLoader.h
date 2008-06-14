#ifndef ZSDX_MAP_LOADER_H
#define ZSDX_MAP_LOADER_H

#include "Common.h"

/**
 * This class parses a map file.
 */
class MapLoader {

 public:
  MapLoader(void);
  ~MapLoader(void);

  void load_map(Map *map);

};

#endif
