#ifndef ZSDX_LINKHOUSE_H
#define ZSDX_LINKHOUSE_H

#include "Map.h"

class MapLinkHouse: public Map {

 private:

 public:
  MapLinkHouse(void);
  inline ~MapLinkHouse(void) { }

  void load(void);
};

#endif
