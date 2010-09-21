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
#include "hud/SmallKeysCounter.h"
#include "Counter.h"
#include "Equipment.h"
#include "lowlevel/Surface.h"

/**
 * @brief Constructor.
 * @param game the current game
 * @param x x coordinate of the top-left corner of the key icon on the destination surface
 * @param y y coordinate of the top-left corner of the key icon on the destination surface
 */
SmallKeysCounter::SmallKeysCounter(Game *game, int x, int y):
  HudElement(game, x, y, 40, 8), counter(NULL) {

  img_icon = new Surface("hud/small_key_icon.png");
  counter = NULL;

  rebuild();
}

/**
 * @brief Destructor.
 */
SmallKeysCounter::~SmallKeysCounter() {

  delete img_icon;
  if (counter != NULL) {
    delete counter;
  }
}

/**
 * @brief Updates this HUD element.
 */
void SmallKeysCounter::update() {

  HudElement::update();

  bool need_rebuild = false;

  // enable or disable the counter
  if (counter == NULL && equipment->are_small_keys_enabled()) {
    counter = new Counter(2, false, 2, 0);
    need_rebuild = true;
  }
  else if (counter != NULL && !equipment->are_small_keys_enabled()) {
    delete counter;
    counter = NULL;
    need_rebuild = true;
  }

  if (equipment->are_small_keys_enabled()) {

    // current small keys
    int nb_keys = equipment->get_small_keys();
    int nb_keys_displayed = counter->get_value();

    if (nb_keys_displayed != nb_keys) {
      counter->set_value(nb_keys);
      need_rebuild = true;
    }
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * @brief Redraws the icon and the counter on the surface.
 */
void SmallKeysCounter::rebuild() {

  HudElement::rebuild();

  if (counter != NULL) { // the small keys are enabled

    // small key icon
    img_icon->blit(surface_drawn);

    // current number of small keys
    counter->display(surface_drawn);
  }
}

