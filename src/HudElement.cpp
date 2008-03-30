/**
 * This module defines the class HudElement.
 */

#include "HudElement.h"
#include "Color.h"
#include "Savegame.h"

/**
 * Constructor.
 * @param x x coordinate of the top-left corner of the hud element surface on the destination surface
 * @param y y coordinate of the top-left corner of the hud element surface on the destination surface
 * @param width width of the hud element surface
 * @param height height of the hud element surface
 */
HudElement::HudElement(Savegame *savegame, int x, int y, int width, int height):
  savegame(savegame) {

  surface_drawn = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, 0, 0, 0, 0);
  SDL_SetColorKey(surface_drawn, SDL_SRCCOLORKEY, COLOR_BLACK);

  destination_position.x = x;
  destination_position.y = y;
}

/**
 * Destructor.
 */
HudElement::~HudElement(void) {
  SDL_FreeSurface(surface_drawn);
}

/**
 * Displays the hud element on a surface.
 * If the savegame is empty, nothing is done.
 * @param destination the destination surface
 */
void HudElement::display(SDL_Surface *destination) {

  if (!savegame->is_empty()) {
    SDL_BlitSurface(surface_drawn, NULL, destination, &destination_position);
  }
}
