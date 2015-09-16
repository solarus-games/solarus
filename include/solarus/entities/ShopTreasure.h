/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/Common.h"
#include "solarus/Treasure.h"
#include "solarus/Sprite.h"
#include "solarus/entities/Detector.h"
#include "solarus/lowlevel/TextSurface.h"
#include <memory>
#include <string>

struct lua_State;

namespace Solarus {

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

    ShopTreasure(
        const std::string& name,
        int layer,
        const Point& xy,
        const Treasure& treasure,
        int price,
        const std::string& font_id,
        const std::string& dialog_id
    );

    static std::shared_ptr<ShopTreasure> create(
        Game& game,
        const std::string& name,
        int layer,
        const Point& xy,
        const Treasure& treasure,
        int price,
        const std::string& font_id,
        const std::string& dialog_id
    );

    virtual EntityType get_type() const override;

    const Treasure& get_treasure() const;
    int get_price() const;
    const std::string& get_dialog_id() const;

    virtual bool is_sword_ignored() const override;
    virtual bool is_obstacle_for(Entity& other) override;
    virtual void notify_collision(Entity& entity_overlapping, CollisionMode collision_mode) override;
    virtual bool notify_action_command_pressed() override;

    virtual void draw_on_map() override;

  private:

    // data
    Treasure treasure;                /**< The treasure the player can buy. */
    int price;                        /**< The treasure's price. */
    std::string dialog_id;            /**< Id of the dialog describing the treasure. */

    // displaying
    TextSurface price_digits;         /**< The digits that show the price. */
    SpritePtr
        rupee_icon_sprite;            /**< The rupee icon near the price. */

};

}

#endif

