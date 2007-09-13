/**
 * This module defines the class MapExit.
 */
#include "MapExit.h"
#include "ZSDX.h"

/**
 * Constructor.
 * @param layer layer of the exit
 * @param x x position of the exit's rectangle
 * @param y y position of the exit's rectangle
 * @param w width of the exit's rectangle
 * @param h height of the exit's rectangle
 * @param map_id id of the destination map
 * @param initial_state_index initial state of the destination map
 */
MapExit::MapExit(Layer layer, int x, int y, int width, int height,
		 MapID destination_map_id, int initial_state_index):
EntityDetector(layer, x, y, width, height),
destination_map_id(destination_map_id), initial_state_index(initial_state_index) {
  
}

/**
 * This function is called by the engine when an entity overlaps the exits.
 * This is a redefinition of EntityDetector::entity_overlaps().
 * The map is not notified anymore: here we just make Link leave the map.
 * @param entity_overlapping the entity overlapping the detector
 */
void MapExit::entity_overlaps(MapEntity *entity_overlapping) {
  
  if (entity_overlapping == ZSDX::game_resource->get_link()) {
    ZSDX::game->set_current_map(destination_map_id, initial_state_index);
  }
}
