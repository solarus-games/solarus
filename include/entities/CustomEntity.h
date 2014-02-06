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
#ifndef SOLARUS_CUSTOM_ENTITY_H
#define SOLARUS_CUSTOM_ENTITY_H

#include "Common.h"
#include "entities/Detector.h"

namespace solarus {

/**
 * \brief A map entity fully controlled by Lua.
 *
 * A custom entity may have a model, which is the name of a Lua script
 * that controls it. This is useful to make several custom entities with the
 * same behavior more easily.
 */
class CustomEntity: public Detector {

  public:

    CustomEntity(
        Game& game,
        const std::string& name,
        int direction,
        Layer layer,
        int x,
        int y,
        int width,
        int height,
        const std::string& sprite_name,
        const std::string& model);
    ~CustomEntity();

    EntityType get_type() const;

    const std::string& get_model() const;

    void set_map(Map& map);

    bool is_obstacle_for(const MapEntity& other) const;

    virtual bool test_collision_custom(MapEntity& entity);

    /* TODO
    bool can_be_obstacle() const;
    bool is_ground_observer() const;
    const Rectangle get_ground_point() const;
    bool is_ground_modifier() const;
    Ground get_modified_ground() const;
    bool can_be_drawn() const;
    bool is_drawn_in_y_order() const;
    bool is_drawn_at_its_position() const;
    const Rectangle get_facing_point() const;
    const Rectangle get_facing_point(int direction) const;

    void notify_map_started();
    void notify_map_opening_transition_finished();
    void notify_tileset_changed();
    void notify_being_removed();
    void notify_enabled(bool enabled);
    void notify_obstacle_reached();
    void notify_position_changed();
    void notify_layer_changed();
    void notify_ground_below_changed();
    void notify_movement_changed();
    void notify_movement_finished();
    void notify_facing_entity_changed(Detector* facing_entity);
    bool has_layer_independent_collisions() const;
    void notify_collision_with_enemy(Enemy& enemy);
    void notify_collision_with_destructible(Destructible& destructible, CollisionMode collision_mode);
    void notify_collision_with_teletransporter(Teletransporter& teletransporter, CollisionMode collision_mode);
    void notify_collision_with_conveyor_belt(ConveyorBelt& conveyor_belt, int dx, int dy);
    void notify_collision_with_stairs(Stairs& stairs, CollisionMode collision_mode);
    void notify_collision_with_jumper(Jumper& jumper, CollisionMode collision_mode);
    void notify_collision_with_sensor(Sensor& sensor, CollisionMode collision_mode);
    void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    void notify_collision_with_switch(Switch& sw, Sprite& sprite_overlapping);
    void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode);
    void notify_collision_with_crystal(Crystal& crystal, Sprite& sprite_overlapping);
    void notify_collision_with_chest(Chest& chest);
    void notify_collision_with_block(Block& block);
    void notify_collision_with_separator(Separator& separator, CollisionMode collision_mode);
    void notify_collision_with_bomb(Bomb& bomb, CollisionMode collision_mode);
    void notify_collision_with_explosion(Explosion& explosion, CollisionMode collision_mode);
    void notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping);
    void notify_collision_with_fire(Fire& fire, Sprite& sprite_overlapping);
    void notify_collision_with_enemy(Enemy& enemy, Sprite& enemy_sprite, Sprite& this_sprite);
    void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed);
    bool is_low_wall_obstacle() const;
    bool is_shallow_water_obstacle() const;
    bool is_deep_water_obstacle() const;
    bool is_hole_obstacle() const;
    bool is_lava_obstacle() const;
    bool is_prickle_obstacle() const;
    bool is_ladder_obstacle() const;
    bool is_hero_obstacle(const Hero& hero) const;
    bool is_block_obstacle(const Block& block) const;
    bool is_teletransporter_obstacle(const Teletransporter& teletransporter) const;
    bool is_conveyor_belt_obstacle(const ConveyorBelt& conveyor_belt) const;
    bool is_stairs_obstacle(const Stairs& stairs) const;
    bool is_sensor_obstacle(const Sensor& sensor) const;
    bool is_switch_obstacle(const Switch& sw) const;
    bool is_raised_block_obstacle(const CrystalBlock& raised_block) const;
    bool is_crystal_obstacle(const Crystal& crystal) const;
    bool is_npc_obstacle(const Npc& npc) const;
    bool is_enemy_obstacle(const Enemy& enemy) const;
    bool is_jumper_obstacle(const Jumper& jumper) const;
    bool is_destructible_obstacle(const Destructible& destructible) const;
    bool is_separator_obstacle(const Separator& separator) const;
    bool is_sword_ignored() const;
    void set_suspended(bool suspended);
    void update();
    void draw_on_map();

    virtual void notify_action_command_pressed();
    virtual bool interaction_with_item(EquipmentItem& item);
    virtual bool start_movement_by_hero();
    virtual void stop_movement_by_hero();
    virtual std::string get_sword_tapping_sound();
*/

  private:

    void initialize_sprite(
        const std::string& sprite_name, int initial_direction);

    const std::string& model;          /**< Model of this custom entity or an empty string. */

};

}

#endif

