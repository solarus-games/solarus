#include "entities/DestinationPoint.h"
#include "entities/Link.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"

/**
 * Constructor.
 * @param name name of the destination point to create
 * @param layer the layer
 * @param x x position of the destination point to create
 * @param y y position of the destination point to create
 * @param link_direction initial direction of link in this state
 * (0 to 3, or -1 to indicate that Link's direction is not changed)
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

  if (is_visible) {
    create_sprite("entities/teletransporter");
    get_sprite()->set_current_animation("destination");
  }
}

/**
 * Destructor.
 */
DestinationPoint::~DestinationPoint(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType DestinationPoint::get_type() {
  return DESTINATION_POINT;
}

/**
 * Places Link on this destination point.
 */
void DestinationPoint::place_hero(void) {

  Link *link = zsdx->game->get_link();
  link->set_map(map, get_direction());
  link->set_x(get_x());
  link->set_y(get_y());
}
