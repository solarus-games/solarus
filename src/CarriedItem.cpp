#include "CarriedItem.h"
#include "TransportableItem.h"
#include "Link.h"
#include "MovementPath.h"
#include "FollowMovement.h"
#include "Layer.h"

/**
 * Movement of the item when Link is lifting it.
 */
static const SDL_Rect lift_translations[4][6] = {
  {{0,0}, {0,0}, {-3,-3}, {-5,-3}, {-5,-2}},
  {{0,0}, {0,0}, {0,-1}, {0,-1}, {0,0}},
  {{0,0}, {0,0}, {3,-3}, {5,-3}, {5,-2}},
  {{0,0}, {0,0}, {0,-10}, {0,-12}, {0, 0}},
};

/**
 * Creates a carried item (i.e. an item carried by Link)
 * based on a transportable item (i.e. the item placed on the map
 * before Link lifts it).
 * @param transportable_item a transportable item
 */
CarriedItem::CarriedItem(Link *link, TransportableItem *transportable_item):
  MapEntity(), link(link), is_lifting(true) {

  // put the item on Link's layer
  set_layer(link->get_layer());

  // align correctly the item with Link
  int direction = link->get_animation_direction();
  if (direction % 2 == 0) {
    set_x(transportable_item->get_x());
    set_y(link->get_y());
  }
  else {
    set_x(link->get_x());
    set_y(transportable_item->get_y());
  }

  // create the movement and the sprite
  MovementPath *movement = new MovementPath(lift_translations[direction], 6, 100, false);
  create_sprite(transportable_item->get_sprite_animations_id());
  set_movement(movement);
}

/**
 * Destructor.
 */
CarriedItem::~CarriedItem(void) {

}

/**
 * This function is called repeatedly by Link.
 * When the lift movement finishes, Link starts carrying the item.
 */
void CarriedItem::update(void) {

  // update the sprite and the position
  MapEntity::update();
  
  // when Link finishes lifting the item, start carrying it
  if (is_lifting) {
    MovementPath *movement = (MovementPath*) get_movement();
    if (movement->is_finished()) {
      is_lifting = false;
      link->start_carrying();
      set_movement(new FollowMovement(link, 0, -18));
    }
  }
}
