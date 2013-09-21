/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/System.h"
#include <SDL.h>
#include <iostream> // print functions

/**
 * \brief Creates a pixel bits object.
 * \param surface the surface where the image is
 * \param image_position position of the image on this surface
 */
PixelBits::PixelBits(Surface& surface, const Rectangle& image_position) {

  // Create a list of boolean values representing the transparency of each pixel.
  // This list is implemented as bit fields.

  uint32_t colorkey;
  SDL_GetColorKey(surface.get_internal_surface(), &colorkey);

  width = image_position.get_width();
  height = image_position.get_height();

  nb_integers_per_row = width >> 5; // width / 32
  if ((width & 31) != 0) { // width % 32 != 0
    nb_integers_per_row++;
  }

  int pixel_index = image_position.get_y() * surface.get_width() + image_position.get_x();

  bits = new uint32_t*[height];
  for (int i = 0; i < height; i++) {
    bits[i] = new uint32_t[nb_integers_per_row];

    // Fill the bits for this row, using nb_integers_per_row sequences of 32 bits.
    int k = -1;
    uint32_t mask = 0x00000000;  // Current bit in the sequence of 32 bits.
    for (int j = 0; j < width; j++) {
      if (mask == 0x00000000) {
        // Time for a new sequence of 32 bits.
        k++;
        mask = 0x80000000;
        bits[i][k] = 0x00000000;  // Initialize the sequence to transparent.
      }

      if (!surface.get_pixel32(pixel_index) == colorkey) {
        // The pixel is opaque.
        bits[i][k] |= mask;
      }

      mask >>= 1;
      pixel_index++;
    }
    pixel_index += surface.get_width() - width;
  }
}

/**
 * \brief Destructor.
 */
PixelBits::~PixelBits() {

  for (int i = 0; i < height; i++) {
    delete[] bits[i];
  }
  delete[] bits;
}

/**
 * \brief Detects whether the image represented by these pixel bits is overlapping another image.
 * \param other the other image
 * \param location1 position of the top-left corner of this image on the map (only x and y must be specified)
 * \param location2 position of the top-left corner of the other image on the map (only x and y must be specified)
 * \return true if there is a collision
 */
