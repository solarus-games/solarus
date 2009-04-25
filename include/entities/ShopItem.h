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
#ifndef ZSDX_SHOP_ITEM_H
#define ZSDX_SHOP_ITEM_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * Represents an item the hero can buy in a shop.
 */
class ShopItem: public Detector {

 private:

  // data
  Treasure *treasure;               /**< the treasure the player can buy */
  int price;                        /**< the treasure's price in rupees */
  MessageId message_id;             /**< id of the message describing the shop item */

  // displaying
  Counter *price_counter;           /**< the digits that show the price */
  SDL_Surface *rupee_icon;          /**< the rupee icon near the price */
  SDL_Rect rupee_icon_src_position; /**< position of the green rupee in the image */

 public:

  ShopItem(const std::string &name, Layer layer, int x, int y,
	   Treasure *treasure, int price, MessageId message_id);
  ~ShopItem(void);

  EntityType get_type(void);

  bool is_obstacle_for(MapEntity *other);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  void action_key_pressed(void);
  void display_on_map(void);
};

#endif
