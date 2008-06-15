#include "MapEntrance.h"

/**
 * Constructor.
 */
MapEntrance::MapEntrance(string name, Layer layer, int link_x, int link_y, int link_direction):
  MapEntity(name, link_direction, layer, link_x, link_y, 16, 16) {

  set_origin(8, 13);
}

/**
 * Destructor.
 */
MapEntrance::~MapEntrance(void) {

}

/**
 * Displays the entrance on the map.
 * This function does nothing because an entrance is not visible.
 * @param map the map
 */
void MapEntrance::display_on_map(Map *map) {

}
