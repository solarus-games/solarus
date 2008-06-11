#include "MapExit.h"
#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include "Link.h"

/**
 * Constructor.
 * @param name name of the exit
 * @param layer layer of the exit
 * @param x x position of the exit's rectangle
 * @param y y position of the exit's rectangle
 * @param width width of the exit's rectangle
 * @param height height of the exit's rectangle
 * @param transition_type type of transition between the two maps
 * @param destination_map_id id of the destination map
 * @param entrance_name initial state on the destination map
 */
MapExit::MapExit(string name, Layer layer, int x, int y, int width, int height,
		 TransitionType transition_type, MapId destination_map_id, string entrance_name):
  EntityDetector(name, layer, x, y, width, height),
  transition_type(transition_type), destination_map_id(destination_map_id), entrance_name(entrance_name) {
  
}

/**
 * Destructor.
 */
MapExit::~MapExit(void) {

}

/**
 * This function is called by the engine when an entity overlaps the exits.
 * This is a redefinition of EntityDetector::entity_overlaps().
 * The map is not notified anymore: here we just make Link leave the map.
 * @param entity_overlapping the entity overlapping the detector
 */
void MapExit::entity_overlaps(MapEntity *entity_overlapping) {
  
  if (entity_overlapping == zsdx->game_resource->get_link()) {
    zsdx->game->set_current_map(destination_map_id, entrance_name, transition_type);
  }
}
