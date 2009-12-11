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
#include "hud/ItemIcon.h"
#include "Counter.h"
#include "ResourceManager.h"
#include "Equipment.h"
#include "ZSDX.h"
#include "Game.h"
#include "KeysEffect.h"
#include "InventoryItem.h"
#include "lowlevel/Surface.h"

/**
 * Name of the image file for each item slot.
 */
const std::string ItemIcon::background_file_names[2] = {
  "hud/item_x_icon.png",
  "hud/item_v_icon.png",
};

/**
 * Creates an item icon.
 * @param slot the slot of this icon (0 for the X icon, 1 for the Y icon)
 * @param savegame the savegame
 * @param x x position of the icon on the screen
 * @param y y position of the icon on the screen
 */
ItemIcon::ItemIcon(int slot, Savegame *savegame, int x, int y):
  HudElement(x, y, 32, 28) {

  this->slot = slot;
  this->savegame = savegame;
  this->background_img = ResourceManager::load_image(background_file_names[slot]);
  this->items_img = ResourceManager::load_image("hud/inventory_items.png");

  this->item_displayed = INVENTORY_NONE;
  this->item_variant_displayed = 0;
  this->counter = new Counter(2, false, 8, 16);
  this->counter_value_displayed = -1;
  this->counter_maximum_displayed = -1;

  rebuild();
}

/**
 * Destructor.
 */
ItemIcon::~ItemIcon(void) {
  delete background_img;
  delete items_img;  
  delete counter;
}

/**
 * Updates the item image displayed and the counter's value.
 */
void ItemIcon::update(void) {

  HudElement::update();

  bool need_rebuild = false;

  Equipment *equipment = savegame->get_equipment();
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();

  // item assigned
  InventoryItemId current_item = equipment->get_item_assigned(slot);
  if (item_displayed != current_item) {

    need_rebuild = true;
    item_displayed = current_item;
    item_variant_displayed = 0;
  }

  // variant of the item
  int current_item_variant = equipment->has_inventory_item(current_item);
  if (item_variant_displayed != current_item_variant) {

    need_rebuild = true;
    item_variant_displayed = current_item_variant;
  }

  // counter index
  int counter_index = InventoryItem::get_counter_index(current_item);
  if (counter_index != -1) {

    int current_counter_value = equipment->get_inventory_item_amount(current_item);
    int current_counter_maximum = equipment->get_inventory_item_maximum(current_item);

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
 * Redraws the icon on the surface.
 */
void ItemIcon::rebuild(void) {

  HudElement::rebuild();

  // background image
  background_img->blit(surface_drawn);

  // item
  if (item_displayed != INVENTORY_NONE) {

    Rectangle dst_position(4, 4, 0, 0);
    Rectangle src_position(0, 0, 16, 16);

    src_position.set_x(16 * item_displayed);
    src_position.set_y(16 * (item_variant_displayed - 1));

    items_img->blit(src_position, surface_drawn, dst_position);

    // counter
    if (counter_value_displayed != -1) {
      counter->display(surface_drawn);
    }
  }
}

