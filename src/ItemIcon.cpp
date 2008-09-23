#include "ItemIcon.h"
#include "Counter.h"
#include "ResourceManager.h"
#include "Equipment.h"
#include "ZSDX.h"
#include "Game.h"
#include "KeysEffect.h"

/**
 * Name of the image file for each item slot.
 */
const string ItemIcon::background_file_names[2] = {
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

  this->item_displayed = InventoryItem::NONE;
  this->item_variant_displayed = 0;
  this->counter = new Counter(2, false, 8, 16);
  this->counter_value_displayed = -1;
  this->is_enabled = true;

  rebuild();
}

/**
 * Destructor.
 */
ItemIcon::~ItemIcon(void) {
  SDL_FreeSurface(background_img);
  SDL_FreeSurface(items_img);  
  delete counter;
}

/**
 * Updates the item image displayed and the counter's value.
 */
void ItemIcon::update(void) {

  bool need_rebuild = false;

  Equipment *equipment = savegame->get_equipment();
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();

  // item assigned
  InventoryItem::ItemId current_item = equipment->get_item_assigned(slot);
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
  InventoryItem *item = InventoryItem::get_item(current_item);
  int counter_index = item->get_counter_index();
  if (counter_index != 0) {

    int current_counter_value = savegame->get_integer(counter_index);

    if (counter_value_displayed != current_counter_value) {
      need_rebuild = true;
      counter_value_displayed = current_counter_value;
      counter->set_value(counter_value_displayed);
    }
  }
  else if (counter_value_displayed != -1) {
    need_rebuild = true;
    counter_value_displayed = -1;
  }

  // icon opacity
  if (keys_effect->are_item_keys_enabled() && !is_enabled) {
    SDL_SetAlpha(surface_drawn, SDL_SRCALPHA, 255);
    is_enabled = true;
    need_rebuild = true;
  }
  else if (!keys_effect->are_item_keys_enabled() && is_enabled) {
    SDL_SetAlpha(surface_drawn, SDL_SRCALPHA, 128);
    is_enabled = false;
    need_rebuild = true;
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
  SDL_BlitSurface(background_img, NULL, surface_drawn, NULL);

  // item
  if (item_displayed != InventoryItem::NONE) {

    SDL_Rect dst_position = {4, 4, 0, 0};
    SDL_Rect src_position = {0, 0, 16, 16};

    src_position.x = 16 * item_displayed;
    src_position.y = 16 * (item_variant_displayed - 1);

    SDL_BlitSurface(items_img, &src_position, surface_drawn, &dst_position);

    // counter
    if (counter_value_displayed != -1) {
      counter->display(surface_drawn);
    }
  }
}
