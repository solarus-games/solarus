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
#ifndef SOLARUS_PICKABLE_H
#define SOLARUS_PICKABLE_H

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"
#include "solarus/entities/MapEntityPtr.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/movements/FallingHeight.h"
#include "solarus/SpritePtr.h"
#include "solarus/Treasure.h"

namespace Solarus {

/**
 * \brief An treasure placed on the ground and that the hero can take.
 */
class Pickable: public Detector {

  public:

    // creation and destruction
    Pickable(
        const std::string& name,
        Layer layer,
        const Point& xy,
        const Treasure& treasure
    );

    static std::shared_ptr<Pickable> create(
        Game& game,
        const std::string& name,
        Layer layer,
        const Point& xy,
        Treasure treasure,
        FallingHeight falling_height,
        bool force_persistent
    );

    virtual EntityType get_type() const override;
    virtual bool can_be_obstacle() const override;

    virtual void notify_created() override;

    // properties
    bool is_falling() const;
    FallingHeight get_falling_height() const;
    const Treasure& get_treasure() const;

    // item state
    virtual void set_suspended(bool suspended) override;
    MapEntityPtr get_entity_followed();
    virtual void notify_collision(
        MapEntity& entity_overlapping,
        CollisionMode collision_mode
    ) override;
    virtual void notify_collision(
        MapEntity& other_entity,
        Sprite& this_sprite,
        Sprite& other_sprite
    ) override;
    virtual void update() override;
    virtual void draw_on_map() override;

  private:

    // initialization
    bool initialize_sprites();
    void initialize_movement();

    // item
    void try_give_item_to_player();
    void set_blinking(bool blinking);

    Treasure treasure;                          /**< the treasure obtained when the player picks this item */
    bool given_to_player;                       /**< indicates that the treasure was given to the player */

    SpritePtr shadow_sprite;                    /**< sprite of the shadow (if any) */
    FallingHeight falling_height;               /**< indicates whether the item is falling when it appears */
    bool will_disappear;                        /**< indicates that the item will disappear after an amount of time
                                                 * (only possible for items not saved) */

    // current state
    Point shadow_xy;                            /**< coordinates of the shadow (which does not move while the item does) */

    uint32_t appear_date;                       /**< date when the item is created */
    uint32_t allow_pick_date;                   /**< date when the item can be picked */
    bool can_be_picked;                         /**< indicates that the item can be picked now (i.e. allow_picked_date is past) */
    uint32_t blink_date;                        /**< date when the item starts blinking */
    uint32_t disappear_date;                    /**< date when the item disappears */
    MapEntityPtr entity_followed;               /**< an entity this item is attached to (e.g. a boomerang or a hookshot) */
};

}

#endif

