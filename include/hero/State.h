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
#ifndef SOLARUS_HERO_STATE_H
#define SOLARUS_HERO_STATE_H

#include "Common.h"
#include "entities/Hero.h"
#include "entities/CarriedItem.h"

/**
 * \brief Abstract base class for a state of the hero.
 *
 * This class contains all hero-related functions that depend on his state
 * and provide a default implementation for them.
 * Most of them are almost empty here because they depend on the state.
 * Redefine for each state the functions that you need to implement or change.
 */
class Hero::State {

  private:

    Map* map;                 /**< the current map (it may change during this state) */

  protected:

    Hero& hero;               /**< the hero controlled by this state */
    bool suspended;           /**< indicates whether this state is suspended */
    uint32_t when_suspended;  /**< indicates when this state was suspended */

    State(Hero& hero);

    bool is_current_state();

    // access to various game objects
    LuaContext& get_lua_context();
    MapEntities& get_entities();
    Game& get_game();
    Map& get_map();
    Equipment& get_equipment();
    KeysEffect& get_keys_effect();
    GameCommands& get_commands();
    DialogBox& get_dialog_box();
    HeroSprites& get_sprites();

  public:

    // creation and destruction
    virtual ~State();
    virtual void start(State* previous_state);
    virtual void stop(State* next_state);

    // game loop
    virtual void update();
    virtual void draw_on_map();
    virtual void set_suspended(bool suspended);
    void notify_command_pressed(GameCommands::Command command);
    void notify_command_released(GameCommands::Command command);
    virtual void notify_action_command_pressed();
    virtual void notify_action_command_released();
    virtual void notify_attack_command_pressed();
    virtual void notify_attack_command_released();
    virtual void notify_direction_command_pressed(int direction4);
    virtual void notify_direction_command_released(int direction4);
    virtual void notify_item_command_pressed(int slot);
    virtual void notify_item_command_released(int slot);

    // game
    virtual void set_map(Map& map);
    virtual bool can_start_gameover_sequence();

    // sprites
    virtual bool is_hero_visible();
    virtual bool is_direction_locked();

    // movement
    virtual bool can_control_movement();
    virtual int get_wanted_movement_direction8();
    virtual void notify_walking_speed_changed();
    virtual void notify_layer_changed();
    virtual void notify_movement_changed();
    virtual void notify_movement_finished();
    virtual void notify_obstacle_reached();
    virtual void notify_position_changed();

    // ground
    virtual bool can_avoid_deep_water();
    virtual bool can_avoid_hole();
    virtual bool can_avoid_lava();
    virtual bool can_avoid_prickle();
    virtual bool is_touching_ground();
    virtual bool can_come_from_bad_ground();
    virtual void notify_ground_changed();

    // obstacles and collisions
    virtual bool are_collisions_ignored();
    virtual bool is_shallow_water_obstacle();
    virtual bool is_deep_water_obstacle();
    virtual bool is_hole_obstacle();
    virtual bool is_lava_obstacle();
    virtual bool is_prickle_obstacle();
    virtual bool is_ladder_obstacle();
    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    virtual bool can_avoid_teletransporter();
    virtual bool is_teletransporter_delayed();
    virtual bool is_conveyor_belt_obstacle(ConveyorBelt& conveyor_belt);
    virtual bool can_avoid_conveyor_belt();
    virtual bool is_stairs_obstacle(Stairs& stairs);
    virtual bool is_sensor_obstacle(Sensor& sensor);
    virtual bool is_jumper_obstacle(Jumper& jumper);
    virtual bool can_avoid_sensor();
    virtual bool can_avoid_explosion();
    virtual bool can_avoid_switch();

    // enemies
    virtual void notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
        EnemyReaction::Reaction& result, bool killed);
    virtual int get_sword_damage_factor();
    virtual bool can_be_hurt(Enemy* attacker);

    // state specific
    virtual bool is_free();
    virtual bool is_using_item();
    virtual EquipmentItemUsage& get_item_being_used();
    virtual bool is_grabbing_or_pulling();
    virtual bool is_moving_grabbed_entity();
    virtual void notify_grabbed_entity_collision();
    virtual bool is_cutting_with_sword(Detector& detector);
    virtual bool can_start_sword();
    virtual bool can_pick_treasure(EquipmentItem& item);
    virtual bool can_start_item(EquipmentItem& item);
    virtual bool can_take_stairs();
    virtual bool can_take_jumper();
    virtual bool can_sword_hit_crystal();
    bool is_carrying_item();
    virtual CarriedItem* get_carried_item();
    virtual CarriedItem::Behavior get_previous_carried_item_behavior(CarriedItem& carried_item);
};

#endif

