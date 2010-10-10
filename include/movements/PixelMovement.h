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
#ifndef SOLARUS_PIXEL_MOVEMENT_H
#define SOLARUS_PIXEL_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"

/**
 * @brief Movement of an entity that follows a predetermined sequence of pixel-precise moves.
 */
class PixelMovement: public Movement {

  private:

    // movement properties

    const Rectangle *translation_vectors; /**< The succession of translations.
					   * Each element of the array represents a move
					   * in pixels (only the x and y fields of the Rectangle are used). */
    const int nb_vectors;                 /**< Number of translation vectors: this is the size of the
					   * translation_vectors array. */

    uint32_t next_move_date;              /**< Date of the next move */
    uint32_t delay;                       /**< Delay in milliseconds between two translations. */

    const bool loop;                      /**< Should the movement return to the beginning once finished? */ 

    // current state

    int vector_index;                     /**< Current translation vector in the array. */
    bool finished;                        /**< Indicates whether the entity has reached the end of the trajectory
					   * (only possible when loop is false). */

  protected:

    PixelMovement(int nb_vectors, uint32_t delay, bool loop, bool ignore_obstacles);

    void set_translation_vectors(const Rectangle *translation_vectors);
    virtual void make_next_move();
    int get_vector_index();

  public:

    PixelMovement(const Rectangle *translation_vectors,
	int nb_vectors, uint32_t delay, bool loop, bool ignore_obstacles);
    virtual ~PixelMovement();

    void set_delay(uint32_t delay);
    virtual void update();
    virtual void set_suspended(bool suspended);
    bool is_started();
    bool is_finished();
    int get_length();

};

#endif

