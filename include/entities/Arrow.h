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
#ifndef SOLARUS_ARROW_H
#define SOLARUS_ARROW_H

#include "Common.h"
#include "entities/MapEntity.h"

namespace solarus {

/**
 * \brief An arrow thrown by the bow on the map.
 */
class Arrow: public MapEntity {

  private:

    const Hero& hero;          /**< the hero */
    uint32_t disappear_date;   /**< date when the arrow disappears */
    bool stop_now;             /**< true to make the arrow stop now */
    MapEntity* entity_reached; /**< a dynamic entity reached by the arrow
                                * (i.e. an enemy or a crystal, not a usual wall) */

  public:

    Arrow(const Hero& hero);
    ~Arrow();

    EntityType get_type() const;
    bool can_be_obstacle() const;
    bool is_drawn_in_y_order() const;

    bool is_teletransporter_obstacle(const Teletransporter& teletransporter) const;
    bool is_conveyor_belt_obstacle(const ConveyorBelt& conveyor_belt) const;
    bool is_stairs_obstacle(const Stairs& stairs) const;
    bool is_low_wall_obstacle() const;
    bool is_deep_water_obstacle() const;
    bool is_hole_obstacle() const;
    bool is_lava_obstacle() const;
    bool is_prickle_obstacle() const;
    bool is_ladder_obstacle() const;
    bool is_switch_obstacle(const Switch& sw) const;
    bool is_raised_block_obstacle(const CrystalBlock& raised_block) const;
    bool is_crystal_obstacle(const Crystal& crystal) const;
    bool is_npc_obstacle(const Npc& npc) const;
    bool is_jumper_obstacle(const Jumper& jumper) const;

    // state
    void update();
    void set_suspended(bool suspended);
    void stop();
    bool is_stopped() const;
    bool is_flying() const;
    void attach_to(MapEntity &entity_reached);
    const Rectangle get_facing_point() const;

    // collisions
    void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode);
    void notify_collision_with_destructible(Destructible& destructible, CollisionMode collision_mode);
    void notify_collision_with_enemy(Enemy& enemy, Sprite& enemy_sprite, Sprite& this_sprite);
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
        EnemyReaction::Reaction& result, bool killed);
    bool has_reached_map_border() const;
};

}

#endif

