/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/ShopTreasure.h"
#include "solarus/entities/Hero.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/KeysEffect.h"
#include "solarus/Sprite.h"
#include "solarus/Equipment.h"
#include "solarus/EquipmentItem.h"
#include "solarus/Savegame.h"
#include "solarus/lowlevel/TextSurface.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/Debug.h"
#include <sstream>

namespace Solarus {

/**
 * \brief Creates a new shop treasure with the specified treasure and price.
 * \param name the name identifying this entity
 * \param layer layer of the entity to create
 * \param xy Coordinate of the entity to create.
 * \param treasure the treasure that the hero can buy
 * \param price the treasure's price
 * \param dialog_id id of the dialog describing the item when the player watches it
 */
ShopTreasure::ShopTreasure(
    const std::string& name,
    Layer layer,
    const Point& xy,
    const Treasure& treasure,
    int price,
    const std::string& dialog_id):
  Detector(COLLISION_FACING, name, layer, xy, Size(32, 32)),
  treasure(treasure),
  price(price),
  dialog_id(dialog_id),
  price_digits(0, 0, TextSurface::HorizontalAlignment::LEFT, TextSurface::VerticalAlignment::TOP),
  rupee_icon_sprite(std::make_shared<Sprite>("entities/rupee_icon")) {

  std::ostringstream oss;
  oss << price;
  price_digits.set_text(oss.str());
}

/**
 * \brief Creates a new shop treasure with the specified treasure and price.
 * \param game the current game
 * \param name the name identifying this entity
 * \param layer layer of the entity to create
 * \param xy Coordinates of the entity to create.
 * \param treasure the treasure that the hero can buy
 * \param price the treasure's price in rupees
 * \param dialog_id id of the dialog describing the item when the player watches it
 * \return the shop treasure created, or nullptr if it is already bought or if it
 * is not obtainable.
 */
std::shared_ptr<ShopTreasure> ShopTreasure::create(
    Game& /* game */,
    const std::string& name,
    Layer layer,
    const Point& xy,
    const Treasure& treasure,
    int price,
    const std::string& dialog_id
) {
  // See if the item is not already bought and is obtainable.
  if (treasure.is_found() || !treasure.is_obtainable()) {
    return nullptr;
  }

  return std::make_shared<ShopTreasure>(
      name, layer, xy, treasure, price, dialog_id
  );
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType ShopTreasure::get_type() const {
  return EntityType::SHOP_TREASURE;
}

/**
 * \brief Returns the treasure for sale in this entity.
 * \return The treasure.
 */
const Treasure& ShopTreasure::get_treasure() const {
  return treasure;
}

/**
 * \brief Returns the price of this shop item.
 * \return The price.
 */
int ShopTreasure::get_price() const {
  return price;
}

/**
 * \brief Returns the id of the dialog describing this shop item when the
 * player watches it.
 * \return The dialog id.
 */
const std::string& ShopTreasure::get_dialog_id() const {
  return dialog_id;
}

/**
 * \brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 *
 * \return true if the sword is ignored
 */
bool ShopTreasure::is_sword_ignored() const {
  return true;
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other another entity
 * \return true
 */
bool ShopTreasure::is_obstacle_for(MapEntity& /* other */) {
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
void ShopTreasure::notify_collision(
    MapEntity& entity_overlapping, CollisionMode /* collision_mode */) {

  if (entity_overlapping.is_hero() && !get_game().is_suspended()) {

    Hero& hero = static_cast<Hero&>(entity_overlapping);

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
        && hero.is_free()) {

      // we show the 'look' icon
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * \copydoc Detector::notify_action_command_pressed
 */
bool ShopTreasure::notify_action_command_pressed() {

  if (get_hero().is_free()
      && get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_LOOK) {

    LuaContext& lua_context = get_lua_context();
    lua_context.notify_shop_treasure_interaction(*this);
    return true;
  }

  return false;
}

/**
 * \brief Draws the entity on the map.
 */
void ShopTreasure::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  const SurfacePtr& map_surface = get_map().get_visible_surface();
  int x = get_x();
  int y = get_y();

  // draw the treasure
  const Rectangle& camera_position = get_map().get_camera_position();
  treasure.draw(map_surface,
      x + 16 - camera_position.get_x(),
      y + 13 - camera_position.get_y()
  );

  // also draw the price
  price_digits.draw(map_surface,
      x + 12 - camera_position.get_x(),
      y + 21 - camera_position.get_y());
  rupee_icon_sprite->draw(map_surface,
      x - camera_position.get_x(),
      y + 22 - camera_position.get_y());
}

}

