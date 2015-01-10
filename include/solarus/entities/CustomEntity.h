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

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"
#include "solarus/lua/ScopedLuaRef.h"
#include <map>

namespace Solarus {

/**
 * \brief A map entity fully controlled by Lua.
 *
 * A custom entity may have a model, which is the name of a Lua script
 * that controls it. This is useful to make several custom entities with the
 * same behavior more easily.
 */
class SOLARUS_API CustomEntity: public Detector {

  public:

    CustomEntity(
        Game& game,
        const std::string& name,
        int direction,
        Layer layer,
        const Point& xy,
        const Size& size,
        const std::string& sprite_name,
        const std::string& model
    );

    virtual EntityType get_type() const override;

    const std::string& get_model() const;

    // Game loop.
    virtual void notify_creating() override;
    virtual void set_suspended(bool suspended) override;
    virtual void notify_enabled(bool enabled) override;
    virtual void update() override;
    virtual void draw_on_map() override;

    // Direction.
    int get_sprites_direction() const;
    void set_sprites_direction(int direction);

    // What can traverse this custom entity.
    bool is_traversable_by_entity(MapEntity& entity);
    void set_traversable_by_entities(bool traversable);
    void set_traversable_by_entities(const ScopedLuaRef& traversable_test_ref);
    void reset_traversable_by_entities();
    void set_traversable_by_entities(EntityType type, bool traversable);
    void set_traversable_by_entities(EntityType type, const ScopedLuaRef& traversable_test_ref);
    void reset_traversable_by_entities(EntityType type);

    virtual bool can_be_obstacle() const override;
    virtual bool is_obstacle_for(MapEntity& other) override;

    // What this custom entity can traverse.
    void set_can_traverse_entities(bool traversable);
    void set_can_traverse_entities(const ScopedLuaRef& traversable_test_ref);
    void reset_can_traverse_entities();
    void set_can_traverse_entities(EntityType type, bool traversable);
    void set_can_traverse_entities(
        EntityType type,
        const ScopedLuaRef& traversable_test_ref
    );
    void reset_can_traverse_entities(EntityType type);

    virtual bool is_hero_obstacle(Hero& hero) override;
    virtual bool is_block_obstacle(Block& block) override;
    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter) override;
    virtual bool is_stream_obstacle(Stream& stream) override;
    virtual bool is_stairs_obstacle(Stairs& stairs) override;
    virtual bool is_sensor_obstacle(Sensor& sensor) override;
    virtual bool is_switch_obstacle(Switch& sw) override;
    virtual bool is_raised_block_obstacle(CrystalBlock& raised_block) override;
    virtual bool is_crystal_obstacle(Crystal& crystal) override;
    virtual bool is_npc_obstacle(Npc& npc) override;
    virtual bool is_enemy_obstacle(Enemy& enemy) override;
    virtual bool is_jumper_obstacle(Jumper& jumper, const Rectangle& candidate_position) override;
    virtual bool is_destructible_obstacle(Destructible& destructible) override;
    virtual bool is_separator_obstacle(Separator& separator) override;

    bool can_traverse_ground(Ground ground) const;
    void set_can_traverse_ground(Ground ground, bool traversable);
    void reset_can_traverse_ground(Ground ground);

    virtual bool is_low_wall_obstacle() const override;
    virtual bool is_shallow_water_obstacle() const override;
    virtual bool is_deep_water_obstacle() const override;
    virtual bool is_hole_obstacle() const override;
    virtual bool is_lava_obstacle() const override;
    virtual bool is_prickle_obstacle() const override;
    virtual bool is_ladder_obstacle() const override;

    // Collisions.
    void add_collision_test(
        CollisionMode collision_test,
        const ScopedLuaRef& callback_ref
    );
    void add_collision_test(
        const ScopedLuaRef& collision_test_ref,
        const ScopedLuaRef& callback_ref
    );
    void clear_collision_tests();

    virtual bool test_collision_custom(MapEntity& entity) override;
    virtual void notify_collision(
        MapEntity& entity_overlapping, CollisionMode collision_mode) override;
    virtual void notify_collision(
        MapEntity& other_entity, Sprite& this_sprite, Sprite& other_sprite) override;

