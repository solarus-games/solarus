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
#include "hud/ItemIcon.h"
#include "Counter.h"
#include "Equipment.h"
#include "ItemProperties.h"
#include "Game.h"
#include "KeysEffect.h"
#include "lowlevel/Surface.h"

/**
 * @brief Name of the image file for each item slot.
 */
const std::string ItemIcon::background_file_names[2] = {
  "hud/item_x_icon.png",
  "hud/item_v_icon.png",
};

/**
 * @brief Creates an item icon.
 * @param game the current game
 * @param slot the slot of this icon (0 for the X icon, 1 for the Y icon)
 * @param x x position of the icon on the screen
 * @param y y position of the icon on the screen
 */
ItemIcon::ItemIcon(Game *game, int slot, int x, int y):
  HudElement(game, x, y, 32, 28) {

  this->slot = slot;
  this->background_img = new Surface(background_file_names[slot]);
  this->items_img = new Surface("hud/inventory_items.png");

  this->item_displayed = "";
  this->item_variant_displayed = 0;
  this->counter = new Counter(2, false, 8, 16);
  this->counter_value_displayed = -1;
  this->counter_maximum_displayed = -1;

  rebuild();
}

/**
 * @brief Destructor.
 */
ItemIcon::~ItemIcon(void) {
  delete background_img;
  delete items_img;  
  delete counter;
}

/**
 * @brief Updates the item image displayed and the counter's value.
 */
void ItemIcon::update(void) {

  HudElement::update();

  bool need_rebuild = false;

  KeysEffect *keys_effect = game->get_keys_effect();

  // item assigned
  const std::string &current_item = equipment->get_item_assigned(slot);
  if (item_displayed != current_item) {

    need_rebuild = true;
    item_displayed = current_item;
    item_variant_displayed = 0;
  }

  // variant of the item
  int current_item_variant = equipment->get_item_variant(current_item);
  if (item_variant_displayed != current_item_variant) {

    need_rebuild = true;
    item_variant_displayed = current_item_variant;
  }

  // counter index
  int counter_index = equipment->get_item_properties(current_item)->get_counter_savegame_variable();
  if (counter_index != -1) {

    int current_counter_value = equipment->get_item_amount(current_item);
    int current_counter_maximum = equipment->get_item_maximum(current_item);

    if (counter_value_displayed != current_counter_value || counter_maximum_displayed != current_counter_maximum) {
      need_rebuild = true;
      counter_maximum_displayed = current_counter_maximum;
      counter_value_displayed = current_counter_value;
      counter->set_maximum(current_counter_maximum);
      counter->set_value(counter_value_displayed);
    }
  }
  else if (counter_value_displayed != -1) {
    need_rebuild = true;
    counter_value_displayed = -1;
  }

  // icon opacity
  if (keys_effect->are_item_keys_enabled() && get_opacity() == 128) {
    set_opacity(255);
  }
  else if (!keys_effect->are_item_keys_enabled() && get_opacity() == 255) {
    set_opacity(128);
  }

  // redraw the icon if needed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * @brief Redraws the icon on the surface.
 */
void ItemIcon::rebuild(void) {

  HudElement::rebuild();

  // background image
  background_img->blit(surface_drawn);

  // item
  if (item_displayed != "") {

    Rectangle dst_position(4, 4, 0, 0);
    Rectangle src_position(0, 0, 16, 16);

// TODO    src_position.set_x(16 * item_index);
    src_position.set_y(16 * (item_variant_displayed - 1));

    items_img->blit(src_position, surface_drawn, dst_position);

    // counter
    if (counter_value_displayed != -1) {
      counter->display(surface_drawn);
    }
  }
}

