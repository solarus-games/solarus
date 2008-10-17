#include "entities/Exit.h"
#include "ZSDX.h"
#include "Game.h"

/**
 * Constructor.
 * @param name name of the exit
 * @param layer layer of the exit
 * @param x x position of the exit's rectangle
 * @param y y position of the exit's rectangle
 * @param width width of the exit's rectangle
 * @param height height of the exit's rectangle
 * @param transition_style style of transition between the two maps
 * @param destination_map_id id of the destination map
 * @param entrance_name initial state on the destination map
 */
Exit::Exit(string name, MapEntity::Layer layer, int x, int y, int width, int height,
	   Transition::Style transition_style, MapId destination_map_id, string entrance_name):
  Detector(COLLISION_ORIGIN_POINT, name, layer, x, y, width, height),
  transition_style(transition_style), destination_map_id(destination_map_id), entrance_name(entrance_name) {
  
}

/**
 * Destructor.
 */
Exit::~Exit(void) {
}

/**
 * This function is called by the engine when an entity overlaps the exits.
 * This is a redefinition of Detector::collision().
 * The map is not notified anymore: here we just make Link leave the map.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Exit::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  
  if (entity_overlapping->is_hero()) {
    zsdx->game->set_current_map(destination_map_id, entrance_name, transition_style);
  }
}
