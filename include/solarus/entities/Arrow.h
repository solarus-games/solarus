/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/Common.h"
#include "solarus/entities/Entity.h"
#include "solarus/entities/EntityPtr.h"

namespace Solarus {

/**
 * \brief An arrow thrown by the bow on the map.
 */
class Arrow: public Entity {

  public:

    static constexpr EntityType ThisType = EntityType::ARROW;

    explicit Arrow(const Hero& hero);

    virtual EntityType get_type() const override;

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
    virtual bool is_npc_obstacle(Npc& npc) override;
    virtual bool is_jumper_obstacle(Jumper& jumper, const Rectangle& candidate_position) override;

    // state
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;
    void stop();
    bool is_stopped() const;
    bool is_flying() const;
    void attach_to(Entity &entity_reached);

    // collisions
    virtual void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) override;
    virtual void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode) override;
    virtual void notify_collision_with_destructible(Destructible& destructible, CollisionMode collision_mode) override;
    virtual void notify_collision_with_enemy(Enemy& enemy, Sprite& enemy_sprite, Sprite& this_sprite) override;
    virtual void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed
    ) override;
    bool has_reached_map_border();

  private:

    const Hero& hero;          /**< the hero */
    uint32_t disappear_date;   /**< date when the arrow disappears */
    bool stop_now;             /**< true to make the arrow stop now */
    EntityPtr
        entity_reached;        /**< a dynamic entity reached by the arrow
                                * (i.e. an enemy or a crystal, not a usual wall) */

};

}

#endif

