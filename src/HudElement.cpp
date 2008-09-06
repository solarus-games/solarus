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
HudElement::HudElement(int x, int y, int width, int height):
  visible(true) {

  surface_drawn = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, 0, 0, 0, 0);
  SDL_SetColorKey(surface_drawn, SDL_SRCCOLORKEY, Color::black);

  set_position(x, y);
}

/**
 * Destructor.
 */
HudElement::~HudElement(void) {
  SDL_FreeSurface(surface_drawn);
}

/**
 * Changes the position of this hud element.
 */
void HudElement::set_position(int x, int y) {
  
  destination_position.x = x;
  destination_position.y = y;
}

/**
 * Redefine this method to redraw the element on the surface.
 * You have to call this method from the subclasses to initialize
 * the surface with transparent color.
 */
void HudElement::rebuild(void) {
  SDL_FillRect(surface_drawn, NULL, Color::black);
}

/**
 * Shows or hides this element.
 * @param visible true to make it visible, false to hide it
 */
void HudElement::set_visible(bool visible) {
  this->visible = visible;
}

/**
 * Returns whether this hud element is visible.
 * The display() function does nothing if this function
 * returns false.
 * @return true if this hud element is visible, false otherwise
 */
bool HudElement::is_visible(void) {
  return visible;
}

/**
 * Displays the hud element on a surface.
 * If the savegame is empty, nothing is done.
 * @param destination the destination surface
 */
void HudElement::display(SDL_Surface *destination) {

  if (is_visible()) {
    // we don't want destination_position to be modified
    SDL_Rect destination_position_unsafe = destination_position;
    SDL_BlitSurface(surface_drawn, NULL, destination, &destination_position_unsafe);
  }
}
