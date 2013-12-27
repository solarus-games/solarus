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
#ifndef SOLARUS_SHOP_TREASURE_H
#define SOLARUS_SHOP_TREASURE_H

#include "Common.h"
#include "Treasure.h"
#include "Sprite.h"
#include "entities/Detector.h"
#include "lowlevel/TextSurface.h"

struct lua_State;

namespace solarus {

/**
 * \brief A treasure the hero can buy in a shop.
 *
 * The treasure is automatically drawn with its icon,
 * the amount and the price. When the hero presses the action
 * key in front of it, a dialog is automatically showed and
 * the hero can buy the item.
 */
class ShopTreasure: public Detector {

  public:

    ~ShopTreasure();
    static ShopTreasure* create(
        Game& game,
        const std::string& name,
        Layer layer,
        int x,
        int y,
        const Treasure& treasure,
        int price,
        const std::string& dialog_id);

    EntityType get_type() const;

    const Treasure& get_treasure() const;
    int get_price() const;
    const std::string& get_dialog_id() const;

    bool is_sword_ignored() const;
    bool is_obstacle_for(const MapEntity& other) const;
    void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode);
    void notify_action_command_pressed();

    void draw_on_map();

  private:

    ShopTreasure(
        const std::string& name,
        Layer layer,
        int x,
        int y,
        const Treasure& treasure,
        int price,
        const std::string& dialog_id);

    // data
    Treasure treasure;                /**< the treasure the player can buy */
    int price;                        /**< the treasure's price in rupees */
    std::string dialog_id;            /**< id of the dialog describing the shop item */

    // displaying
    TextSurface price_digits;         /**< the digits that show the price */
    Sprite rupee_icon_sprite;         /**< the rupee icon near the price */
};

}

#endif

