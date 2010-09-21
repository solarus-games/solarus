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
#include "entities/AnimatedTilePattern.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"

/**
 * @brief Interval in millisecond between two frames of an animation.
 */
static const uint32_t TILE_FRAME_INTERVAL = 250;

/**
 * @brief Gives the current frame depending on the sequence type and the frame counter.
 *
 * This array associates the current frame (0, 1 or 2) to
 * the sequence type and the frame counter (0 to 11).
 */
static const short frames[2][12] = {
  {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}, // sequence 0-1-2
  {0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1}, // sequence 0-1-2-1
};

int AnimatedTilePattern::frame_counter = 0;

int AnimatedTilePattern::current_frames[3] = {0, 0, 0};

uint32_t AnimatedTilePattern::next_frame_date = 0;

/**
 * @brief Constructor.
 * @param obstacle is the tile pattern an obstacle?
 * @param sequence animation sequence type
 * @param width width of the tile (the same for each frame of the animation)
 * @param height height of the tile (the same for each frame of the animation)
 * @param x1 x position of the first frame in the tileset
 * @param y1 y position of the first frame in the tileset
 * @param x2 x position of the second frame in the tileset
 * @param y2 y position of the second frame in the tileset
 * @param x3 x position of the third frame in the tileset
 * @param y3 y position of the third frame in the tileset
 */
AnimatedTilePattern::AnimatedTilePattern(Obstacle obstacle,
					 AnimationSequence sequence,
					 int width, int height,
					 int x1, int y1,
					 int x2, int y2,
					 int x3, int y3):
  TilePattern(obstacle, width, height), sequence(sequence) {

  this->position_in_tileset[0] = Rectangle(x1, y1);
  this->position_in_tileset[1] = Rectangle(x2, y2);
  this->position_in_tileset[2] = Rectangle(x3, y3);

  for (int i = 0; i < 3; i++) {
    this->position_in_tileset[i].set_size(width, height);
  }
}

/**
 * @brief Destructor.
 */
AnimatedTilePattern::~AnimatedTilePattern() {
  
}

/**
 * @brief Updates the current frame of all tiles.
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
 * @brief Displays the tile on a surface.
 * @param destination the destination surface
 * @param dst_position position of the tile pattern on the destination surface
 * @param tileset_image the tileset image of this tile pattern
 */
void AnimatedTilePattern::display(Surface *destination, const Rectangle &dst_position, Surface *tileset_image) {
  tileset_image->blit(position_in_tileset[current_frames[sequence]], destination, dst_position);
}

