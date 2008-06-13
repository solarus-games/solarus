#include "EntityDetector.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"

/**
 * Constructor.
 * @param name a name identifying the detector (or an empty string if you
 * don't need to identify the detector)
 * @param layer layer of the detector
 * @param x x position of the detector's rectangle
 * @param y y position of the detector's rectangle
 * @param width width of the detector's rectangle
 * @param height height of the detector's rectangle
 */
EntityDetector::EntityDetector(string name, Layer layer, int x, int y, int width, int height):
  MapEntity(name, 0, layer, x, y, width, height) {
  
}

/**
 * Destructor.
 */
EntityDetector::~EntityDetector(void) {

}

/**
 * This function is called by the map when an entity has just moved.
 * It checks whether the entity is overlapping this dector. Is so,
 * the method entity_overlaps() is called.
 * @param entity the entity
 */
void EntityDetector::check_entity_overlapping(MapEntity *entity) {

  if (get_layer() == entity->get_layer() // the entity is in the same layer as the detector
      && entity->is_origin_point_in(get_position_in_map())) { // the entity is in the detector's rectangle
    
    entity_overlaps(entity); // notify the detector
  }
}

/**
 * This function is called by check_entity_overlapping()
 * when an entity overlaps the detector.
 * By default, the map is notified.
 * @param entity_overlapping the entity overlapping the detector
 */
void EntityDetector::entity_overlaps(MapEntity *entity_overlapping) {

  Map *map = zsdx->game->get_current_map();
  map->event_entity_on_detector(this, entity_overlapping);
}
