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
#include "hud/HudElement.h"
#include "Game.h"
#include "Savegame.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "lowlevel/System.h"

/**
 * @brief Creates a HUD element in a game.
 * @param game the current game
 * @param x x coordinate of the top-left corner of the hud element surface on the destination surface
 * @param y y coordinate of the top-left corner of the hud element surface on the destination surface
 * @param width width of the hud element surface
 * @param height height of the hud element surface
 */
HudElement::HudElement(Game *game, int x, int y, int width, int height):
  game(game), equipment(game->get_equipment()), keys_effect(game->get_keys_effect()),
  visible(true), opacity(255), blinking(false) {

  surface_drawn = new Surface(width, height);
  surface_drawn->set_transparency_color(Color::get_black());

  set_position(x, y);
}

/**
 * @brief Creates a HUD element outside a game and representing some equipment-related data.
 *
 * The fields game and keys_effect will be NULL.
 *
 * @param equipment the equipment object to represent
 * @param x x coordinate of the top-left corner of the hud element surface on the destination surface
 * @param y y coordinate of the top-left corner of the hud element surface on the destination surface
 * @param width width of the hud element surface
 * @param height height of the hud element surface
 */
HudElement::HudElement(Equipment *equipment, int x, int y, int width, int height):
  game(NULL), equipment(equipment), keys_effect(NULL),
  visible(true), opacity(255), blinking(false) {

  surface_drawn = new Surface(width, height);
  surface_drawn->set_transparency_color(Color::get_black());

  set_position(x, y);
}

/**
 * @brief Creates a HUD element outside a game and representing some keys effect related data.
 *
 * The fields game and equipment will be NULL.
 *
 * @param keys_effect the keys_effect object to represent
 * @param x x coordinate of the top-left corner of the hud element surface on the destination surface
 * @param y y coordinate of the top-left corner of the hud element surface on the destination surface
 * @param width width of the hud element surface
 * @param height height of the hud element surface
 */
HudElement::HudElement(KeysEffect *keys_effect, int x, int y, int width, int height):
  game(NULL), equipment(NULL), keys_effect(keys_effect),
  visible(true), opacity(255), blinking(false) {

  surface_drawn = new Surface(width, height);
  surface_drawn->set_transparency_color(Color::get_black());

  set_position(x, y);
}

/**
 * @brief Destructor.
 */
HudElement::~HudElement(void) {
  delete surface_drawn;
}

/**
 * @brief Changes the position of this hud element.
 * @param x the new x coordinate
 * @param y the new y coordinate
 */
void HudElement::set_position(int x, int y) {
  
  destination_position.set_x(x);
  destination_position.set_y(y);
}

/**
 * @brief Redefine this method to redraw the element on the surface.
 *
 * You have to call this method from your redifined method to initialize
 * the surface with transparent color.
 */
void HudElement::rebuild(void) {
  surface_drawn->fill_with_color(Color::get_black());
}

/**
 * @brief Shows or hides this element.
 * @param visible true to make it visible, false to hide it
 */
void HudElement::set_visible(bool visible) {
  this->visible = visible;
}

/**
 * @brief Returns whether this hud element is visible.
 *
 * The display() function does nothing if this function
 * returns false.
 *
 * @return true if this hud element is visible, false otherwise
 */
bool HudElement::is_visible(void) {
  return visible;
}

/**
 * @brief Returns the current opacity of this element.
 * @return the current opacity, between 0 and 255
 */
int HudElement::get_opacity(void) {
  return opacity;
}

/**
 * @brief Sets the opacity of this element.
 * @param opacity the opacity, between 0 and 255
 */
void HudElement::set_opacity(int opacity) {

  if (opacity != this->opacity) {
    this->opacity = opacity;
    surface_drawn->set_opacity(opacity);
    rebuild();
  }
}

/**
 * @brief Makes this element blink or stop blinking.
 * @param blinking true to make the element blink, false otherwise
 */
void HudElement::set_blinking(bool blinking) {

  if (blinking != this->blinking) {

    this->blinking = blinking;
    if (blinking) {
      next_blink_date = System::now();
      blinking_is_visible = false;
    }
  }
}

/**
 * @brief Returns whether this element is blinking.
 * @return true if this element is blinking
 */
bool HudElement::is_blinking(void) {
  return blinking;
}

/**
 * @brief Updates this HUD element.
 */
void HudElement::update(void) {

  uint32_t now = System::now();
  if (blinking && now >= next_blink_date) {
    blinking_is_visible = !blinking_is_visible;
    next_blink_date = now + 250;
  }
}

/**
 * @brief Displays the hud element on a surface.
 * @param destination the destination surface
 */
void HudElement::display(Surface *destination) {

  if (is_visible() && (!blinking || blinking_is_visible)) {
    surface_drawn->blit(destination, destination_position);
  }
}