    virtual void notify_collision_with_destructible(Destructible& destructible, CollisionMode collision_mode) override;
    virtual void notify_collision_with_teletransporter(Teletransporter& teletransporter, CollisionMode collision_mode) override;
    virtual void notify_collision_with_stream(Stream& stream, int dx, int dy) override;
    virtual void notify_collision_with_stairs(Stairs& stairs, CollisionMode collision_mode) override;
    virtual void notify_collision_with_jumper(Jumper& jumper, CollisionMode collision_mode) override;
    virtual void notify_collision_with_sensor(
        Sensor& sensor, CollisionMode collision_mode) override;
    virtual void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) override;
    virtual void notify_collision_with_switch(Switch& sw, Sprite& sprite_overlapping) override;
    virtual void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode) override;
    virtual void notify_collision_with_crystal(Crystal& crystal, Sprite& sprite_overlapping) override;
    virtual void notify_collision_with_chest(Chest& chest) override;
    virtual void notify_collision_with_block(Block& block) override;
    virtual void notify_collision_with_separator(Separator& separator, CollisionMode collision_mode) override;
    virtual void notify_collision_with_bomb(Bomb& bomb, CollisionMode collision_mode) override;
    virtual void notify_collision_with_explosion(Explosion& explosion, CollisionMode collision_mode) override;
    virtual void notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping) override;
    virtual void notify_collision_with_fire(Fire& fire, Sprite& sprite_overlapping) override;
    virtual void notify_collision_with_enemy(Enemy& enemy) override;
    virtual void notify_collision_with_enemy(Enemy& enemy, Sprite& enemy_sprite, Sprite& this_sprite) override;
    virtual bool notify_action_command_pressed() override;
    virtual bool interaction_with_item(EquipmentItem& item) override;

    // Observing the ground.
    virtual bool is_ground_observer() const override;
    virtual void notify_ground_below_changed() override;

    // Modifying the ground.
    virtual bool is_ground_modifier() const override;
    virtual Ground get_modified_ground() const override;
    void set_modified_ground(Ground modified_ground);

  private:

    /**
     * \brief Stores whether a custom entity can be traversed by or can traverse
     * other entities.
     */
    class TraversableInfo {

      public:

        TraversableInfo();
        TraversableInfo(
            LuaContext& lua_context,
            bool traversable
        );
        TraversableInfo(
            LuaContext& lua_context,
            const ScopedLuaRef& traversable_test_ref
        );

        bool is_empty() const;
        bool is_traversable(
            CustomEntity& current_entity,
            MapEntity& other_entity
        ) const;

      private:

        LuaContext* lua_context;       /**< The Lua world.
                                        * nullptr means no info. */
        ScopedLuaRef
            traversable_test_ref;      /**< Lua ref to a boolean function
                                        * that decides, or LUA_REFNIL. */
        bool traversable;              /**< Traversable property (unused if
                                        * there is a Lua function). */

    };

    /**
     * \brief Stores a callback to be executed when the specified test
     * detects a collision.
     */
    class CollisionInfo {

      public:

        CollisionInfo();
        CollisionInfo(
            LuaContext& lua_context,
            CollisionMode built_in_test,
            const ScopedLuaRef& callback_ref
        );
        CollisionInfo(
            LuaContext& lua_context,
            const ScopedLuaRef& custom_test_ref,
            const ScopedLuaRef& callback_ref
        );

        CollisionMode get_built_in_test() const;
        const ScopedLuaRef& get_custom_test_ref() const;
        const ScopedLuaRef& get_callback_ref() const;

      private:

        LuaContext* lua_context;         /**< The Lua world.
                                          * nullptr means no info. */
        CollisionMode built_in_test;     /**< A built-in collision test
                                          * or COLLISION_CUSTOM. */
        ScopedLuaRef custom_test_ref;    /**< Ref to a custom collision test
                                          * or LUA_REFNIL. */
        ScopedLuaRef callback_ref;       /**< Ref to the function to called when
                                          * a collision is detected. */

    };

    const TraversableInfo& get_traversable_by_entity_info(EntityType type);
    const TraversableInfo& get_can_traverse_entity_info(EntityType type);

    void notify_collision_from(MapEntity& other_entity);
    void notify_collision_from(MapEntity& other_entity, Sprite& other_sprite, Sprite& this_sprite);

    std::string model;                                /**< Model of this custom entity or an empty string. */

    // Obstacles.

    TraversableInfo traversable_by_entities_general;  /**< Whether entities can traverse me by default or nullptr. */
    std::map<EntityType, TraversableInfo>
        traversable_by_entities_type;                 /**< Whether entities of a type can traverse me. */

    TraversableInfo can_traverse_entities_general;    /**< Whether I can traverse entities by default or nullptr. */
    std::map<EntityType, TraversableInfo>
        can_traverse_entities_type;                   /**< Whether I can traverse entities of a type. */
    std::map<Ground, bool> can_traverse_grounds;      /**< Whether I can traverse each kind of ground. */

    // Collisions.

    std::vector<CollisionInfo>
        collision_tests;               /**< The collision tests to perform. */
    std::vector<CollisionInfo>
        successful_collision_tests;    /**< Collision test that detected
                                        * collisions other than
                                        * COLLISION_SPRITE. */

    bool ground_modifier;              /**< Whether this custom entity is a ground modifier. */
    Ground modified_ground;            /**< The ground defined by this custom
                                        * entity or GROUND_EMPTY. */
};

}

#endif

