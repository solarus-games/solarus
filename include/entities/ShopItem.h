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
    TextSurface *price_digits;        /**< the digits that show the price */
    Sprite *rupee_icon_sprite;        /**< the rupee icon near the price */

    // state
    bool is_looking_item;             /**< indicates that the message describing the item is being shown */
    bool is_asking_question;          /**< indicates that the buy question is being shown */

    ShopItem(const std::string &name, Layer layer, int x, int y,
	Treasure *treasure, int price, const MessageId &message_id);

  public:

    ~ShopItem(void);
    static ShopItem * create(const std::string &name, Layer layer, int x, int y,
	Treasure *treasure, int price, const MessageId &message_id);
    static CreationFunction parse;

    EntityType get_type(void);
    bool can_be_added(Map *map);
    void set_map(Map *map);

    bool is_sword_ignored(void);
    bool is_obstacle_for(MapEntity *other);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
    void action_key_pressed(void);

    void update(void);
    void display_on_map(void);
};

#endif

