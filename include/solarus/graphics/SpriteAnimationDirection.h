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
#ifndef SOLARUS_SPRITE_ANIMATION_DIRECTION
#define SOLARUS_SPRITE_ANIMATION_DIRECTION

#include "solarus/core/Common.h"
#include "solarus/core/Debug.h"
#include "solarus/core/PixelBits.h"
#include "solarus/core/Rectangle.h"
#include "solarus/graphics/Drawable.h"
#include <vector>

namespace Solarus {

class Surface;

/**
 * \brief A sequence of frames representing a sprite animated in a particular direction.
 *
 * An animation direction is a sequence of frames representing a sprite
 * with a specific animation and in a specific direction.
 */
class SpriteAnimationDirection {

  public:

    // creation and destruction
    SpriteAnimationDirection(
        const std::vector<Rectangle>& frames,
        const Point& origin
    );

    // size and origin point
    Size get_size() const;
    const Point& get_origin() const;

    // frames
    int get_nb_frames() const;
    const Rectangle& get_frame(int frame) const;
    void draw(Surface& dst_surface, const Point& dst_position,
        int current_frame, Surface& src_image, const DrawInfos &infos) const;

    // pixel collisions
    void enable_pixel_collisions(Surface& src_image);
    void disable_pixel_collisions();
    bool are_pixel_collisions_enabled() const;
    const PixelBits& get_pixel_bits(int frame) const;

  private:

    std::vector<Rectangle> frames;      /**< position of each frame of the sequence on the image */
    Point origin;                       /**< coordinates of the sprite's origin from the
                                         * upper-left corner of its image. */

    std::vector<PixelBits>
        pixel_bits;                    /**< bit masks representing the non-transparent pixels of each frame,
                                         * computed only if enable_pixel_collisions() is called */
};

/**
 * \brief Returns the origin point of a frame.
 * \return The origin point of a frame.
 */
inline const Point& SpriteAnimationDirection::get_origin() const {
  return origin;
}

/**
 * \brief Returns the pixel bits object of a frame.
 *
 * It represents the transparent bits of the frame and permits to detect
 * pixel-precise collisions.
 * The pixel collisions must be enabled.
 *
 * \param frame A frame of the animation.
 * \return The pixel bits object of a frame.
 */
inline const PixelBits& SpriteAnimationDirection::get_pixel_bits(int frame) const {

  SOLARUS_ASSERT(are_pixel_collisions_enabled(),
      "Pixel-precise collisions are not enabled for this sprite");
  SOLARUS_ASSERT(frame >= 0 && frame < get_nb_frames(), "Invalid frame number");

  return pixel_bits[frame];
}

}

#endif

