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
#ifndef SOLARUS_ENTITY_STATE_H
#define SOLARUS_ENTITY_STATE_H

#include "solarus/Common.h"
#include "solarus/entities/CarriedItem.h"
#include "solarus/entities/Hero.h"
#include <cstdint>
#include <memory>
#include <string>

namespace Solarus {

/**
 * \brief Abstract base class for a state of the hero.
 *
 * This class contains all hero-related functions that depend on his state
 * and provide a default implementation for them.
 * Most of them are almost empty here because they depend on the state.
 * Redefine for each state the functions that you need to implement or change.
 */
class Entity::State {

  public:

    // creation and destruction
    virtual ~State();
    const std::string& get_name() const;
    virtual void start(const State* previous_state);
    virtual void stop(const State* next_state);

    // game loop
    virtual void update();
    virtual void draw_on_map();
    bool is_suspended() const;
    virtual void set_suspended(bool suspended);
    uint32_t get_when_suspended() const;
    void notify_command_pressed(GameCommand command);
    void notify_command_released(GameCommand command);
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
    virtual bool can_start_gameover_sequence() const;

    // sprites
    virtual bool is_visible() const;
    virtual bool is_direction_locked() const;

    // movement
    virtual bool can_control_movement() const;
    virtual int get_wanted_movement_direction8() const;
    virtual void notify_walking_speed_changed();
    virtual void notify_layer_changed();
    virtual void notify_movement_changed();
    virtual void notify_movement_finished();
    virtual void notify_obstacle_reached();
    virtual void notify_position_changed();

    // ground
    virtual bool can_avoid_deep_water() const;
    virtual bool can_avoid_hole() const;
    virtual bool can_avoid_ice() const;
    virtual bool can_avoid_lava() const;
    virtual bool can_avoid_prickle() const;
    virtual bool is_touching_ground() const;
    virtual bool can_come_from_bad_ground() const;
    virtual void notify_ground_changed();

    // obstacles and collisions
    virtual bool are_collisions_ignored() const;
    virtual bool is_shallow_water_obstacle() const;
    virtual bool is_deep_water_obstacle() const;
    virtual bool is_hole_obstacle() const;
    virtual bool is_lava_obstacle() const;
    virtual bool is_prickle_obstacle() const;
    virtual bool is_ladder_obstacle() const;
    virtual bool is_teletransporter_obstacle(const Teletransporter& teletransporter) const;
    virtual bool can_avoid_teletransporter() const;
    virtual bool is_teletransporter_delayed() const;
    virtual bool is_stream_obstacle(const Stream& stream) const;
    virtual bool can_avoid_stream(const Stream& stream) const;
    virtual bool can_persist_on_stream(const Stream& stream) const;
    virtual bool is_stairs_obstacle(const Stairs& stairs) const;
    virtual bool is_sensor_obstacle(const Sensor& sensor) const;
    virtual bool is_jumper_obstacle(const Jumper& jumper, const Rectangle& candidate_position) const;
    virtual bool is_separator_obstacle(const Separator& separator) const;
    virtual bool can_avoid_sensor() const;
    virtual bool can_avoid_explosion() const;
    virtual bool can_avoid_switch() const;

    // enemies
    virtual void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed);
    virtual int get_sword_damage_factor() const;
    virtual bool can_be_hurt(Entity* attacker) const;

    // state specific
    virtual bool is_free() const;
    virtual bool is_using_item() const;
    virtual EquipmentItemUsage& get_item_being_used();
    virtual bool is_brandishing_treasure() const;
    virtual bool is_grabbing_or_pulling() const;
    virtual bool is_moving_grabbed_entity() const;
    virtual void notify_grabbed_entity_collision();
    virtual bool is_cutting_with_sword(Detector& detector);
    virtual bool can_start_sword() const;
    virtual bool can_pick_treasure(EquipmentItem& item) const;
    virtual bool can_use_shield() const;
    virtual bool can_start_item(EquipmentItem& item) const;
    virtual bool can_take_stairs() const;
    virtual bool can_sword_hit_crystal() const;
    virtual bool can_take_jumper() const;
    virtual void notify_jumper_activated(Jumper& jumper);
    bool is_carrying_item() const;
    virtual std::shared_ptr<CarriedItem> get_carried_item() const;
    virtual CarriedItem::Behavior get_previous_carried_item_behavior() const;

  protected:

    State(Entity& entity, const std::string& state_name);

    bool is_current_state() const;
    bool is_stopping() const;

    // access to various game objects
    LuaContext& get_lua_context();
    MapEntities& get_entities();
    Game& get_game();
    const Game& get_game() const;
    Map& get_map();
    Equipment& get_equipment();
    const Equipment& get_equipment() const;
    KeysEffect& get_keys_effect();
    GameCommands& get_commands();
    const GameCommands& get_commands() const;
    virtual Entity& get_entity();
    virtual const Entity& get_entity() const;
    HeroSprites& get_sprites();
    const HeroSprites& get_sprites() const;

  private:

    Entity& entity;           /**< The entity controlled by this state. */
    bool suspended;           /**< Whether this state is suspended. */
    uint32_t when_suspended;  /**< When this state was suspended. */
    Map* map;                 /**< The current map (it may change during this state). */
    const std::string name;   /**< Name describing this state. */
    bool stopping;            /**< Indicates that this state is being stopped. */

};

}

#endif

