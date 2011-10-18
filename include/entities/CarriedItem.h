/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * @brief An item carried or thrown by the hero.
 *
 * Represents an item (pot, bush...) carried by the hero. He can throw it.
 * While the hero is carrying it, the item is attached to the hero, not to the map.
 * As soon as he throws it, the item becomes attached to the map and the hero
 * may lift another item.
 */
class CarriedItem: public MapEntity {

  private:

    // game data
    Hero &hero;				/**< the hero, who is carrying or throwing this item */

    // state
    bool is_lifting;			/**< indicates that the hero is lifting this item */
    bool is_throwing;			/**< indicates that the item is being thrown */
    bool is_breaking;			/**< indicates that the item is breaking */
    bool break_on_intermediate_layer;	/**<  indicates that the item has to get broken now and on the intermediate layer instead of the lower one */
    SoundId destruction_sound_id;	/**< the sound played when the item breaks */
    int damage_on_enemies;		/**< damage for an enemy that receives the item */

    // throwing the item
    Sprite *shadow_sprite;		/**< sprite of the shadow when the item is being thrown */
    int throwing_direction;		/**< direction where the item is thrown (0 to 3) */
    uint32_t next_down_date;		/**< when the item is thrown, date when it move one pixel downwards next time */
    int item_height;			/**< current height where the item is displayed above its shadow */
    int y_increment;			/**< next y change for item_height */

    // explosion of the item
    uint32_t explosion_date;		/**< date when the item explodes (0 if there is no explosion) */

    static const std::string lifting_trajectories[4];	/**< trajectory of the lifting movement for each direction */

    void initialize(const Rectangle& xy, const Rectangle& size,
	const Rectangle& origin,
        const SpriteAnimationSetId& animation_set_id,
        const SoundId& destruction_sound_id,
        int damage_on_enemies, uint32_t explosion_date);

    bool will_explode_soon();

  public:

    /**
     * Indicates what to do with a carried item.
     */
    enum Behavior {
      BEHAVIOR_THROW,          /**< make the hero throw the item */
      BEHAVIOR_DESTROY,        /**< destroy the item silently */
      BEHAVIOR_KEEP            /**< let the hero continue to carry the item */
    };

    CarriedItem(Hero &hero, MapEntity &original_entity,
	const SpriteAnimationSetId& animation_set_id,
        const SoundId& destruction_sound_id,
        int damage_on_enemies,
        uint32_t explosion_date);
    ~CarriedItem();

    EntityType get_type();

    bool can_be_obstacle();
    bool can_detect_entities();
    bool can_be_displayed();
    bool is_displayed_in_y_order();

    int get_damage_on_enemies();

    void set_animation_stopped();
    void set_animation_walking();
    void throw_item(int direction);
    bool is_being_lifted();
    bool is_being_thrown();
    void break_item();
    bool is_broken();
    bool can_explode();

    void set_suspended(bool suspended);
    void update();
    void display_on_map();

    bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt& conveyor_belt);
    bool is_stairs_obstacle(Stairs& stairs);
    bool is_deep_water_obstacle();
    bool is_hole_obstacle();
    bool is_lava_obstacle();
    bool is_prickle_obstacle();
    bool is_ladder_obstacle();
    bool is_switch_obstacle(Switch& sw);
    bool is_raised_block_obstacle(CrystalSwitchBlock& raised_block);
    bool is_crystal_switch_obstacle(CrystalSwitch& crystal_switch);
    bool is_sensor_obstacle(Sensor& sensor);
    bool is_npc_obstacle(InteractiveEntity& npc);
    bool is_jumper_obstacle(Jumper& jumper);
    bool is_enemy_obstacle(Enemy& enemy);
    void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    void notify_collision_with_crystal_switch(CrystalSwitch &crystal_switch, CollisionMode collision_mode);
    void notify_collision_with_stairs(Stairs &stairs, CollisionMode collision_mode);
    void notify_collision_with_enemy(Enemy &enemy);
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim, EnemyReaction::Reaction& result, bool killed);
};

#endif

