/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/PickableItemHeart.h"
#include "movements/PixelMovement.h"
#include "Sprite.h"

/**
 * @brief Translation vectors that define the movement of the falling heart.
 */
static const Rectangle falling_moves[24] = {
  Rectangle( 0, 0), Rectangle( 0,-2), Rectangle( 0,-2), Rectangle( 0,-2), Rectangle( 0,-2), Rectangle( 0,-2),
  Rectangle( 0, 0), Rectangle( 0, 0), Rectangle( 1, 1), Rectangle( 1, 1), Rectangle( 1, 0), Rectangle( 1, 1),
  Rectangle( 1, 1), Rectangle( 0, 0), Rectangle(-1, 0), Rectangle(-1, 1), Rectangle(-1, 0), Rectangle(-1, 1),
  Rectangle(-1, 0), Rectangle(-1, 1), Rectangle( 0, 1), Rectangle( 1, 1), Rectangle( 1, 1), Rectangle(-1, 0)
};

/**
 * @brief Creates a pickable heart.
 * @param layer layer of the heart to create on the map
 * @param x x coordinate of the heart to create
 * @param y y coordinate of the heart to create
 */
PickableItemHeart::PickableItemHeart(Layer layer, int x, int y):
  PickableItem(layer, x, y, PickableItem::HEART, 0) {

}

/**
 * @brief Destructor.
 */
PickableItemHeart::~PickableItemHeart(void) {

}

/**
 * @brief Creates the sprite of this pickable heart.
 *
 * Like the normal pickable items, a pickable heart is represented with two sprites:
 * the heart itself and its shadow. The difference is that the heart sprite has a special
 * animation when falling.
 */
void PickableItemHeart::initialize_sprites(void) {

  // first initialize the sprites like any item
  PickableItem::initialize_sprites();

  if (is_falling()) {
    // special animation of the heart when falling
    get_sprite()->set_current_animation("small_falling");
  }
}

/**
 * @brief Initializes the movement of the heart (if it is falling).
 *
 * The movement of a falling heart is completely different to the other
 * pickable items and the shadow is also moving.
 */
void PickableItemHeart::initialize_movement(void) {
  
  if (is_falling()) {
    set_movement(new PixelMovement(falling_moves, 24, 100, false, false));
  }
}

/**
 * @brief Updates the pickable heart.
 *
 * This function is called repeatedly by the map.
 * This is a redefinition of PickableItem::update() to
 * update the shadow position when the heart is falling.
 */
void PickableItemHeart::update(void) {

  PickableItem::update();

  if (is_falling()) {
    // move the shadow according to the heart x position
    shadow_x = get_x();
  }
}

