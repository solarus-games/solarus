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
#include "lowlevel/PixelBits.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/Debug.h"
#include <SDL/SDL.h>
#include <iostream> // print functions

/**
 * @brief Creates a pixel bits object.
 * @param surface the surface where the image is
 * @param image_position position of the image on this surface
 */
PixelBits::PixelBits(Surface *surface, const Rectangle &image_position) {

  SDL_PixelFormat *format = surface->get_internal_surface()->format;
  Debug::check_assertion(format->BitsPerPixel == 8, "This surface should have an 8-bit pixel format");

  uint8_t colorkey = (uint8_t) format->colorkey;

  width = image_position.get_width();
  height = image_position.get_height();

  nb_integers_per_row = width >> 5; // width / 32
  if ((width & 31) != 0) { // width % 32 != 0
    nb_integers_per_row++;
  }

  uint8_t *pixels = (uint8_t*) surface->get_internal_surface()->pixels;
  int pixel_index = image_position.get_y() * surface->get_width() + image_position.get_x();

  bits = new uint32_t*[height];
  for (int i = 0; i < height; i++) {
    bits[i] = new uint32_t[nb_integers_per_row];

    // fill the bits for this row
    int k = -1;
    uint32_t mask = 0x00000000;
    for (int j = 0; j < width; j++) {

      if (mask == 0x00000000) {
	k++;
	mask = 0x80000000;
	bits[i][k] = 0x00000000;
      }

      if (pixels[pixel_index] != colorkey) {
	bits[i][k] |= mask;
      }
      mask >>= 1;
      pixel_index++;
    }
    pixel_index += surface->get_width() - width;
  }

}

/**
 * @brief Destructor.
 */
PixelBits::~PixelBits() {

  for (int i = 0; i < height; i++) {
    delete[] bits[i];
  }
  delete[] bits;
}

/**
 * @brief Detects whether the image represented by these pixel bits is overlapping another image.
 * @param other the other image
 * @param location1 position of the top-left corner of this image on the map (only x and y must be specified)
 * @param location2 position of the top-left corner of the other image on the map (only x and y must be specified)
 * @return true if there is a collision
 */
