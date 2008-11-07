#include "entities/Teletransporter.h"
#include "ZSDX.h"
#include "Game.h"

/**
 * Constructor.
 * @param name name of the teletransporter
 * @param layer layer of the teletransporter
 * @param x x position of the teletransporter's rectangle
 * @param y y position of the teletransporter's rectangle
 * @param width width of the teletransporter's rectangle
 * @param height height of the teletransporter's rectangle
 * @param transition_style style of transition between the two maps
 * @param destination_map_id id of the destination map
 * @param destination_point_name location on the destination map, or "_same" to keep Link's coordinates,
 * or "_side" to place Link on the appropriate side of the map
 */
Teletransporter::Teletransporter(string name, MapEntity::Layer layer, int x, int y, int width, int height,
	   Transition::Style transition_style, MapId destination_map_id, string destination_point_name):
  Detector(COLLISION_ORIGIN_POINT, name, layer, x, y, width, height),
  transition_style(transition_style), destination_map_id(destination_map_id), destination_point_name(destination_point_name) {
  
}

/**
 * Destructor.
 */
Teletransporter::~Teletransporter(void) {
}

/**
 * This function is called by the engine when an entity overlaps the teletransporter.
 * This is a redefinition of Detector::collision().
 * The map is not notified anymore: here we just make Link leave the map.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Teletransporter::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  
  if (entity_overlapping->is_hero()) {
    zsdx->game->set_current_map(destination_map_id, destination_point_name, transition_style);
  }
}
