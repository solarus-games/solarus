/**
 * This module defines the class EntityDetector.
 */
#include "EntityDetector.h"
#include "ZSDX.h"

/**
 * Constructor.
 * @param layer layer of the detector
 * @param x x position of the detector's rectangle
 * @param y y position of the detector's rectangle
 * @param w width of the detector's rectangle
 * @param h height of the detector's rectangle
 */
EntityDetector::EntityDetector(Layer layer, int x, int y, int width, int height):
MapEntity(layer, x, y, width, height) {
  
}

/**
 * Displays the entity on the map.
 * This function does nothing because by default, an entity detector is invisible.
 * @param map the map
 */
void EntityDetector::display_on_map(Map *map) {
  
}

/**
 * This function is called by the engine when an entity overlaps the detector.
 * The map is notified.
 * @param entity_overlapping the entity overlapping the detector
 */
void EntityDetector::entity_overlaps(MapEntity *entity_overlapping) {

  Map *map = ZSDX::game->get_current_map();
  map->event_entity_on_detector(this, entity_overlapping);
}
