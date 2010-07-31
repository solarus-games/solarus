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
#include "hero/StateJumping.h"

/**
 * Constructor.
 * @param hero the hero controlled by this state
 * @param direction8 direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param with_collisions true to stop the movement if there is a collision
 * @param with_sound true to play the "jump" sound
 * @param movement_delay delay between each one-pixel move in the jump movement (0: default)
 * @param layer_after_jump the layer to set when the jump is finished
 * (or LAYER_NB to keep the same layer)
 */
Hero::StateJumping::StateJumping(Hero *hero, int direction8, int length,
    bool with_collisions, bool with_sound,
    uint32_t movement_delay, Layer layer_after_jump):
  State(hero) {

}

/**
 * Destructor.
 */
Hero::StateJumping::~StateJumping(void) {

}

