/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/ShopItem.h"
#include "entities/Hero.h"
#include "lua/LuaContext.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "Equipment.h"
#include "EquipmentItem.h"
#include "Savegame.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>

/**
 * \brief Creates a new shop item with the specified treasure and price.
 * \param name the name identifying this entity
 * \param layer layer of the entity to create
 * \param x x coordinate of the entity to create
 * \param y y coordinate of the entity to create
 * \param treasure the treasure that the hero can buy (will be deleted automatically)
 * \param price the treasure's price in rupees
 * \param dialog_id id of the dialog describing the item when the player watches it
 */
ShopItem::ShopItem(const std::string& name, Layer layer, int x, int y,
		   const Treasure& treasure, int price, const std::string& dialog_id):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 32, 32),
  treasure(treasure),
  price(price),
  dialog_id(dialog_id),
  price_digits(0, 0, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP),
  rupee_icon_sprite("entities/rupee_icon") {

  std::ostringstream oss;
  oss << price;
  price_digits.set_text(oss.str());
}

/**
 * \brief Destructor.
 */
ShopItem::~ShopItem() {
}

/**
 * \brief Creates a new shop item with the specified treasure and price.
 * \param game the current game
 * \param name the name identifying this entity
 * \param layer layer of the entity to create
 * \param x x coordinate of the entity to create
 * \param y y coordinate of the entity to create
 * \param treasure the treasure that the hero can buy
 * \param price the treasure's price in rupees
 * \param dialog_id id of the dialog describing the item when the player watches it
 * \return the shop item created, or NULL if it is already bought
 */
ShopItem* ShopItem::create(Game& game, const std::string& name, Layer layer, int x, int y,
    const Treasure &treasure, int price, const std::string& dialog_id) {

  // see if the item is not already bought
  if (treasure.is_found()) {
    return NULL;
  }

  return new ShopItem(name, layer, x, y, treasure, price, dialog_id);
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType ShopItem::get_type() {
  return SHOP_ITEM;
}

/**
 * \brief Returns the treasure for sale in this entity.
 * \return The treasure.
 */
const Treasure& ShopItem::get_treasure() const {
  return treasure;
}

/**
 * \brief Returns the price of this shop item.
 * \return The price.
 */
int ShopItem::get_price() const {
  return price;
}

/**
 * \brief Returns the id of the dialog describing this shop item when the
 * player watches it.
 * \return The dialog id.
 */
const std::string& ShopItem::get_dialog_id() const {
  return dialog_id;
}

/**
 * \brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 *
 * \return true if the sword is ignored
 */
bool ShopItem::is_sword_ignored() {
  return true;
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other another entity
 * \return true
 */
bool ShopItem::is_obstacle_for(MapEntity &other) {
  return true;
}

/**
 * \brief This function is called by the engine when an entity overlaps the shop item.
 *
 * If the entity is the hero, we allow him to buy the item.
 *
 * \param entity_overlapping the entity overlapping the detector
 * \param collision_mode the collision mode that detected the collision
 */
void ShopItem::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping.is_hero() && !get_game().is_suspended()) {

    Hero &hero = (Hero&) entity_overlapping;

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
        && hero.is_free()) {

      // we show the 'look' icon
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * \brief Notifies this detector that the player is interacting with it by
 * pressing the action command.
 *
 * This function is called when the player presses the action command
 * while the hero is facing this detector, and the action command effect lets
 * him do this.
 * A dialog is shown to let the hero buy the item.
 */
void ShopItem::notify_action_command_pressed() {

  if (get_hero().is_free()
      && get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_LOOK) {

    LuaContext& lua_context = get_lua_context();
    lua_context.notify_shop_item_interaction(*this);
  }
}

/**
 * \brief Draws the entity on the map.
 */
void ShopItem::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  Surface& map_surface = get_map().get_visible_surface();
  int x = get_x();
  int y = get_y();

  // draw the treasure
  const Rectangle& camera_position = get_map().get_camera_position();
  treasure.draw(map_surface,
      x + 16 - camera_position.get_x(),
      y + 13 - camera_position.get_y());

  // also draw the price
  price_digits.draw(map_surface,
      x + 12 - camera_position.get_x(),
      y + 21 - camera_position.get_y());
  rupee_icon_sprite.draw(map_surface,
      x - camera_position.get_x(),
      y + 22 - camera_position.get_y());
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& ShopItem::get_lua_type_name() const {
  return LuaContext::entity_shop_item_module_name;
}

