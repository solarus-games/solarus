#include "entities/DestinationPoint.h"

/**
 * Constructor.
 * @param name name of the destination point to create
 * @param layer the layer
 * @param x x position of the destination point to create
 * @param y y position of the destination point to create
 * @param link_direction initial direction of link in this state (0 to 3, or -1
 * to indicate that Link's direction is not changed)
 * @param is_visible true to make the destination point visible
 */
DestinationPoint::DestinationPoint(string name, Layer layer, int x, int y,
				   int link_direction, bool is_visible):
  MapEntity(name, 0, layer, x, y, 16, 16) {

  set_origin(8, 13);
  this->change_direction = (direction != -1);
  if (change_direction) {
    set_direction(link_direction);
  }
}

/**
 * Destructor.
 */
DestinationPoint::~DestinationPoint(void) {

}

/**
 * Displays the destination point on the map if it is visible.
 * @param map the map
 */
void DestinationPoint::display_on_map(Map *map) {

  if (is_visible) {
    // TODO
  }
}
