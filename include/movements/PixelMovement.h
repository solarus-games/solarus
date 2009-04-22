/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_PIXEL_MOVEMENT_H
#define ZSDX_PIXEL_MOVEMENT_H

#include "Common.h"
#include "movements/MovementWithCollision.h"

/**
 * Movement for an entity that follows a predetermined succession
 * of per-pixel translation vectors.
 */
class PixelMovement: public MovementWithCollision {

 private:

  // movement properties

  /**
   * The succession of translations.
   * Each element of the array represents a move
   * in pixels (only the fields x and y of the SDL_Rect are used).
   */
  const SDL_Rect *translation_vectors;

  /**
   * Number of translation vectors: this is the size of the
   * translation_vectors array.
   */
  const int nb_vectors;

  /**
   * Delay in milliseconds between two translations.
   */
  Uint32 delay;

  /**
   * Should the movement return to the beginning once finished?
   */
  const bool loop;

  /**
   * Indicates whether this movement is sensitive to the obstacles.
   */
  const bool with_collisions;

  // current state

  /**
   * Current translation vector in the array.
   */
  int vector_index;

  /**
   * Indicates whether the entity has reached the end of the trajectory
   * (only possible when loop is false).
   */
  bool finished;

 protected:

  PixelMovement(int nb_vectors, Uint32 delay, bool loop, bool with_collisions);

  bool collision_with_map(int dx, int dy);

  void set_translation_vectors(const SDL_Rect *translation_vectors);
  virtual void make_next_move(void);
  int get_vector_index(void);
  int get_length(void);

 public:

  PixelMovement(const SDL_Rect *translation_vectors,
		int nb_vectors, Uint32 delay, bool loop, bool with_collisions);
  virtual ~PixelMovement(void);

  void set_delay(Uint32 delay);
  virtual void update(void);
  bool is_finished(void);

};

#endif
