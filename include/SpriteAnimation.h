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
#ifndef SOLARUS_SPRITE_ANIMATION_H
#define SOLARUS_SPRITE_ANIMATION_H

#include "Common.h"

/**
 * @brief Stores the animated sequences of a sprite in a specific animation.
 *
 * Each animated sequence correspond to a direction of the sprite in this animation.
 */
class SpriteAnimation {

  private:

    Surface *src_image;          /**< image from which the frames are extracted;
				  * this image is the same for
				  * all directions of the sprite's animation */
    bool src_image_loaded;       /**< indicates that src_image was loaded from this class */
    const int nb_directions;     /**< number of directions of this animation */
    SpriteAnimationDirection **directions; /**< array of directions:
					    * each direction is a sequence of images */
    const uint32_t frame_delay;  /**< default interval in milliseconds between two frames
				  * (this delay is the same for all directions) */
    const int loop_on_frame;     /**< number of the frame to loop on, or -1 to make no loop */

  public:

    SpriteAnimation(const std::string &image_file_name, 
	int nb_directions, SpriteAnimationDirection **directions, uint32_t frame_interval, int loop_on_frame);
    virtual ~SpriteAnimation();

    void set_map(Map *map);

    int get_next_frame(int current_direction, int current_frame) const;
    void display(Surface *destination, int x, int y,
	int current_direction, int current_frame);

    int get_nb_directions() const;
    const SpriteAnimationDirection *get_direction(int direction) const;
    uint32_t get_frame_delay() const;
    bool is_looping() const;

    void enable_pixel_collisions();
    bool are_pixel_collisions_enabled() const;
};

#endif

