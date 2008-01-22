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
  int initial_state_index;

 public:

  /**
   * Constructor.
   * @param layer layer of the exit
   * @param x x position of the exit's rectangle
   * @param y y position of the exit's rectangle
   * @param width width of the exit's rectangle
   * @param height height of the exit's rectangle
   * @param destination_map_id id of the destination map
   * @param initial_state_index initial state of the destination map
   */
  MapExit(Layer layer, int x, int y, int width, int height,
	  MapId destination_map_id, int initial_state_index);
  /**
   * Destructor.
   */
  virtual ~MapExit(void) { };

  /**
   * This function is called by the engine when an entity overlaps the exits.
   * This is a redefinition of EntityDetector::entity_overlaps().
   * The map is not notified anymore: here we just make Link leave the map.
   * @param entity_overlapping the entity overlapping the detector
   */
  void entity_overlaps(MapEntity *entity_overlapping);

};

#endif
