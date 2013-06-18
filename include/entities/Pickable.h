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
#ifndef SOLARUS_PICKABLE_H
#define SOLARUS_PICKABLE_H

#include "Common.h"
#include "Treasure.h"
#include "entities/Detector.h"
#include "movements/FallingHeight.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief An treasure placed on the ground and that the hero can take.
 */
class Pickable: public Detector {

  public:

    // creation and destruction
    static Pickable* create(Game& game, const std::string& name,
        Layer layer, int x, int y, Treasure treasure,
        FallingHeight falling_height, bool force_persistent);

    ~Pickable();

    EntityType get_type();
    bool can_be_obstacle();

    void set_map(Map& map);
    void notify_map_started();

    // properties
    bool is_falling();
    FallingHeight get_falling_height();
    const Treasure& get_treasure();

    // item state
    void set_suspended(bool suspended);
    MapEntity* get_entity_followed();
    void notify_movement_changed();
    void notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode);
    void notify_collision(MapEntity &other_entity, Sprite &other_sprite, Sprite &this_sprite);
    void update();
    void draw_on_map();

    virtual const std::string& get_lua_type_name() const;

  private:

    // creation and initialization
    Pickable(const std::string& name, Layer layer, int x, int y,
        const Treasure& treasure);

    void initialize_sprites();
    void initialize_movement();

    // item
    void give_item_to_player();
    void set_blinking(bool blinking);

    Treasure treasure;                          /**< the treasure obtained when the player picks this item */
    bool given_to_player;                       /**< indicates that the treasure was given to the player */

    Sprite* shadow_sprite;                      /**< sprite of the shadow (if any) */
    FallingHeight falling_height;               /**< indicates whether the item is falling when it appears */
    bool will_disappear;                        /**< indicates that the item will disappear after an amount of time
                                                 * (only possible for items not saved) */

    // current state
    Rectangle shadow_xy;                        /**< coordinates of the shadow (which does not move while the item does) */

    uint32_t appear_date;                       /**< date when the item is created */
    uint32_t allow_pick_date;                   /**< date when the item can be picked */
    bool can_be_picked;                         /**< indicates that the item can be picked now (i.e. allow_picked_date is past) */
    uint32_t blink_date;                        /**< date when the item starts blinking */
    uint32_t disappear_date;                    /**< date when the item disappears */
    MapEntity* entity_followed;                 /**< an entity this item is attached to (e.g. a boomerang or a hookshot) */
};

#endif

