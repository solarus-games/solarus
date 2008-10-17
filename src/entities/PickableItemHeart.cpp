#include "entities/PickableItemHeart.h"
#include "movements/PathMovement.h"
#include "Sprite.h"

/**
 * Translation vectors composing the movement of the falling heart.
 */
static const SDL_Rect falling_moves[24] = {
  { 0, 0}, { 0,-2}, { 0,-2}, { 0,-2}, { 0,-2}, { 0,-2}, { 0, 0}, { 0, 0},
  { 1, 1}, { 1, 1}, { 1, 0}, { 1, 1}, { 1, 1}, { 0, 0}, {-1, 0}, {-1, 1},
  {-1, 0}, {-1, 1}, {-1, 0}, {-1, 1}, { 0, 1}, { 1, 1}, { 1, 1}, {-1, 0}
};

/**
 * Creates a pickable heart.
 * @param map the map
 * @param layer layer of the heart to create on the map
 * @param x x coordinate of the heart to create
 * @param y y coordinate of the heart to create
 */
PickableItemHeart::PickableItemHeart(Map *map, Layer layer, int x, int y):
  PickableItem(map, layer, x, y, PickableItem::HEART, 0) {

}

/**
 * Destructor.
 */
PickableItemHeart::~PickableItemHeart(void) {

}

/**
 * Creates the sprite of this pickable heart.
 * Like the normal pickable items, a pickable heart is represented with two sprites:
 * the heart itself and its shadow. The difference is that the heart sprite has a special
 * animation when falling.
 */
void PickableItemHeart::initialize_sprites(void) {

  // first initialize the sprites like any item
  PickableItem::initialize_sprites();

  if (is_falling()) {
    // special animation of the heart when falling
    get_sprite(0)->set_current_animation("small_falling");
  }
}

/**
 * Initializes the movement of the heart (if it is falling).
 * The movement of a falling heart is completely different to the other
 * pickable items and the shadow is also moving.
 */
void PickableItemHeart::initialize_movement(void) {
  
  if (is_falling()) {
    set_movement(new PathMovement(falling_moves, 24, 100, false));
  }
}

/**
 * Updates the pickable heart.
 * This function is called repeatedly by the map.
 * This is a redefinition of PickableItem::update() to make
 * update the shadow position when the heart is falling.
 */
void PickableItemHeart::update(void) {

  PickableItem::update();

  if (is_falling()) {
    // move the shadow according to the heart x position
    shadow_x = get_x();
  }
}
