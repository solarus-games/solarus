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
#include "hud/RupeesCounter.h"
#include "Counter.h"
#include "Game.h"
#include "Equipment.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Sound.h"

/**
 * @brief Position of the 3 rupee icons in the source image.
 */
const Rectangle RupeesCounter::rupee_icon_positions[3] = {
  Rectangle( 0, 0, 12, 12),
  Rectangle(12, 0, 12, 12),
  Rectangle(24, 0, 12, 12),
};

/**
 * @brief Constructor.
 * @param game the current game
 * @param x x coordinate of the top-left corner of the rupees on the destination surface
 * @param y y coordinate of the top-left corner of the rupees on the destination surface
 */
RupeesCounter::RupeesCounter(Game &game, int x, int y):
  HudElement(game, x, y, 48, 12),
  counter(new Counter(3, true, 16, 2)),
  next_rupee_update_date(System::now()) {

  img_rupee_icon = new Surface("hud/rupee_icon.png");
  rupee_bag_displayed = equipment->get_item_variant("rupee_bag");
  counter->set_value(equipment->get_money());
  rebuild();
}

/**
 * @brief Destructor.
 */
RupeesCounter::~RupeesCounter() {
  delete img_rupee_icon;
  delete counter;
}

/**
 * @brief Updates the number of rupees displayed and the color of the icon.
 */
void RupeesCounter::update() {

  HudElement::update();

  bool need_rebuild = false;

  // max rupees
  int rupee_bag = equipment->get_item_variant("rupee_bag");
  if (rupee_bag_displayed != rupee_bag) {
    rupee_bag = rupee_bag_displayed;
    need_rebuild = true;
  }

  // current rupees
  uint32_t nb_current_rupees = equipment->get_money();
  unsigned int nb_current_rupees_displayed = counter->get_value();

  if (nb_current_rupees_displayed != nb_current_rupees && System::now() > next_rupee_update_date) {

    next_rupee_update_date = System::now() + 40;
    
    // increment or decrement the counter until the right value is reached
    if (nb_current_rupees < nb_current_rupees_displayed) {
      counter->decrease();
    }
    else {
      counter->increase();
    }

    // if we have just reached the right value, we play a specific sound
    if (counter->get_value() == nb_current_rupees) {
      Sound::play("rupee_counter_end");
    }

    // while the counter is scrolling, play a sound (every 3 values)
    else if (nb_current_rupees_displayed % 3 == 0) {
      Sound::play("rupee_counter");
    }

    need_rebuild = true;
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * @brief Redraws the icon and the counter on the surface.
 */
void RupeesCounter::rebuild() {

  HudElement::rebuild();
  
  // max rupees (icon)
  img_rupee_icon->blit(rupee_icon_positions[rupee_bag_displayed - 1], surface_drawn);

  // current rupees (counter)
  counter->set_maximum(equipment->get_max_money());
  counter->display(surface_drawn);
}

