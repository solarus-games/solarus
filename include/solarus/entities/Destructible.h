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
#ifndef SOLARUS_DESTRUCTIBLE_H
#define SOLARUS_DESTRUCTIBLE_H

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"
#include "solarus/entities/Ground.h"
#include "solarus/Treasure.h"

namespace Solarus {

/**
 * \brief An entity that the hero can destroy and that may contain a pickable
 * treasure.
 *
 * Some destructible objects can be lifted and thrown (a pot, a stone, etc.),
 * some of them can be cut with the sword (for example some grass)
 * and others have both behaviors (for example a bush).
 * Some others can explode when they are lifted (a bomb or a bomb flower).
 * When the hero lifts a destructible object, it is removed from the map and
 * replaced by an instance of CarriedItem that is attached to the hero.
 */
class Destructible: public Detector {

  public:

    // Creation and destruction.
    Destructible(
        const std::string& name,
        Layer layer,
        const Point& xy,
        const std::string& animation_set_id,
        const Treasure& treasure,
        Ground modified_ground
    );

    // Properties overridden from MapEntity.
    virtual EntityType get_type() const override;
    virtual bool is_ground_modifier() const override;
    virtual Ground get_modified_ground() const override;

    // Properties specific to destructibles.
    const Treasure& get_treasure() const;
    void set_treasure(const Treasure& treasure);
    const std::string& get_animation_set_id() const;
    const std::string& get_destruction_sound() const;
    void set_destruction_sound(const std::string& destruction_sound);
    int get_weight() const;
    void set_weight(int weight);
    bool get_can_be_cut() const;
    void set_can_be_cut(bool can_be_cut);
    bool get_can_explode() const;
    void set_can_explode(bool can_explode);
    bool get_can_regenerate() const;
    void set_can_regenerate(bool can_regenerate);
    int get_damage_on_enemies() const;
    void set_damage_on_enemies(int damage_on_enemies);

    // State.
    void explode();
    bool is_waiting_for_regeneration() const;

    // Collisions.
    virtual bool is_obstacle_for(MapEntity& other) override;
    virtual bool test_collision_custom(MapEntity& entity) override;
    virtual void notify_collision(
        MapEntity& entity_overlapping,
        CollisionMode collision_mode
    ) override;
    virtual void notify_collision(
        MapEntity& other_entity,
        Sprite& this_sprite,
        Sprite& other_sprite
    ) override;
    void notify_collision_with_hero(Hero& hero, CollisionMode collision_mode);
    virtual bool notify_action_command_pressed() override;

    // Game loop.
    virtual void set_suspended(bool suspended) override;
    virtual void update() override;

  private:

    void update_collision_modes();
    void play_destroy_animation();
    void create_treasure();

    const Ground modified_ground;      /**< The ground defined by this entity. */
    Treasure treasure;                 /**< The pickable treasure that appears when this object is destroyed. */
    const std::string
        animation_set_id;              /**< Animation set of the sprite of this object. */
    std::string destruction_sound_id;  /**< Sound played when this object is cut or broken
                                        * or an empty string. */
    bool can_be_cut;                   /**< Whether this object can be cut with the sword. */
    bool can_explode;                  /**< Whether this object explodes after a delay when lifted. */
    bool can_regenerate;               /**< Whether this object regenerates once destroyed. */
    int weight;                        /**< Weight of this object (level of "lift" ability required).
                                        * -1 means an object that cannot be lifted. */
    int damage_on_enemies;             /**< Damage this object can cause to enemies. */

    bool is_being_cut;                 /**< Whether this object is being cut */
    uint32_t regeneration_date;        /**< Date when this object starts regenerating. */
    bool is_regenerating;              /**< Whether this object is currently regenerating. */

};

}

#endif

