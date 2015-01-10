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
#ifndef SOLARUS_ENEMY_H
#define SOLARUS_ENEMY_H

#include "solarus/Common.h"
#include "solarus/Treasure.h"
#include "solarus/entities/Detector.h"
#include "solarus/entities/EnemyAttack.h"
#include "solarus/entities/EnemyReaction.h"
#include "solarus/entities/Explosion.h"
#include "solarus/entities/MapEntityPtr.h"

namespace Solarus {

/**
 * \brief Represents an enemy.
 *
 * This class stores the attack and defense properties of the enemy.
 * Every enemy sprite must have at least the following animations
 * and with the four main directions:
 * - "walking": used when the enemy is in its normal state
 * - "hurt": used when it was just hurt
 * If the enemy can be immobilized (i.e. it is sensible to attacks that immobilize it),
 * the following animations are also required:
 * - "immobilized": used while the enemy is immobilized
 * - "shaking": used when the immobilized enemy is about to get back to its normal state.
 * The animation of its sprites automatically switches between those animations
 * depending on its current movement and the attacks it is subject to.
 * Additional animations may be defined.
 */
class Enemy: public Detector {

  public:

    /**
     * \brief Enemy ranks.
     */
    enum class Rank {
      NORMAL,
      MINIBOSS,
      BOSS
    };

    /**
     * \brief Defines the style of sounds and animations played when an enemy
     * is hurt or killed.
     */
    enum class HurtStyle {
      NORMAL,   /**< "enemy_hurt" (and if necessary "enemy_killed") is played */
      MONSTER,  /**< "monster_hurt" (and if necessary "enemy_killed") is played */
      BOSS,     /**< "boss_hurt" or "boss_killed" is played and explosions are created */
    };

    /**
     * \brief Defines how an enemy behaves with obstacles.
     */
    enum class ObstacleBehavior {
      NORMAL,   /**< the enemy is on the ground and stops on normal obstacles */
      FLYING,   /**< the enemy ignores holes, lava, water, etc. */
      SWIMMING  /**< the enemy can move in water */
    };

  public:

    // creation
    Enemy(
        Game& game,
        const std::string& name,
        Layer layer,
        const Point& xy,
        const std::string& breed,
        const Treasure& treasure
    );

    static MapEntityPtr create(
        Game& game,
        const std::string& breed,
        Rank rank,
        const std::string& savegame_variable,
        const std::string& name,
        Layer layer,
        const Point& xy,
        int direction,
        const Treasure& treasure
    );

    virtual EntityType get_type() const override;
    virtual bool is_ground_observer() const override;

    virtual void notify_creating() override;
    virtual void notify_created() override;
    virtual void notify_map_opening_transition_finished() override;
    Rank get_rank() const;

    // Enemy properties.
    const std::string& get_breed() const;
    int get_damage() const;
    void set_damage(int damage_on_hero);
    int get_life() const;
    void set_life(int life);
    HurtStyle get_hurt_style() const;
    void set_hurt_style(HurtStyle hurt_style);
    bool get_can_attack() const;
    void set_can_attack(bool can_attack);
    bool is_traversable() const;
    void set_traversable(bool traversable);
    ObstacleBehavior get_obstacle_behavior() const;
    void set_obstacle_behavior(ObstacleBehavior obstacle_behavior);
    bool get_pushed_back_when_hurt() const;
    void set_pushed_back_when_hurt(bool pushed_back_when_hurt);
    bool get_push_hero_on_sword() const;
    void set_push_hero_on_sword(bool push_hero_on_sword);
    bool get_can_hurt_hero_running() const;
    void set_can_hurt_hero_running(bool can_hurt_hero_running);
    int get_minimum_shield_needed() const;
    void set_minimum_shield_needed(int minimum_shield_needed);
    EnemyReaction::Reaction get_attack_consequence(
        EnemyAttack attack,
        const Sprite* this_sprite) const;
    void set_attack_consequence(
        EnemyAttack attack,
        EnemyReaction::ReactionType reaction,
        int life_lost = 0);
    void set_attack_consequence_sprite(
        const Sprite& sprite,
        EnemyAttack attack,
        EnemyReaction::ReactionType reaction,
        int life_lost = 0);
    void set_no_attack_consequences();
    void set_no_attack_consequences_sprite(const Sprite& sprite);
    void set_default_attack_consequences();
    void set_default_attack_consequences_sprite(const Sprite& sprite);

    // sprites
    const std::string& get_animation() const;
    void set_animation(const std::string& animation);

    // obstacles
    virtual bool is_obstacle_for(MapEntity& other) override;
    virtual bool is_destructible_obstacle(Destructible& destructible) override;
    virtual bool is_block_obstacle(Block& block) override;
    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter) override;
    virtual bool is_raised_block_obstacle(CrystalBlock& raised_block) override;
    virtual bool is_low_wall_obstacle() const override;
    virtual bool is_deep_water_obstacle() const override;
    virtual bool is_shallow_water_obstacle() const override;
    virtual bool is_hole_obstacle() const override;
    virtual bool is_prickle_obstacle() const override;
    virtual bool is_lava_obstacle() const override;