bool PixelBits::test_collision(PixelBits *other, const Rectangle &location1, const Rectangle &location2) {

  // compute the two bounding boxes
  Rectangle bounding_box1 = location1;
  bounding_box1.set_size(width, height);

  Rectangle bounding_box2 = location2;
  bounding_box2.set_size(other->width, other->height);

  // check the collision between the two bounding boxes
  if (!bounding_box1.overlaps(bounding_box2)) {
    return false;
  }

  /* debug
  std::cout << System::now() << " bounding box collision\n";
  std::cout << "rect1 = " << location1.x << "," << location1.y << " x " << bounding_box1.w << "," << bounding_box1.h << "\n";
  std::cout << "rect2 = " << location2.x << "," << location2.y << " x " << bounding_box2.w << "," << bounding_box2.h << "\n";
  print();
  other->print();
  */

  // compute the intersection between the two rectangles
  Rectangle intersection;
  intersection.set_x(std::max(bounding_box1.get_x(), bounding_box2.get_x()));
  intersection.set_y(std::max(bounding_box1.get_y(), bounding_box2.get_y()));
  intersection.set_width(std::min(bounding_box1.get_x() + bounding_box1.get_width(),
      bounding_box2.get_x() + bounding_box2.get_width()) - intersection.get_x());
  intersection.set_height(std::min(bounding_box1.get_y() + bounding_box1.get_height(),
      bounding_box2.get_y() + bounding_box2.get_height()) - intersection.get_y());

  /* debug
  std::cout << "intersection: " << intersection.x << "," << intersection.y << " x " << intersection.w << "," << intersection.h << "\n";
  */

  // compute the relative position of the intersection rectangle for each bounding_box
  int offset_x1 = intersection.get_x() - bounding_box1.get_x();
  int offset_y1 = intersection.get_y() - bounding_box1.get_y();

  int offset_x2 = intersection.get_x() - bounding_box2.get_x();
  int offset_y2 = intersection.get_y() - bounding_box2.get_y();

  /* debug
  std::cout << "offset_x1 = " << offset_x1 << ", offset_y1 = " << offset_y1;
  std::cout << ", offset_x2 = " << offset_x2 << ", offset_y2 = " << offset_y2 << std::endl;
  */

  /*
   * For each row of the intersection, we will call row 'a' the row coming from the right bounding box
   * and row 'b' the one coming from the left bounding box.
   */

  uint32_t **rows_a;           // for each row: array of masks of the right bounding box
  uint32_t **rows_b;           // for each row: array of masks of the left bounding box

  uint32_t *bits_a;            // array of masks of the right bounding box for the current row
  uint32_t *bits_b;            // array of masks of the left bounding box for the current row

  int nb_masks_per_row_a;    // number of masks on row a that are in the intersection (row b may have one more mask)
  bool has_row_b_additional_mask;
  int nb_unused_masks_row_b; // number of unused masks on row b (i.e. before the intersection)
  int nb_unused_bits_row_b;  // number of unused bits on the first used mask of row b
  int nb_used_bits_row_b;    // number of bits used on the first used mask of row b

  // compute the number of masks on row a
  nb_masks_per_row_a = intersection.get_width() >> 5;
  if ((intersection.get_width() & 31) != 0) {
    nb_masks_per_row_a++;
  }

  // make sure row a starts after row b
  if (bounding_box1.get_x() > bounding_box2.get_x()) {
    rows_a = &this->bits[offset_y1];
    rows_b = &other->bits[offset_y2];
    nb_unused_masks_row_b = offset_x2 >> 5;
    nb_unused_bits_row_b = offset_x2 & 31;
    has_row_b_additional_mask = (nb_masks_per_row_a + nb_unused_masks_row_b < other->nb_integers_per_row);
  }
  else {
    rows_a = &other->bits[offset_y2];
    rows_b = &this->bits[offset_y1]; 
    nb_unused_masks_row_b = offset_x1 >> 5;
    nb_unused_bits_row_b = offset_x1 & 31;
    has_row_b_additional_mask = (nb_masks_per_row_a + nb_unused_masks_row_b < this->nb_integers_per_row);
  }
  nb_used_bits_row_b = 32 - nb_unused_bits_row_b;

  // check the collisions each row of the intersection rectangle
  bool collision = false;
  for (int i = 0; i < intersection.get_height() && !collision; i++) {

    // current row
    bits_a = rows_a[i];
    bits_b = rows_b[i];

    /* debug
    std::cout << "*** checking row " << i << " of the intersection rectangle\n";
    */

    // check each mask
    for (int j = 0; j < nb_masks_per_row_a && !collision; j++) {

      /*
       * We compare a mask of row a with the right part of a mask of row b
       * and the left part of the next mask of row b
       */

      uint32_t mask_a = bits_a[j];
      uint32_t mask_b = bits_b[j + nb_unused_masks_row_b];
      uint32_t next_mask_b_left;

      /* debug
      std::cout << "mask_a = ";
      print_mask(mask_a);
      std::cout << ", mask b = ";
      print_mask(mask_b);
      std::cout << "\n";
      */

      if (has_row_b_additional_mask) {
	next_mask_b_left = bits_b[j + nb_unused_masks_row_b + 1] >> nb_used_bits_row_b;
      }
      else { // special case where the next mask b does not exist because a and b are aligned
	next_mask_b_left = 0x00000000;
      }

      uint32_t mask_a_left = mask_a >> nb_unused_bits_row_b;
      collision = ((mask_a_left & mask_b) | (mask_a & next_mask_b_left)) != 0x00000000;
    }
  }
 
  return collision;
}

/**
 * @brief Prints an ascii representation of the pixels (for debugging purposes only).
 */
void PixelBits::print() {

  std::cout << "frame size is " << width << " x " << height << std::endl;
  for (int i = 0; i < height; i++) {
    int k = -1;
    uint32_t mask = 0x00000000;
    for (int j = 0; j < width; j++) {

      if (mask == 0x00000000) {
	k++;
	mask = 0x80000000;
      }

      //      std::cout << "bit " << i << "," << j << " is " << (bits[i][k] & mask) << endl;
      
      if (bits[i][k] & mask) {
	std::cout << "X";
      }
      else {
	std::cout << ".";
      }

      mask >>= 1;      
    }
    std::cout << std::endl;
  }
}

/**
 * @brief Prints an ascii representation of a 32-bit mask (for debugging purposes only).
 */
void PixelBits::print_mask(uint32_t mask) {
  for (int i = 0; i < 32; i++) {
    std::cout << (((mask & 0x8000) != 0x0000) ? "X" : ".");
    mask >>= 1;
  }
}

