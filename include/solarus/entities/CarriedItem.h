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
#ifndef SOLARUS_CARRIED_ITEM_H
#define SOLARUS_CARRIED_ITEM_H

#include "solarus/Common.h"
#include "solarus/entities/MapEntity.h"
#include "solarus/SpritePtr.h"
#include <string>

namespace Solarus {

/**
 * \brief An item carried or thrown by the hero.
 *
 * Represents an item (pot, bush...) carried by the hero. He can throw it.
 * While the hero is carrying it, the item is attached to the hero, not to the map.
 * As soon as he throws it, the item becomes attached to the map and the hero
 * may lift another item.
 */
class CarriedItem: public Entity {

  public:

    /**
     * Indicates what to do with a carried item.
     */
    enum Behavior {
      BEHAVIOR_THROW,          /**< make the hero throw the item */
      BEHAVIOR_DESTROY,        /**< destroy the item silently */
      BEHAVIOR_KEEP            /**< let the hero continue to carry the item */
    };

    CarriedItem(
        Hero& hero,
        const Entity& original_entity,
        const std::string& animation_set_id,
        const std::string& destruction_sound_id,
        int damage_on_enemies,
        uint32_t explosion_date
    );

    virtual EntityType get_type() const override;
    virtual bool can_be_obstacle() const override;
    virtual bool is_ground_observer() const override;

    int get_damage_on_enemies() const;

    void set_animation_stopped();
    void set_animation_walking();
    void throw_item(int direction);
    bool is_being_lifted() const;
    bool is_being_thrown() const;
    void break_item();
    void break_item_on_ground();
    bool is_broken() const;
    bool can_explode() const;

    virtual void set_suspended(bool suspended) override;
    virtual void update() override;
    virtual void draw_on_map() override;

    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter) override;
    virtual bool is_stream_obstacle(Stream& stream) override;
    virtual bool is_stairs_obstacle(Stairs& stairs) override;
    virtual bool is_low_wall_obstacle() const override;
    virtual bool is_deep_water_obstacle() const override;
    virtual bool is_hole_obstacle() const override;
    virtual bool is_lava_obstacle() const override;
    virtual bool is_prickle_obstacle() const override;
    virtual bool is_ladder_obstacle() const override;
    virtual bool is_switch_obstacle(Switch& sw) override;
    virtual bool is_raised_block_obstacle(CrystalBlock& raised_block) override;
    virtual bool is_crystal_obstacle(Crystal& crystal) override;
    virtual bool is_sensor_obstacle(Sensor& sensor) override;
    virtual bool is_npc_obstacle(Npc& npc) override;
    virtual bool is_jumper_obstacle(Jumper& jumper, const Rectangle& candidate_position) override;
    virtual bool is_enemy_obstacle(Enemy& enemy) override;
    virtual void notify_obstacle_reached() override;
    virtual void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) override;
    virtual void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode) override;
    virtual void notify_collision_with_stairs(Stairs& stairs, CollisionMode collision_mode) override;
    virtual void notify_collision_with_enemy(Enemy& enemy) override;
    virtual void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed
    ) override;

  private:

    bool will_explode_soon() const;

    // game data
    Hero& hero;                 /**< the hero, who is carrying or throwing this item */

    // state
    bool is_lifting;            /**< indicates that the hero is lifting this item */
    bool is_throwing;           /**< indicates that the item is being thrown */
    bool is_breaking;           /**< indicates that the item is breaking */
    bool break_one_layer_above; /**< indicates that the item has to get broken
                                 * now one layer above its current position */
    std::string destruction_sound_id;   /**< the sound played when the item breaks */
    int damage_on_enemies;      /**< damage for an enemy that receives the item */

    // throwing the item
    SpritePtr shadow_sprite;    /**< sprite of the shadow when the item is being thrown */
    int throwing_direction;     /**< direction where the item is thrown (0 to 3) */
    uint32_t next_down_date;    /**< when the item is thrown, date when it move one pixel downwards next time */
    int item_height;            /**< current height where the item is drawn above its shadow */
    int y_increment;            /**< next y change for item_height */

    // explosion of the item
    uint32_t explosion_date;    /**< date when the item explodes (0 if there is no explosion) */

    static const std::string lifting_trajectories[4];   /**< trajectory of the lifting movement for each direction */

};

}

#endif

