/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/AnimatedTilePattern.h"
#include "solarus/entities/ParallaxScrollingTilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Surface.h"

namespace Solarus {

/**
 * \brief Interval in millisecond between two frames of an animation.
 */
static constexpr uint32_t TILE_FRAME_INTERVAL = 250;

/**
 * \brief Gives the current frame depending on the sequence type and the frame counter.
 *
 * This array associates the current frame (0, 1 or 2) to
 * the sequence type and the frame counter (0 to 11).
 */
static constexpr short frames[2][12] = {
  {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}, // sequence 0-1-2
  {0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1}, // sequence 0-1-2-1
};

int AnimatedTilePattern::frame_counter = 0;

int AnimatedTilePattern::current_frames[3] = {0, 0, 0};

uint32_t AnimatedTilePattern::next_frame_date = 0;

/**
 * \brief Constructor.
 * \param ground Kind of ground of the tile pattern.
 * \param sequence animation sequence type
 * \param size Size of the pattern (the same for each frame of the animation)
 * \param x1 x position of the first frame in the tileset
 * \param y1 y position of the first frame in the tileset
 * \param x2 x position of the second frame in the tileset
 * \param y2 y position of the second frame in the tileset
 * \param x3 x position of the third frame in the tileset
 * \param y3 y position of the third frame in the tileset
 * \param parallax true to also set parallax scrolling to the tile pattern
 */
AnimatedTilePattern::AnimatedTilePattern(Ground ground,
    AnimationSequence sequence,
    const Size& size,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    bool parallax):
  TilePattern(ground, size),
  sequence(sequence),
  parallax(parallax) {

  this->position_in_tileset[0] = Rectangle(x1, y1);
  this->position_in_tileset[1] = Rectangle(x2, y2);
  this->position_in_tileset[2] = Rectangle(x3, y3);

  for (int i = 0; i < 3; i++) {
    this->position_in_tileset[i].set_size(size);
  }
}

/**
 * \brief Updates the current frame of all tiles.
 *
 * This function is called repeatedly by the map.
 */
void AnimatedTilePattern::update() {

  uint32_t now = System::now();

  while (now >= next_frame_date) {

    frame_counter = (frame_counter + 1) % 12;
    current_frames[1] = frames[0][frame_counter];
    current_frames[2] = frames[1][frame_counter];

    next_frame_date += TILE_FRAME_INTERVAL; // the frame changes every 250 ms
  }
}

/**
 * \brief Draws the tile image on a surface.
 * \param dst_surface the surface to draw
 * \param dst_position position where tile pattern should be drawn on dst_surface
 * \param tileset the tileset of this tile
 * \param viewport coordinates of the top-left corner of dst_surface relative
 * to the map (may be used for scrolling tiles)
 */
void AnimatedTilePattern::draw(
    const SurfacePtr& dst_surface,
    const Point& dst_position,
    Tileset& tileset,
    const Point& viewport
) {
  const SurfacePtr& tileset_image = tileset.get_tiles_image();
  const Rectangle& src = position_in_tileset[current_frames[sequence]];
  Point dst = dst_position;

  if (parallax) {
    dst += viewport / ParallaxScrollingTilePattern::ratio;
  }

  tileset_image->draw_region(src, dst_surface, dst);
}

/**
 * \brief Returns whether tiles having this tile pattern are drawn at their
 * position.
 *
 * Usually, this function returns true, and when it is the case, drawn() is
 * called only for tiles that are located in the current viewport.
 *
 * However, some tile patterns may want to be drawn even when they are not
 * in the viewport, typically to make an illusion of movement like parallax
 * scrolling.
 *
 * \return true to if this tile pattern is always drawn at its coordinates
 */
bool AnimatedTilePattern::is_drawn_at_its_position() const {
  return !parallax;
}

}

