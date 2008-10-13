#ifndef ZSDX_PIXEL_BITS_H
#define ZSDX_PIXEL_BITS_H

#include "Common.h"

/**
 * Represents the non-transparent pixels of a 8-bit image.
 * For each pixel of the image, a bit indicates whether
 * this pixel is transparent.
 * This class allows to perform fast pixel-perfect collision checks.
 */
class PixelBits {

 private:

  int width;
  int height;

  Uint32 **bits; /**< two-dimensional array representing the transparency bit of each pixel in the image */

 public:

  PixelBits(SDL_Surface *surface, SDL_Rect &image_position);
  ~PixelBits(void);
};

#endif
