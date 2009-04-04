#ifndef ZSDX_CUSTOM_OBSTACLE_H
#define ZSDX_CUSTOM_OBSTACLE_H

#include "Common.h"
#include "entities/MapEntity.h"
#include <map>

/**
 * This type of entity is an invisible obstacle for some other types of entities.
 */
class CustomObstacle: public MapEntity {

 private:

  std::map<int, bool> entity_types_stopped;

 public:

  CustomObstacle(std::string name, Layer layer, int x, int y, int width, int height,
		 bool stops_hero, bool stops_enemies, bool stops_npcs, bool stops_blocks);
  ~CustomObstacle(void);

  EntityType get_type(void);
  bool is_obstacle_for(MapEntity *other);
};

#endif