    // enemy state
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;
    virtual void draw_on_map() override;

    virtual void notify_enabled(bool enabled) override;
    virtual void notify_ground_below_changed() override;
    virtual void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) override;
    virtual void notify_collision(MapEntity& other_entity, Sprite& this_sprite, Sprite& other_sprite) override;
    virtual void notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping) override;
    virtual void notify_collision_with_fire(Fire& fire, Sprite& sprite_overlapping) override;
    virtual void notify_collision_with_enemy(Enemy& other, Sprite& other_sprite, Sprite& this_sprite) override;

    // attack the hero
    void attack_hero(Hero& hero, Sprite* this_sprite);
    void attack_stopped_by_hero_shield();

    // receive an attack
    void restart();
    bool is_in_normal_state() const;
    bool is_invulnerable() const;
    bool is_being_hurt() const;
    bool is_immobilized() const;
    bool is_killed() const;
    bool is_dying_animation_finished() const;
    void try_hurt(EnemyAttack attack, MapEntity& source, Sprite* this_sprite);
    void hurt(MapEntity& source, Sprite* this_sprite);
    void kill();
    bool is_dying() const;
    const Treasure& get_treasure() const;
    void set_treasure(const Treasure& treasure);

    static const std::map<EnemyAttack, std::string> attack_names;                   /**< Lua names of the EnemyAttack enum. */
    static const std::map<HurtStyle, std::string> hurt_style_names;                 /**< Lua names of the HurtStyle enum. */
    static const std::map<ObstacleBehavior, std::string> obstacle_behavior_names;   /**< Lua names of the ObstacleBehavior enum. */

  private:

    // hurt the enemy
    void play_hurt_sound();
    bool is_sprite_finished_or_looping() const;
    void immobilize();
    void stop_immobilized();
    void custom_attack(EnemyAttack attack, Sprite* this_sprite);
    void notify_hurt(MapEntity& source, EnemyAttack attack);
    void notify_dead();
    void notify_immobilized();
    bool is_saved() const;

    // enemy characteristics
    std::string breed;                 /**< breed of the enemy (determines its sprites and behavior) */

    int damage_on_hero;                /**< number of heart quarters the player loses when he gets hurt by this enemy;
                                        * this number is divided depending on the hero's tunic number (default: 1) */
    int life;                          /**< number of health points of the enemy (default: 1) */
    HurtStyle hurt_style;              /**< style of sounds and animations when this enemy gets hurt
                                        * (default: HURT_NORMAL) */
    bool pushed_back_when_hurt;        /**< indicates whether the enemy is pushed back when it gets hurt by the hero
                                        * (default: true) */
    bool push_hero_on_sword;           /**< indicates whether the hero is pushed back when he hurts the enemy with his
                                        * sword (default: false) */
    bool can_hurt_hero_running;        /**< indicates that the enemy can attack the hero even when the hero is running */
    int minimum_shield_needed;         /**< shield number needed by the hero to avoid the attack of this enemy,
                                        * or 0 to make the attack unavoidable (default: 0) */
    std::map<EnemyAttack, EnemyReaction>
        attack_reactions;              /**< how the enemy reacts to each attack
                                           * (by default, it depends on the attacks) */
    Rank rank;                         /**< is this enemy a normal enemy, a miniboss or a boss? */
    std::string savegame_variable;     /**< name of the boolean variable indicating whether this enemy is killed,
                                        * or an empty string if it is not saved */
    bool traversable;                  /**< Whether this enemy can be traversed by other entities. */
    ObstacleBehavior obstacle_behavior; /**< behavior with obstacles */

    // enemy state
    bool being_hurt;                   /**< indicates that the enemy is being hurt */
    uint32_t stop_hurt_date;           /**< date when the enemy stops being hurt */
    bool invulnerable;                 /**< indicates that the enemy cannot be hurt for now */
    uint32_t vulnerable_again_date;    /**< date when the enemy can be hurt again */
    bool can_attack;                   /**< indicates that the enemy can currently attack the hero */
    uint32_t can_attack_again_date;    /**< date when the enemy can attack again (0 means never) */
    bool immobilized;                  /**< indicates that the enemy is currently immobilized */
    uint32_t start_shaking_date;       /**< date when the enemy shakes */
    uint32_t end_shaking_date;         /**< date when the enemy stops shaking and walks again */
    bool dying_animation_started;      /**< whether the dying animation was started */

    Treasure treasure;                 /**< pickable item that appears when this enemy gets killed */

    // boss or mini-boss
    bool exploding;                    /**< indicates that the boss is dying and some explosions are triggered on him */
    int nb_explosions;                 /**< number of explosions already played */
    uint32_t next_explosion_date;      /**< date of the next explosion */

};

}

#endif

