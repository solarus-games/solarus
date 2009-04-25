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
#include "entities/ShopItem.h"
#include "entities/Hero.h"
#include "ZSDX.h"
#include "ResourceManager.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "Treasure.h"
#include "Counter.h"

/**
 * Creates a new shop item with the specified treasure and price.
 * @param name the name identifying this entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param treasure the treasure that the hero can buy (will be deleted automatically)
 * @param price the treasure's price in rupees
 * @param message_id id of the message describing the item when the player watches it
 */
ShopItem::ShopItem(const std::string &name, Layer layer, int x, int y,
		   Treasure *treasure, int price, MessageId message_id):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  treasure(treasure), price(price), message_id(message_id) {

  price_counter = new Counter(3, false, 0, 0);
  price_counter->set_value(price);

  rupee_icon = ResourceManager::load_image("entities/miscellaneous.png");
  rupee_icon_src_position.x = 0;
  rupee_icon_src_position.y = 16;
  rupee_icon_src_position.w = 8;
  rupee_icon_src_position.h = 8;
}

/**
 * Destructor.
 */
ShopItem::~ShopItem(void) {

  if (treasure != NULL) {
    // delete the treasure only if the player didn't buy it
    delete treasure;
  }

  SDL_FreeSurface(rupee_icon);
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType ShopItem::get_type() {
  return SHOP_ITEM;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool ShopItem::is_obstacle_for(MapEntity *other) {
  return true;
}

/**
 * This function is called by the engine when an entity overlaps the shop item.
 * This is a redefinition of Detector::collision().
 * If the entity is the hero, we allow him to buy the item.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void ShopItem::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero() && !zsdx->game->is_suspended()) {

    Hero *hero = zsdx->game->get_hero();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE) {

      // we show the 'look' icon
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * A dialog is shown to let the hero buy the item.
 */
void ShopItem::action_key_pressed(void) {

  Hero *hero = zsdx->game->get_hero();

  if (hero->get_state() == Hero::FREE) { // don't buy an item while pushing
    // TODO
  }
}

/**
 * Displays the entity on the map.
 */
void ShopItem::display_on_map(void) {

  SDL_Surface *map_surface = map->get_visible_surface();
  int x = get_x();
  int y = get_y();

  // display the treasure
  const SDL_Rect &camera_position = map->get_camera_position();
  treasure->display(map_surface, x - camera_position.x, y - camera_position.y);

  // TODO also display the price

  price_counter->display(map_surface, x + 2, y + 28);
  SDL_Rect dst_position = {x - 8, y + 28};
  SDL_BlitSurface(rupee_icon, &rupee_icon_src_position, map_surface, &dst_position);
}
