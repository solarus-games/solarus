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
#include "SpriteAnimationDirection.h"
#include "lowlevel/PixelBits.h"

/**
 * Constructor.
 * @param nb_frames number of frames in the sequence
 * @param frames position of each frames of the sequence in the image
 * (the pointer is copied, not the array, so don't modify after calling this constructor)
 * @param x_origin x coordinate of the sprite's origin
 * @param y_origin y coordinate of the sprite's origin
 */
SpriteAnimationDirection::SpriteAnimationDirection(int nb_frames, Rectangle *frames,
						   int x_origin, int y_origin):
  nb_frames(nb_frames), frames(frames), pixel_bits(NULL) {

  origin.set_xy(x_origin, y_origin);
}

/**
 * Destructor.
 */
SpriteAnimationDirection::~SpriteAnimationDirection(void) {
  delete[] frames;

  if (pixel_bits != NULL) {
    for (int i = 0; i < nb_frames; i++) {
      delete pixel_bits[i];
    }
    delete[] pixel_bits;
  }
}

/**
 * Returns the size of a frame.
 * @return the size of a frame
 */
const Rectangle & SpriteAnimationDirection::get_size(void) {
  return frames[0];
}

/**
 * Returns the origin point of a frame.
 * @return the origin point of a frame
 */
const Rectngle & SpriteAnimationDirection::get_origin(void) {
  return origin;
}

/**
 * Returns the number of frames in this direction.
 * @return the number of frames
 */
int SpriteAnimationDirection::get_nb_frames(void) {
  return nb_frames;
}

/**
 * Returns the rectangle representing the specified frame on the source image.
 * @param frame a frame number
 * @return the rectangle of this frame
 */
const Rectangle & SpriteAnimationDirection::get_frame(int frame) {
  return frames[frame];
}

/**
 * Displays a specific frame on the map.
 * @param destination the surface on which the frame will be displayed
 * @param x x coordinate of the sprite on this surface
 * (the origin point will be displayed at this position)
 * @param y y coordinate of the sprite on this surface
 * (the origin point will be displayed at this position)
 * @param current_frame the frame to show
 * @param src_image the image from wich the frame is extracted
 */
void SpriteAnimationDirection::display(Surface *destination, int x, int y,
				       int current_frame, Surface *src_image) {

  Rectangle position_top_left; // position of the sprite's upper left corner

  const Rectangle &current_frame_rect = frames[current_frame];

  position_top_left.set_xy(x - origin.x, y - origin.y);
  position_top_left.set_size(current_frame_rect.get_width(), current_frame_rect.get_height());

  src_image->blit(current_frame_rect, destination, &position_top_left);
}

/**
 * Calculates the bit fields representing the non-transparent pixels
 * of the images in this direction.
 * This method has to be called if you want a sprite with this animations
 * to be able to detect pixel-perfect collisions.
 * Nothing happens if the pixel-perfect collisions were already enabled.
 */
void SpriteAnimationDirection::enable_pixel_collisions(Surface *src_image) {

  if (pixel_bits == NULL) {
    pixel_bits = new PixelBits*[nb_frames];
    for (int i = 0; i < nb_frames; i++) {
      pixel_bits[i] = new PixelBits(src_image, frames[i]);
    }
  }
}

/**
 * Returns whether the pixel-perfect collisions are enabled for this direction.
 * @return true if the pixel-perfect collisions are enabled
 */
bool SpriteAnimationDirection::are_pixel_collisions_enabled(void) {
  return pixel_bits != NULL;
}

/**
 * Returns the pixel bits object of a frame.
 * It represents the transparent bits of the frame and permits to detect pixel collisions.
 * The pixel collisions must be enabled.
 * @return the pixel bits object of a frame
 */
PixelBits * SpriteAnimationDirection::get_pixel_bits(int frame) {

  if (pixel_bits == NULL) {
    DIE("The pixel collisions are not enabled for this sprite");
  }

  return pixel_bits[frame];
}

