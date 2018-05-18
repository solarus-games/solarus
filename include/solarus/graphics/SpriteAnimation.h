/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/core/Debug.h"
#include "solarus/graphics/Drawable.h"
#include "solarus/graphics/SpriteAnimationDirection.h"
#include "solarus/graphics/SurfacePtr.h"
#include <string>
#include <vector>

namespace Solarus {

class Point;
class Tileset;

/**
 * \brief Stores the animated sequences of a sprite in a specific animation.
 *
 * Each animated sequence correspond to a direction of the sprite in this animation.
 */
class SpriteAnimation {

  public:

    SpriteAnimation(
        const std::string& image_file_name,
        const std::vector<SpriteAnimationDirection>& directions,
        uint32_t frame_interval,
        int loop_on_frame
    );

    void set_tileset(const Tileset& tileset);

    int get_next_frame(int current_direction, int current_frame) const;
    void draw(Surface& dst_surface, const Point& dst_position,
        int current_direction, int current_frame, const DrawInfos& infos) const;

    int get_nb_directions() const;
    const SpriteAnimationDirection& get_direction(int direction) const;
    uint32_t get_frame_delay() const;
    bool is_looping() const;

    void enable_pixel_collisions();
    bool are_pixel_collisions_enabled() const;

  private:

    void do_enable_pixel_collisions();
    void disable_pixel_collisions();

    SurfacePtr src_image;         /**< Image from which the frames are extracted,
                                   * or nullptr if missing or not loaded yet.
                                   * This image is the same for
                                   * all directions of the sprite's animation. */
    const bool
        src_image_is_tileset;     /**< Whether the image comes from the tileset. */
    std::vector<SpriteAnimationDirection>
        directions;               /**< List of directions:
                                   * each direction is a sequence of images. */
    const uint32_t frame_delay;   /**< Default interval in milliseconds between two frames
                                   * (same delay for all directions). */
    const int loop_on_frame;      /**< Number of the frame to loop on, or -1 to make no loop. */
    bool should_enable_pixel_collisions; /**< Whether pixel-perfect collisions should be
                                           * enabled as soon as the image becomes available. */
};

/**
 * \brief Returns a direction.
 * \param direction the direction
 * \return the sequence of images corresponding to this direction
 */
inline const SpriteAnimationDirection& SpriteAnimation::get_direction(
    int direction
) const {
  SOLARUS_ASSERT(direction >= 0 && direction < get_nb_directions(),
      "Invalid sprite direction");
  return directions[direction];
}

}

#endif

