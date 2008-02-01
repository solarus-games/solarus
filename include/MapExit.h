/**
 * This module defines the class MapExit.
 */

#ifndef ZSDX_MAP_EXIT_H
#define ZSDX_MAP_EXIT_H

#include "Common.h"
#include "EntityDetector.h"

/**
 * A map exit is a detector placed on a map.
 * When Link walks on this detector, he leaves the map and enters another one.
 */
class MapExit: public EntityDetector {

 private:

  /**
   * Id of the destination map.
   */
  MapId destination_map_id;

  /**
   * Initial state of the destination map.
   */
  int entrance_index;

 public:

  MapExit(Layer layer, int x, int y, int width, int height,
	  MapId destination_map_id, int entrance_index);
  virtual ~MapExit(void) { };

  void entity_overlaps(MapEntity *entity_overlapping);

};

#endif
