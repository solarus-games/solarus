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
#ifndef SOLARUS_ANIMATED_TILE_PATTERN_H
#define SOLARUS_ANIMATED_TILE_PATTERN_H

#include "Common.h"
#include "entities/TilePattern.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Animated tile pattern.
 *
 * Unlike simple tile patterns that always display the same image,
 * an animated tile pattern displays three frames alternatively.
 */
class AnimatedTilePattern: public TilePattern {

  public:

    /**
     * @brief Tile animation sequence mode: 0-1-2 or 0-1-2-1.
     */
    enum AnimationSequence {
      ANIMATION_SEQUENCE_012  = 1,
      ANIMATION_SEQUENCE_0121 = 2 
    };

  private:

    // static variables to handle the animations of all tiles
    static int frame_counter;         /**< Frame counter (0 to 11), increased every 250 ms. */
    static int current_frames[3];     /**< Current frame (0 to 2) for both sequences. */
    static uint32_t next_frame_date;  /**< Date of the next frame change. */

    const AnimationSequence sequence; /**< Animation sequence type of this tile pattern: 0-1-2-1 or 0-1-2. */

    Rectangle position_in_tileset[3]; /**< Array of 3 rectangles representing the 3 animation frames
				       * of this tile pattern in the tileset image.
				       * The 3 frames should have the same width and height. */

  public:

    AnimatedTilePattern(Obstacle obstacle, AnimationSequence sequence,
	int width, int height, int x1, int y1, int x2, int y2, int x3, int y3);
    ~AnimatedTilePattern();

    static void update();
    void display(Surface *destination, const Rectangle &dst_position, Tileset &tileset);
};

#endif

