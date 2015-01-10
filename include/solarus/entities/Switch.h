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
#ifndef SOLARUS_SWITCH_H
#define SOLARUS_SWITCH_H

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"

namespace Solarus {

class Arrow;

/**
 * \brief A button that the hero can trigger.
 *
 * A switch can be triggered by walking onto it
 * or by using weapons on it, depending on its subtype
 * Some walkable switches require a block to be activated.
 * Some walkable switches become disabled when the hero or the block leave it.
 * A switch can have a sprite or be invisible.
 */
class Switch: public Detector {

  public:

    /**
     * Subtypes of switches.
     */
    enum class Subtype {
      WALKABLE,         /**< A button the hero can walk on. */
      ARROW_TARGET,     /**< A switch that can be triggered by shooting an
                         * arrow on it. */
      SOLID             /**< A non-traversable switch that can be triggered
                         * with the sword or a projectile. */
    };

    Switch(
        const std::string& name,
        Layer layer,
        const Point& xy,
        Subtype subtype,
        const std::string& sprite_name,
        const std::string& sound_id,
        bool needs_block,
        bool inactivate_when_leaving
    );

    virtual EntityType get_type() const override;
    virtual bool is_obstacle_for(MapEntity& other) override;

    bool is_walkable() const;
    bool is_arrow_target() const;
    bool is_solid() const;
    bool is_activated() const;
    void activate();
    void set_activated(bool enabled);
    void set_locked(bool locked);

    void try_activate(Hero& hero);
    void try_activate(Block& block);
    void try_activate(Arrow& arrow);
    void try_activate();

    virtual void update() override;
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

    static const std::map<Subtype, std::string> subtype_names;  /**< Lua names of the Subtype enum. */

  private:

    Subtype subtype;                           /**< Subtype of switch. */
    const std::string sound_id;                /**< Sound to play when activating this switch. */
    bool activated;                            /**< Whether this switch is currently on. */
    bool locked;                               /**< Whether this switch cannot be activated or inactivated by other entities for now. */

    // the following fields are used only for walkable switches
    bool needs_block;                          /**< Whether a block or a statue is required to activate this walkable switch. */
    bool inactivate_when_leaving;              /**< Whether this walkable switch becomes inactivated when the hero or the block leaves it. */
    MapEntity* entity_overlapping;             /**< The entity currently on this walkable switch (as arrows may be destroyed at any moment). */
    bool entity_overlapping_still_present;     /**< To detect when the entity overlapping leaves the switch. */

};

}

#endif

