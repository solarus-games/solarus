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
#ifndef SOLARUS_HOOKSHOT_H
#define SOLARUS_HOOKSHOT_H

#include "Common.h"
#include "entities/MapEntity.h"
#include "Sprite.h"

/**
 * @brief The hookshot thrown by the hero.
 */
class Hookshot: public MapEntity {

  private:

    uint32_t next_sound_date;    /**< date when the hookshot sound is be played next time */

    bool has_to_go_back;         /**< true if the hookshot is about to go back */
    bool going_back;             /**< indicates that the hookshot is going back towards the hero */
    MapEntity* entity_reached;   /**< the entity the hookshot is attached to (or NULL) */

    Sprite link_sprite;          /**< sprite of the links */

  public:

    Hookshot(Hero &hero);
    ~Hookshot();

    EntityType get_type();

    // features
    bool can_be_obstacle();
    bool can_detect_entities();
    bool can_be_displayed();
    bool is_displayed_in_y_order();

    bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt& conveyor_belt);
    bool is_stairs_obstacle(Stairs& stairs);
    bool is_deep_water_obstacle();
    bool is_hole_obstacle();
    bool is_lava_obstacle();
    bool is_prickle_obstacle();
    bool is_ladder_obstacle();
    bool is_switch_obstacle(Switch& sw);
    bool is_crystal_switch_obstacle(CrystalSwitch& crystal_switch);
    bool is_jump_sensor_obstacle(JumpSensor& jump_sensor);

    // state
    void update();
    virtual void display_on_map();
    const Rectangle get_facing_point();
    bool is_flying();
    bool is_going_back();
    void go_back();
    void attach_to(MapEntity& entity_reached);

    // collisions
    void notify_movement_tried(bool success);
    void notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite);
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim, EnemyReaction::Reaction& result, bool killed);
    void notify_collision_with_chest(Chest& chest);
    void notify_collision_with_destructible_item(DestructibleItem& destructible_item, CollisionMode collision_mode);
    void notify_collision_with_block(Block& block);
    void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    void notify_collision_with_crystal_switch(CrystalSwitch& crystal_switch, CollisionMode collision_mode);
};

#endif