bool PixelBits::test_collision(const PixelBits& other, const Rectangle& location1, const Rectangle& location2) const {

  bool debug_pixel_collisions = false;

  // compute the two bounding boxes
  Rectangle bounding_box1 = location1;
  bounding_box1.set_size(width, height);

  Rectangle bounding_box2 = location2;
  bounding_box2.set_size(other.width, other.height);

  // check the collision between the two bounding boxes
  if (!bounding_box1.overlaps(bounding_box2)) {
    return false;
  }

  if (debug_pixel_collisions) {
    std::cout << System::now() << "\n bounding box collision\n";
    std::cout << "rect1 = " << location1 << "\n";
    std::cout << "rect2 = " << location2 << "\n";
    print();
    other.print();
  }

  // compute the intersection between the two rectangles
  Rectangle intersection;
  intersection.set_x(std::max(bounding_box1.get_x(), bounding_box2.get_x()));
  intersection.set_y(std::max(bounding_box1.get_y(), bounding_box2.get_y()));
  intersection.set_width(std::min(bounding_box1.get_x() + bounding_box1.get_width(),
      bounding_box2.get_x() + bounding_box2.get_width()) - intersection.get_x());
  intersection.set_height(std::min(bounding_box1.get_y() + bounding_box1.get_height(),
      bounding_box2.get_y() + bounding_box2.get_height()) - intersection.get_y());

  if (debug_pixel_collisions) {
    std::cout << "intersection: " << intersection << "\n";
  }

  // compute the relative position of the intersection rectangle for each bounding_box
  int offset_x1 = intersection.get_x() - bounding_box1.get_x();
  int offset_y1 = intersection.get_y() - bounding_box1.get_y();

  int offset_x2 = intersection.get_x() - bounding_box2.get_x();
  int offset_y2 = intersection.get_y() - bounding_box2.get_y();

  if (debug_pixel_collisions) {
    std::cout << "offset_x1 = " << offset_x1 << ", offset_y1 = " << offset_y1;
    std::cout << ", offset_x2 = " << offset_x2 << ", offset_y2 = " << offset_y2 << std::endl;
  }

  /*
   * For each row of the intersection, we will call row 'a' the row coming from the right bounding box
   * and row 'b' the one coming from the left bounding box.
   */

  uint32_t** rows_a;           // for each row: array of masks of the right bounding box
  uint32_t** rows_b;           // for each row: array of masks of the left bounding box

  uint32_t* bits_a;            // for the current row: array of masks of the right bounding box
  uint32_t* bits_b;            // for the current row: array of masks of the left bounding box

  int nb_masks_per_row_a;    // number of masks on row a that are in the intersection (row b may have one more mask)
  int nb_masks_per_row_b;    // number of masks on row b that are in the intersection
  int nb_unused_masks_row_b; // number of unused masks on row b (i.e. before the intersection)
  int nb_unused_bits_row_b;  // number of unused bits on the first used mask of row b
  int nb_used_bits_row_b;    // number of bits used on the first used mask of row b

  // make sure row a starts after row b
  if (bounding_box1.get_x() > bounding_box2.get_x()) {
    rows_a = &this->bits[offset_y1];
    rows_b = &other.bits[offset_y2];
    nb_unused_masks_row_b = offset_x2 >> 5;
    nb_unused_bits_row_b = offset_x2 & 31;
  }
  else {
    rows_a = &other.bits[offset_y2];
    rows_b = &this->bits[offset_y1];
    nb_unused_masks_row_b = offset_x1 >> 5;
    nb_unused_bits_row_b = offset_x1 & 31;
  }
  nb_used_bits_row_b = 32 - nb_unused_bits_row_b;

  // compute the number of masks in the intersection on row a
  nb_masks_per_row_a = intersection.get_width() >> 5;
  if ((intersection.get_width() & 31) != 0) {
    nb_masks_per_row_a++;
  }

  // compute the number of masks in the intersection on row b
  nb_masks_per_row_b = (intersection.get_width() + nb_unused_bits_row_b) >> 5;
  if (((intersection.get_width() + nb_unused_bits_row_b) & 31) != 0) {
    nb_masks_per_row_b++;
  }

  // check the collisions each row of the intersection rectangle
  bool collision = false;
  for (int i = 0; i < intersection.get_height() && !collision; i++) {

    // current row
    bits_a = rows_a[i];
    bits_b = rows_b[i];

    if (debug_pixel_collisions) {
      std::cout << "*** checking row " << i << " of the intersection rectangle\n";
    }

    // check each mask
    for (int j = 0; j < nb_masks_per_row_a && !collision; j++) {

      /*
       * We compare the left of a with the right part of b,
       * and the right part of a with the right part of the next b if any.
       */

      uint32_t mask_a = bits_a[j];
      uint32_t mask_b = bits_b[j + nb_unused_masks_row_b];
      uint32_t mask_a_left = mask_a >> nb_unused_bits_row_b;
      uint32_t next_mask_b_left = 0x00000000;
      if (j + 1 < nb_masks_per_row_a ||            // Not the last a-mask.
          nb_masks_per_row_b > nb_masks_per_row_a  // Last a-mask but there exists one more b-mask.
          ) {
        // We are still inside the intersection: next_mask_b_left exists.
        next_mask_b_left = bits_b[j + nb_unused_masks_row_b + 1] >> nb_used_bits_row_b;
      }

      if (debug_pixel_collisions) {
        std::cout << "mask_a = ";
        print_mask(mask_a);
        std::cout << ", mask b = ";
        print_mask(mask_b);
        std::cout << "\n";
      }

      collision = ((mask_a_left & mask_b) | (mask_a & next_mask_b_left)) != 0x00000000;
    }
  }
 
  return collision;
}

/**
 * \brief Prints an ASCII representation of the pixels (for debugging purposes only).
 */
void PixelBits::print() const {

  std::cout << "frame size is " << width << " x " << height << std::endl;
  for (int i = 0; i < height; i++) {
    int k = -1;
    uint32_t mask = 0x00000000;
    for (int j = 0; j < width; j++) {

      if (mask == 0x00000000) {
        k++;
        mask = 0x80000000;
      }

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
 * \brief Prints an ASCII representation of a 32-bit mask (for debugging purposes only).
 */
void PixelBits::print_mask(uint32_t mask) const {

  for (int i = 0; i < 32; i++) {
    std::cout << (((mask & 0x80000000) != 0x00000000) ? "X" : ".");
    mask <<= 1;
  }
}

