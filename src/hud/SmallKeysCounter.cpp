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
#include "hud/SmallKeysCounter.h"
#include "Counter.h"
#include "Equipment.h"
#include "ResourceManager.h"

/**
 * Constructor.
 * @param equipment the player's equipment
 * @param x x coordinate of the top-left corner of the key icon on the destination surface
 * @param y y coordinate of the top-left corner of the key icon on the destination surface
 */
SmallKeysCounter::SmallKeysCounter(Equipment *equipment, int x, int y):
  HudElement(x, y, 40, 16), equipment(equipment), counter(NULL) {

  img_icon = ResourceManager::load_image("hud/message_and_treasure_icons.png");
  counter = NULL;

  rebuild();
}

/**
 * Destructor.
 */
SmallKeysCounter::~SmallKeysCounter(void) {
  SDL_FreeSurface(img_icon);

  if (counter != NULL) {
    delete counter;
  }
}

/**
 * Updates this HUD element.
 */
void SmallKeysCounter::update(void) {

  HudElement::update();

  bool need_rebuild = false;

  // enable or disable the counter
  if (counter == NULL && equipment->are_small_keys_enabled()) {
    counter = new Counter(2, false, 6, 8);
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
 * Redraws the icon and the counter on the surface.
 */
void SmallKeysCounter::rebuild(void) {

  HudElement::rebuild();

  if (counter != NULL) { // the small keys are enabled

    // small key icon
    static SDL_Rect src_position = {36, 112, 8, 16};
    SDL_BlitSurface(img_icon, &src_position, surface_drawn, NULL);

    // current number of small keys
    counter->display(surface_drawn);
  }
}
