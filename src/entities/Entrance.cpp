#include "entities/Entrance.h"

/**
 * Constructor.
 */
Entrance::Entrance(string name, Layer layer, int link_x, int link_y, int link_direction):
  MapEntity(name, link_direction, layer, link_x, link_y, 16, 16) {

  set_origin(8, 13);
}

/**
 * Destructor.
 */
Entrance::~Entrance(void) {

}

/**
 * Displays the entrance on the map.
 * This function does nothing because an entrance is not visible.
 * @param map the map
 */
void Entrance::display_on_map(Map *map) {

}
