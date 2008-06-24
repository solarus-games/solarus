#include "PickableItemHeart.h"
#include "Sprite.h"
#include "MovementPath.h"

/**
 * Translation vectors composing the movement of the falling heart.
 */
static const SDL_Rect falling_moves[24] = {
  { 0, 0}, { 0,-1}, { 0,-1}, { 0,-1}, { 0,-1}, { 0,-1}, { 0, 0}, { 0, 0},
  { 1, 1}, { 1, 1}, { 1, 0}, { 1, 1}, { 1, 1}, { 0, 0}, {-1, 0}, {-1, 1},
  {-1, 0}, {-1, 1}, {-1, 0}, {-1, 1}, { 0, 1}, { 1, 1}, { 1, 1}, {-1, 0}
};

/**
 * Creates a pickable heart.
 * @param map the map
 * @param layer layer of the heart to create on the map
 * @param x x coordinate of the heart to create
 * @param y y coordinate of the heart to create
 * @param falling true to make the heart fall when it appears
 * @param disappear true to make the heart disappear after an amout of time
 */
PickableItemHeart::PickableItemHeart(Map *map, Layer layer, int x, int y,
				     bool falling, bool will_disappear):
  PickableItem(map, layer, x, y, PICKABLE_ITEM_HEART, falling, will_disappear) {

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

  cout << "is\n";

  // first initialize the sprites like any item
  PickableItem::initialize_sprites();

  if (falling) {
    // special animation of the heart when falling
    get_sprite(0)->set_current_animation("falling");
  }
}

/**
 * Initializes the movement of the heart (if it is falling).
 * The movement of a falling heart is completely different to the other
 * pickable items and the shadow is also moving.
 */
void PickableItemHeart::initialize_movement(void) {
  
  cout << "im\n";

  if (falling) {
    set_movement(new MovementPath(falling_moves, 24, 60, false));
  }
}

/**
 * Updates the pickable heart.
 * This function is called repeatedly by the map.
 * This is a redefinition of PickableItem::update() to make
 * update the shadow position when the heart is falling.
 */
void PickableItemHeart::update(void) {

  cout << "up\n";

  PickableItem::update();

  if (falling) {
    // move the shadow according to the heart x position
    shadow_x = get_x();
  }
}
