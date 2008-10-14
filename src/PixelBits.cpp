#include "PixelBits.h"

/**
 * Creates a pixel bits object.
 * @param surface the surface where the image is
 * @param image_position position of the image on this surface
 */
PixelBits::PixelBits(SDL_Surface *surface, SDL_Rect &image_position) {

  width = image_position.w;
  height = image_position.h;


  nb_integers_per_row = width / 32;
  if (width % 32 != 0) {
    nb_integers_per_row++;
  }

  bits = new Uint32*[height];
  for (int i = 0; i < height; i++) {
    bits[i] = new Uint32[nb_integers_per_row];
  }
}

/**
 * Destructor.
 */
PixelBits::~PixelBits(void) {

  for (int i = 0; i < height; i++) {
    delete bits[i];
  }
  delete[] bits;
}
