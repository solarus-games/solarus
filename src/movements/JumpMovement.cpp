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
#include "movements/JumpMovement.h"

/**
 * @brief Basic movement vector of each direction.
 */
const Rectangle JumpMovement::basic_translations[8] = {
  Rectangle( 1,  0), // right
  Rectangle( 1, -1),
  Rectangle( 0, -1),
  Rectangle(-1, -1),
  Rectangle(-1,  0),
  Rectangle(-1,  1),
  Rectangle( 0,  1),
  Rectangle( 1,  1)
};

/**
 * @brief Creates a jump movement.
 * @param direction8 of the movement (0 to 7)
 * @param length length of the jump
 * @param ignore_obstacles true to make the movement ignore obstacles
 * @param movement_delay delay between each one-pixel move in the jump movement in milliseconds (0: default)
 */
JumpMovement::JumpMovement(int direction8, int length, bool ignore_obstacles, uint32_t movement_delay):
  PixelMovement(length, 10, false, ignore_obstacles), jump_height(0) {

  // compute the path
  translation_vectors = new Rectangle[length];
  for (int i = 0; i < length; i++) {
    translation_vectors[i] = basic_translations[direction8];
  }
  set_translation_vectors(translation_vectors);

  // set the speed
  if (movement_delay == 0) {
    set_delay(std::max(4, 14 - length / 10));
  }
  else {
    set_delay(movement_delay);
  }
}

/**
 * @brief Destructor.
 */
JumpMovement::~JumpMovement() {
  delete[] translation_vectors;
}

/**
 * @brief Returns the height in pixels where the entity should be displayed over its shadow.
 * @return the height of the jump
 */
int JumpMovement::get_jump_height() {
  return jump_height;
}

/**
 * @brief Makes a move in the path.
 */
void JumpMovement::make_next_move() {

  PixelMovement::make_next_move();

  int i = get_vector_index();
  if (i == 1 || i == get_length()) {
    jump_height = 0;
  }
  else {
    int jump_sign = (i <= get_length() / 2) ? 1 : -1;
    int jump_unit = (get_length() <= 16) ? 2 : 1;
    
    jump_height += jump_sign * jump_unit;
  }
}

