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
#include "hud/HudElement.h"
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
  visible(true), opacity(255), blinking(false) {

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
 * Returns the current opacity of this element.
 * @return the current opacity, between 0 and 255
 */
int HudElement::get_opacity(void) {
  return opacity;
}

/**
 * Sets the opacity of this element.
 * @param opacity the opacity, between 0 and 255
 */
void HudElement::set_opacity(int opacity) {

  if (opacity != this->opacity) {
    this->opacity = opacity;
    SDL_SetAlpha(surface_drawn, SDL_SRCALPHA, opacity);
    rebuild();
  }
}

/**
 * Makes this element blink.
 */
void HudElement::set_blinking(int blinking) {

  if (blinking != this->blinking) {

    this->blinking = blinking;
    if (blinking) {
      next_blink_date = SDL_GetTicks();
      blinking_is_visible = false;
    }
  }
}

/**
 * Returns whether this element is blinking.
 * @return true if this element is blinking
 */
bool HudElement::is_blinking(void) {
  return blinking;
}

/**
 * Updates this HUD element.
 */
void HudElement::update(void) {

  Uint32 now = SDL_GetTicks();
  if (blinking && now >= next_blink_date) {
    blinking_is_visible = !blinking_is_visible;
    next_blink_date = now + 250;
  }
}

/**
 * Displays the hud element on a surface.
 * If the savegame is empty, nothing is done.
 * @param destination the destination surface
 */
void HudElement::display(SDL_Surface *destination) {

  if (is_visible() && (!blinking || blinking_is_visible)) {
    // we don't want destination_position to be modified
    SDL_Rect destination_position_unsafe = destination_position;
    SDL_BlitSurface(surface_drawn, NULL, destination, &destination_position_unsafe);
  }
}

