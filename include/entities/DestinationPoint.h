#ifndef ZSDX_DESTINATION_POINT_H
#define ZSDX_DESTINATION_POINT_H

#include "Common.h"
#include "MapEntity.h"

/**
 * A destination point on the map.
 * A destination point is a location where the hero can arrive when using a teletransporter.
 */
class DestinationPoint: public MapEntity {

 private:

  bool is_visible;
  bool change_direction;

 public:

  DestinationPoint(std::string name, Layer layer, int x, int y, int hero_direction, bool is_visible);
  ~DestinationPoint(void);

  EntityType get_type(void);

  void place_hero(void);
};

#endif
