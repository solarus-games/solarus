#ifndef ZSDX_DESTINATION_POINT_H
#define ZSDX_DESTINATION_POINT_H

#include "Common.h"
#include "MapEntity.h"

/**
 * A destination point on the map.
 * A destination point is a location where Link can arrive when using a teletransporter.
 */
class DestinationPoint: public MapEntity {

 private:

  bool is_visible;
  bool change_direction;

 public:

  DestinationPoint(string name, Layer layer, int link_x, int link_y, int link_direction, bool is_visible);
  ~DestinationPoint(void);

  EntityType get_type(void);

  void place_hero(void);
};

#endif
