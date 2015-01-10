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
#include "solarus/entities/Enemy.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/MapEntities.h"
#include "solarus/entities/CarriedItem.h"
#include "solarus/entities/Pickable.h"
#include "solarus/entities/Destructible.h"
#include "solarus/entities/CrystalBlock.h"
#include "solarus/entities/Block.h"
#include "solarus/entities/Fire.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Game.h"
#include "solarus/Savegame.h"
#include "solarus/Equipment.h"
#include "solarus/Sprite.h"
#include "solarus/SpriteAnimationSet.h"
#include "solarus/Map.h"
#include "solarus/movements/StraightMovement.h"
#include "solarus/movements/FallingHeight.h"
#include "solarus/lowlevel/Geometry.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Random.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Sound.h"
#include <sstream>

namespace Solarus {

const std::map<EnemyAttack, std::string> Enemy::attack_names = {
    { EnemyAttack::SWORD, "sword" },
    { EnemyAttack::THROWN_ITEM, "thrown_item" },
    { EnemyAttack::EXPLOSION, "explosion" },
    { EnemyAttack::ARROW, "arrow" },
    { EnemyAttack::HOOKSHOT, "hookshot" },
    { EnemyAttack::BOOMERANG, "boomerang" },
    { EnemyAttack::FIRE, "fire" },
    { EnemyAttack::SCRIPT, "script" }
};

const std::map<Enemy::HurtStyle, std::string> Enemy::hurt_style_names = {
    { HurtStyle::NORMAL, "normal" },
    { HurtStyle::MONSTER, "monster" },
    { HurtStyle::BOSS, "boss" }
};

const std::map<Enemy::ObstacleBehavior, std::string> Enemy::obstacle_behavior_names = {
    { ObstacleBehavior::NORMAL, "normal" },
    { ObstacleBehavior::FLYING, "flying" },
    { ObstacleBehavior::SWIMMING, "swimming" }
};

/**
 * \brief Creates an enemy.
 * \param game The game.
 * \param name Name identifying the enemy or an empty string.
 * \param layer The layer on the map.
 * \param xy Coordinates on the map.
 * \param breed Breed of the enemy.
 * \param treasure Treasure dropped when the enemy is killed.
 */
Enemy::Enemy(
    Game& /* game */,
    const std::string& name,
    Layer layer,
    const Point& xy,
    const std::string& breed,
    const Treasure& treasure
):
  Detector(COLLISION_OVERLAPPING | COLLISION_SPRITE,
      name, layer, xy, Size(0, 0)),
  breed(breed),
  damage_on_hero(1),
  life(1),
  hurt_style(HurtStyle::NORMAL),
  pushed_back_when_hurt(true),
  push_hero_on_sword(false),
  can_hurt_hero_running(false),
  minimum_shield_needed(0),
  rank(Rank::NORMAL),
  savegame_variable(),
  traversable(true),
  obstacle_behavior(ObstacleBehavior::NORMAL),
  being_hurt(false),
  stop_hurt_date(0),
  invulnerable(false),
  vulnerable_again_date(0),
  can_attack(true),
  can_attack_again_date(0),
  immobilized(false),
  start_shaking_date(0),
  end_shaking_date(0),
  dying_animation_started(false),
  treasure(treasure),
  exploding(false),
  nb_explosions(0),
  next_explosion_date(0) {

  set_size(16, 16);
  set_origin(8, 13);
  set_drawn_in_y_order(true);
}

/**
 * \brief Creates an enemy.
 *
 * This method acts like a constructor, and usually returns an enemy.
 * However, if the enemy is already dead and cannot be killed again,
 * this function returns:
 * - nullptr if the enemy has no treasure (or its treasure was already picked)
 * - or a pickable treasure if the enemy has one
 *
 * \param game the current game
 * \param breed breed of the enemy to create
 * \param name a name identifying the enemy
 * \param rank rank of the enemy: normal, miniboss or boss
 * \param savegame_variable name of the boolean variable indicating that the enemy is dead
 * \param layer layer of the enemy on the map
 * \param xy Coordinates of the enemy on the map.
 * \param direction initial direction of the enemy on the map (0 to 3)
 * this enemy is killed, or -1 if this enemy is not saved
 * \param treasure the pickable item that the enemy drops
 * \return the enemy created (may also be a Pickable or nullptr)
 */
MapEntityPtr Enemy::create(
    Game& game,
    const std::string& breed,
    Rank rank,
    const std::string& savegame_variable,
    const std::string& name,
    Layer layer,
    const Point& xy,
    int direction,
    const Treasure& treasure
) {
  // see if the enemy is dead
  if (!savegame_variable.empty()
      && game.get_savegame().get_boolean(savegame_variable)) {

    // the enemy is dead: create its pickable treasure (if any) instead
    if (treasure.is_saved() && !game.get_savegame().get_boolean(treasure.get_savegame_variable())) {
      return Pickable::create(game, "", layer, xy, treasure, FALLING_NONE, true);
    }
    return nullptr;
  }

  // create the enemy
  std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(
      game, name, layer, xy, breed, treasure
  );

  // initialize the fields
  enemy->set_direction(direction);
  enemy->rank = rank;
  enemy->savegame_variable = savegame_variable;

  if (rank != Rank::NORMAL) {
    enemy->hurt_style = HurtStyle::BOSS;
  }

  enemy->set_default_attack_consequences();

  return enemy;
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Enemy::get_type() const {
  return EntityType::ENEMY;
}

/**
 * \brief Returns whether this entity is sensible to the ground below it.
 * \return \c true if this entity is sensible to its ground.
 */
bool Enemy::is_ground_observer() const {
  return true;  // To make enemies fall into holes, water, etc.
}

/**
 * \copydoc MapEntity::notify_creating
 */
void Enemy::notify_creating() {

  get_lua_context().run_enemy(*this);
}

/**
 * \copydoc MapEntity::notify_created
 */
void Enemy::notify_created() {

  Detector::notify_created();

  // At this point, enemy:on_created() was called.
  enable_pixel_collisions();

  // Give sprites their initial direction.
  int initial_direction = get_direction();
  for (const SpritePtr& sprite: get_sprites()) {
    sprite->set_current_direction(initial_direction);
  }

  if (is_enabled()) {
    restart();
  }
}

/**
 * \brief Notifies this entity that its map has just become active.
 */
void Enemy::notify_map_opening_transition_finished() {

  Detector::notify_map_opening_transition_finished();

  if (is_enabled() && is_in_normal_state()) {
    restart();
  }
}

/**
 * \brief Returns the breed of this enemy.
 * \return The breed.
 */
const std::string& Enemy::get_breed() const {
  return breed;
}

/**
 * \brief Returns the rank of the enemy.
 * \return the enemy rank
 */
Enemy::Rank Enemy::get_rank() const {
  return rank;
}

/**
 * \brief Returns whether the state of this enemy is saved.
 * \return true if this enemy is saved.
 */
bool Enemy::is_saved() const {
  return !savegame_variable.empty();
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other Another entity.
 * \return \c true if this entity is an obstacle for the other one.
 */
bool Enemy::is_obstacle_for(MapEntity& other) {

  if (!is_enabled()) {
    return false;
  }

  return !is_traversable() || other.is_enemy_obstacle(*this);
}

/**
 * \brief Returns whether a low wall is currently considered as an obstacle
 * by this entity.
 * \return \c true if low walls are currently obstacle for this entity.
 */
bool Enemy::is_low_wall_obstacle() const {

  // Flying enemies can traverse low walls.
  return obstacle_behavior != ObstacleBehavior::FLYING;
}

/**
 * \brief Returns whether a destructible item is currently considered as an obstacle for this entity.
 * \param destructible a destructible item
 * \return true if the destructible item is currently an obstacle this entity
 */
bool Enemy::is_destructible_obstacle(Destructible& destructible) {

  // The destructible object is an obstacle unless the enemy is already
  // overlapping it, which is possible with destructibles that can regenerate.
  if (this->overlaps(destructible)) {
    return false;
  }
  return obstacle_behavior != ObstacleBehavior::FLYING;
}

/**
 * \copydoc MapEntity::is_block_obstacle
 */
bool Enemy::is_block_obstacle(Block& block) {

  // The block is an obstacle unless the enemy is already overlapping it,
  // which is easily possible with blocks created by the hero.
  if (this->overlaps(block)) {
    return false;
  }
  return true;
}

/**
 * \brief Returns whether a teletransporter is currently considered as an obstacle.
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is currently an obstacle for this entity
 */
bool Enemy::is_teletransporter_obstacle(Teletransporter& /* teletransporter */) {
  return false;
}

/**
 * \brief Returns whether a raised crystal block is currently considered as an
 * obstacle by this entity.
 * \param raised_block A crystal block raised.
 * \return \c true if the raised block is currently an obstacle for this entity.
 */
bool Enemy::is_raised_block_obstacle(CrystalBlock& raised_block) {

  // The crystal block is an obstacle unless the enemy is already on it.
  if (this->overlaps(raised_block)) {
    return false;
  }

  return true;
}

/**
 * \brief Returns whether a deep water tile is currently considered as an obstacle by this entity.
 * \return true if the deep water tiles are currently an obstacle for this entity
 */
bool Enemy::is_deep_water_obstacle() const {

  if (obstacle_behavior == ObstacleBehavior::FLYING
      || obstacle_behavior == ObstacleBehavior::SWIMMING) {
    return false;
  }

  if (is_being_hurt()) {
    return false;
  }

  const Layer layer = get_layer();
  const int x = get_top_left_x();
  const int y = get_top_left_y();
  if (get_map().get_ground(layer, x, y) == Ground::DEEP_WATER
      || get_map().get_ground(layer, x + get_width() - 1, y) == Ground::DEEP_WATER
      || get_map().get_ground(layer, x, y + get_height() - 1) == Ground::DEEP_WATER
      || get_map().get_ground(layer, x + get_width() - 1, y + get_height() - 1) == Ground::DEEP_WATER) {
    return false;
  }

  return true;
}

/**
 * \brief Returns whether a shallow water tile is currently considered as an obstacle by this entity.
 * \return true if the shallow water tiles are currently an obstacle for this entity
 */
bool Enemy::is_shallow_water_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a hole is currently considered as an obstacle by this entity.
 * \return true if the holes are currently an obstacle for this entity
 */
bool Enemy::is_hole_obstacle() const {

  if (obstacle_behavior == ObstacleBehavior::FLYING) {
    return false;
  }

  if (is_being_hurt()) {
    return false;
  }

  const Layer layer = get_layer();
  const int x = get_top_left_x();
  const int y = get_top_left_y();
  if (get_map().get_ground(layer, x, y) == Ground::HOLE
      || get_map().get_ground(layer, x + get_width() - 1, y) == Ground::HOLE
      || get_map().get_ground(layer, x, y + get_height() - 1) == Ground::HOLE
      || get_map().get_ground(layer, x + get_width() - 1, y + get_height() - 1) == Ground::HOLE) {
    return false;
  }

  return true;
}

/**
 * \brief Returns whether prickles are currently considered as obstacle by this entity.
 * \return true if prickles are currently obstacle for this entity
 */
bool Enemy::is_prickle_obstacle() const {
  return false;
}

/**
 * \brief Returns whether lava is currently considered as obstacle by this entity.
 * \return true if lava is currently obstacle for this entity
 */
bool Enemy::is_lava_obstacle() const {

  if (obstacle_behavior == ObstacleBehavior::FLYING) {
    return false;
  }

  if (is_being_hurt()) {
    return false;
  }

  const Layer layer = get_layer();
  const int x = get_top_left_x();
  const int y = get_top_left_y();
  if (get_map().get_ground(layer, x, y) == Ground::LAVA
      || get_map().get_ground(layer, x + get_width() - 1, y) == Ground::LAVA
      || get_map().get_ground(layer, x, y + get_height() - 1) == Ground::LAVA
      || get_map().get_ground(layer, x + get_width() - 1, y + get_height() - 1) == Ground::LAVA) {
    return false;
  }

  return true;
}

/**
 * \brief Returns the amount of damage this kind of enemy can make to the hero.
 * \return Number of life points the player loses.
 */
int Enemy::get_damage() const {
  return damage_on_hero;
}

/**
 * \brief Sets the amount of damage this kind of enemy can make to the hero.
 * \param damage_on_hero Number of life points the player loses.
 */
void Enemy::set_damage(int damage_on_hero) {
  this->damage_on_hero = damage_on_hero;
}

/**
 * \brief Returns the number of health points of the enemy.
 * \return number of health points of the enemy
 */
int Enemy::get_life() const {
  return life;
}

/**
 * \brief Sets the number of health points of the enemy.
 * \param life number of health points of the enemy
 */
void Enemy::set_life(int life) {

  this->life = life;
  if (!being_hurt && this->life <= 0) {
    kill();
  }
}

/**
 * \brief Returns the style of sounds and animations when this enemy is hurt.
 * \return The style when hurt.
 */
Enemy::HurtStyle Enemy::get_hurt_style() const {
  return hurt_style;
}

/**
 * \brief Sets the style of sounds and animations when this enemy is hurt.
 * \param hurt_style The style when hurt.
 */
void Enemy::set_hurt_style(HurtStyle hurt_style) {
  this->hurt_style = hurt_style;
}

/**
 * \brief Returns whether this enemy can currently attack the hero.
 * \return true if this enemy can currently attack the hero
 */
bool Enemy::get_can_attack() const {
  return can_attack;
}

/**
 * \brief Sets whether this enemy can attack the hero.
 * \param can_attack true to allow this enemy to attack the hero
 */
void Enemy::set_can_attack(bool can_attack) {

  this->can_attack = can_attack;
  if (!can_attack) {
    can_attack_again_date = 0;
  }
}

/**
 * \brief Returns the current behavior with obstacles for this enemy.
 * \return the behavior with obstacles
 */
Enemy::ObstacleBehavior Enemy::get_obstacle_behavior() const {
  return obstacle_behavior;
}

/**
 * \brief Sets the behavior with obstacles for this enemy.
 * \param obstacle_behavior the behavior with obstacles
 */
void Enemy::set_obstacle_behavior(ObstacleBehavior obstacle_behavior) {
  this->obstacle_behavior = obstacle_behavior;
}

/**
 * \brief Returns whether this enemy is traversable.by other entities.
 *
 * If the enemy is not traversable, is_obstacle_for() will always return
 * \c false.
 * If the enemy is traversable, is_obstacle_for() will call
 * is_enemy_obstacle() on the entity to test.
 *
 * \return \c true if this enemy is traversable.
 */
bool Enemy::is_traversable() const {
  return traversable;
}

/**
 * \brief Sets whether this enemy is traversable.by other entities.
 *
 * If the enemy is not traversable, is_obstacle_for() will always return
 * \c false.
 * If the enemy is traversable, is_obstacle_for() will call
 * is_enemy_obstacle() on the entity to test.
 *
 * \param traversable \c true to make this enemy traversable.
 */
void Enemy::set_traversable(bool traversable) {
  this->traversable = traversable;
}

/**
 * \brief Returns whether the enemy is pushed back when it gets hurt by the hero.
 * \return \c true if the enemy is pushed back when it gets hurt.
 */
bool Enemy::get_pushed_back_when_hurt() const {
  return pushed_back_when_hurt;
}

/**
 * \brief Sets whether the enemy is pushed back when it gets hurt by the hero.
 * \param pushed_back_when_hurt true to make the enemy pushed back when it gets hurt.
 */
void Enemy::set_pushed_back_when_hurt(bool pushed_back_when_hurt) {
  this->pushed_back_when_hurt = pushed_back_when_hurt;
}

/**
 * \brief Returns whether the hero is pushed when he strikes the enemy with his sword.
 * \return true if the hero is pushed away when he strkes the enemy with his sword
 */
bool Enemy::get_push_hero_on_sword() const {
  return push_hero_on_sword;
}

/**
 * \brief Sets whether the hero is pushed when he hurts the enemy with his sword.
 * \param push_hero_on_sword true to make the hero pushed back when he hurts the enemy with his sword
 */
void Enemy::set_push_hero_on_sword(bool push_hero_on_sword) {
  this->push_hero_on_sword = push_hero_on_sword;
}

/**
 * \brief Returns whether this enemy can hurt the hero even if the hero is running.
 * \return true if this enemy can attack the hero while he is running
 */
bool Enemy::get_can_hurt_hero_running() const {
  return can_hurt_hero_running;
}

/**
 * \brief Sets whether this enemy can hurt the hero even if the hero is running.
 * \param can_hurt_hero_running true to allow this enemy to attack the hero while he is running
 */
void Enemy::set_can_hurt_hero_running(bool can_hurt_hero_running) {
  this->can_hurt_hero_running = can_hurt_hero_running;
}

/**
 * \brief Returns the shield level required to avoid attacks of this enemy.
 * \return The minimum shield needed (\c 0 means unavoidable attacks).
 */
int Enemy::get_minimum_shield_needed() const {
  return minimum_shield_needed;
}

/**
 * \brief Sets the shield level required to avoid attacks of this enemy.
 * \param minimum_shield_needed The minimum shield needed (\c 0 means unavoidable attacks).
 */
void Enemy::set_minimum_shield_needed(int minimum_shield_needed) {
  this->minimum_shield_needed = minimum_shield_needed;
}

/**
 * \brief Returns the reaction corresponding to an attack on a sprite of this enemy.
 * \param attack an attack this enemy receives
 * \param this_sprite the sprite attacked, or nullptr if the attack does not come from
 * a pixel-precise collision test
 * \return the corresponding reaction
 */
EnemyReaction::Reaction Enemy::get_attack_consequence(
    EnemyAttack attack,
    const Sprite* this_sprite) const {

  const auto& it = attack_reactions.find(attack);
  if (it == attack_reactions.end()) {
    // Attack consequence was not initialized. Return a default value.
    return EnemyReaction::Reaction();
  }
  return it->second.get_reaction(this_sprite);
}

/**
 * \brief Sets how the enemy reacts to an attack.
 *
 * This reaction will be used unless the attack is received by a sprite
 * that has a specific reaction (see set_attack_consequence_sprite()).
 *
 * \param attack an attack
 * \param reaction how the enemy will react
 * \param life_lost number of life points lost with this attack (possibly zero)
 */
void Enemy::set_attack_consequence(
    EnemyAttack attack,
    EnemyReaction::ReactionType reaction,
    int life_lost) {

  if (life_lost < 0) {
    std::ostringstream oss;
    oss << "Invalid amount of life: " << life_lost;
    Debug::die(oss.str());
  }
  attack_reactions[attack].set_general_reaction(reaction, life_lost);
}

/**
 * \brief Sets how the enemy reacts to an attack on a particular sprite.
 * \param sprite a sprite of this enemy
 * \param attack an attack
 * \param reaction how the enemy will react
 * \param life_lost number of life points lost with this attack (possibly zero)
 */
void Enemy::set_attack_consequence_sprite(
    const Sprite& sprite,
    EnemyAttack attack,
    EnemyReaction::ReactionType reaction,
    int life_lost) {

  if (life_lost < 0) {
    std::ostringstream oss;
    oss << "Invalid amount of life: " << life_lost;
    Debug::die(oss.str());
  }
  attack_reactions[attack].set_sprite_reaction(&sprite, reaction, life_lost);
}

/**
 * \brief Sets the enemy insensible to all attacks.
 */
void Enemy::set_no_attack_consequences() {

  for (const auto& kvp : attack_names) {
    set_attack_consequence(kvp.first, EnemyReaction::ReactionType::IGNORED);
  }
}

/**
 * \brief Sets a particular sprite of the enemy insensible to all attacks.
 * \param sprite a sprite of this enemy
 */
void Enemy::set_no_attack_consequences_sprite(const Sprite& sprite) {

  for (const auto& kvp : attack_names) {
    EnemyAttack attack = kvp.first;
    set_attack_consequence_sprite(sprite, attack, EnemyReaction::ReactionType::IGNORED);
  }
}

/**
 * \brief Sets some default values for the reactions of the attacks.
 */
void Enemy::set_default_attack_consequences() {

  for (const auto& kvp : attack_names) {
    EnemyAttack attack = kvp.first;
    attack_reactions[attack].set_default_reaction();
  }
  set_attack_consequence(EnemyAttack::SWORD, EnemyReaction::ReactionType::HURT, 1); // multiplied by the sword strength
  set_attack_consequence(EnemyAttack::THROWN_ITEM, EnemyReaction::ReactionType::HURT, 1); // multiplied depending on the item
  set_attack_consequence(EnemyAttack::EXPLOSION, EnemyReaction::ReactionType::HURT, 2);
  set_attack_consequence(EnemyAttack::ARROW, EnemyReaction::ReactionType::HURT, 2);
  set_attack_consequence(EnemyAttack::HOOKSHOT, EnemyReaction::ReactionType::IMMOBILIZED);
  set_attack_consequence(EnemyAttack::BOOMERANG, EnemyReaction::ReactionType::IMMOBILIZED);
  set_attack_consequence(EnemyAttack::FIRE, EnemyReaction::ReactionType::HURT, 3);
}

/**
 * \brief Set some default values for the reactions of the attacks
 * on a particular sprite of this enemy.
 * \param sprite a sprite of this enemy
 */
void Enemy::set_default_attack_consequences_sprite(const Sprite& sprite) {

  for (const auto& kvp : attack_names) {
    EnemyAttack attack = kvp.first;
    set_attack_consequence_sprite(sprite, attack, EnemyReaction::ReactionType::HURT, 1);
  }
  set_attack_consequence_sprite(sprite, EnemyAttack::EXPLOSION, EnemyReaction::ReactionType::HURT, 2);
  set_attack_consequence_sprite(sprite, EnemyAttack::HOOKSHOT, EnemyReaction::ReactionType::IMMOBILIZED);
  set_attack_consequence_sprite(sprite, EnemyAttack::BOOMERANG, EnemyReaction::ReactionType::IMMOBILIZED);
}

/**
 * \brief Returns the current animation of the first sprite of the enemy.
 *
 * This function is useful when the enemy has several sprites.
 *
 * \return name of the current animation of the first sprite
 */
const std::string& Enemy::get_animation() const {

  return get_sprite().get_current_animation();
}

/**
 * \brief Changes the animation of this enemy's sprites.
 *
 * This function is useful when the enemy has several sprites.
 *
 * \param animation name of the animation to set
 */
void Enemy::set_animation(const std::string& animation) {

  for (const SpritePtr& sprite: get_sprites()) {
    sprite->set_current_animation(animation);
  }
}

/**
 * \brief Updates the enemy.
 */
void Enemy::update() {

  Detector::update();

  if (is_suspended() || !is_enabled()) {
    return;
  }

  uint32_t now = System::now();

  if (being_hurt) {

    // see if we should stop the animation "hurt"
    if (now >= stop_hurt_date) {
      being_hurt = false;
      set_movement_events_enabled(true);

      if (life <= 0) {
        kill();
      }
      else if (is_immobilized()) {
        clear_movement();
        set_animation("immobilized");
        notify_immobilized();
      }
      else {
        clear_movement();
        restart();
      }
    }
  }

  if (life > 0 && invulnerable && now >= vulnerable_again_date && !being_hurt) {
    invulnerable = false;
  }

  if (life > 0 && !can_attack && !is_immobilized()
      && can_attack_again_date != 0 && now >= can_attack_again_date) {
    can_attack = true;
  }

  if (is_immobilized() && !is_killed() && now >= end_shaking_date &&
      get_sprite().get_current_animation() == "shaking") {

    restart();
  }

  if (is_immobilized() && !is_killed() && !is_being_hurt() && now >= start_shaking_date &&
      get_sprite().get_current_animation() != "shaking") {

    end_shaking_date = now + 2000;
    set_animation("shaking");
  }

  if (exploding) {
    uint32_t now = System::now();
    if (now >= next_explosion_date) {

      // create an explosion
      Point xy;
      xy.x = get_top_left_x() + Random::get_number(get_width());
      xy.y = get_top_left_y() + Random::get_number(get_height());
      get_entities().add_entity(std::make_shared<Explosion>(
          "", LAYER_HIGH, xy, false
      ));
      Sound::play("explosion");

      next_explosion_date = now + 200;
      nb_explosions++;

      if (nb_explosions >= 15) {
        exploding = false;
      }
    }
  }

  if (is_killed() && is_dying_animation_finished()) {

    // Create the pickable treasure if any.
    get_entities().add_entity(Pickable::create(
        get_game(),
        "",
        get_layer(),
        get_xy(),
        treasure,
        FALLING_HIGH,
        false
    ));

    // Remove the enemy.
    remove_from_map();

    // Notify Lua that this enemy is dead.
    // We need to do this after remove_from_map() so that this enemy is
    // considered dead in functions like map:has_entities(prefix).
    notify_dead();
  }

  get_lua_context().entity_on_update(*this);
}

/**
 * \brief Suspends or resumes the entity.
 * \param suspended true to suspend the entity, false to resume it
 */
void Enemy::set_suspended(bool suspended) {

  Detector::set_suspended(suspended);

  if (!suspended) {
    uint32_t diff = System::now() - get_when_suspended();
    stop_hurt_date += diff;
    vulnerable_again_date += diff;
    if (can_attack_again_date != 0) {
      can_attack_again_date += diff;
    }
    start_shaking_date += diff;
    end_shaking_date += diff;
    next_explosion_date += diff;
  }
  get_lua_context().entity_on_suspended(*this, suspended);
}

/**
 * \brief Draws the entity on the map.
 */
void Enemy::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  get_lua_context().entity_on_pre_draw(*this);
  Detector::draw_on_map();
  get_lua_context().entity_on_post_draw(*this);
}

/**
 * \brief Notifies this entity that it was just enabled or disabled.
 * \param enabled true if the entity is now enabled
 */
void Enemy::notify_enabled(bool enabled) {

  Detector::notify_enabled(enabled);

  if (!is_on_map()) {
    return;
  }

  if (enabled) {
    restart();
    get_lua_context().entity_on_enabled(*this);
  }
  else {
    get_lua_context().entity_on_disabled(*this);
  }
}

/**
 * \copydoc MapEntity::notify_ground_below_changed
 */
void Enemy::notify_ground_below_changed() {

  if (get_obstacle_behavior() != ObstacleBehavior::NORMAL
      || get_life() <= 0) {
    return;
  }

  Ground ground = get_ground_below();
  switch (ground) {

    case Ground::HOLE:
    case Ground::LAVA:
    case Ground::DEEP_WATER:
      // Kill the enemy.
      set_life(0);
      break;

    default:
      break;
  }
}

/**
 * \brief Notifies the enemy that a collision was just detected with another entity
 * \param entity_overlapping the other entity
 * \param collision_mode the collision mode that detected the collision
 */
void Enemy::notify_collision(MapEntity& entity_overlapping, CollisionMode /* collision_mode */) {

  entity_overlapping.notify_collision_with_enemy(*this);
}

/**
 * \copydoc Detector::notify_collision(MapEntity&, Sprite&, Sprite&)
 */
void Enemy::notify_collision(MapEntity& other_entity, Sprite& this_sprite, Sprite& other_sprite) {

  other_entity.notify_collision_with_enemy(*this, this_sprite, other_sprite);
}

/**
 * \brief This function is called when an explosion's sprite detects a collision
 * with a sprite of this enemy.
 * \param explosion the explosion
 * \param sprite_overlapping the sprite of this enemy that collides with the explosion
 */
void Enemy::notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping) {

  explosion.try_attack_enemy(*this, sprite_overlapping);
}

/**
 * \brief Notifies this entity that a sprite of fire
 * detects a pixel-precise collision with a sprite of this entity.
 * \param fire the fire
 * \param sprite_overlapping the sprite of the current entity that collides with the fire
 */
void Enemy::notify_collision_with_fire(Fire& fire, Sprite& sprite_overlapping) {

  try_hurt(EnemyAttack::FIRE, fire, &sprite_overlapping);
}

/**
 * \brief This function is called when this enemy detects a collision with another enemy.
 * \param other the other enemy
 * \param other_sprite the other enemy's sprite that overlaps a sprite of this enemy
 * \param this_sprite this enemy's sprite that overlaps the other
 */
void Enemy::notify_collision_with_enemy(Enemy& other,
    Sprite& other_sprite, Sprite& this_sprite) {

  if (is_in_normal_state()) {
    get_lua_context().enemy_on_collision_enemy(
        *this, other, other_sprite, this_sprite);
  }
}

/**
 * \brief Attacks the hero if possible.
 *
 * This function is called when there is a collision between the enemy and the hero.
 *
 * \param hero The hero to attack.
 * \param this_sprite The sprite of this enemy that detected the collision with the hero,
 * or nullptr if it was not a pixel-precise collision.
 */
void Enemy::attack_hero(Hero& hero, Sprite* this_sprite) {

  if (!is_immobilized()
      && can_attack
      && hero.can_be_hurt(this)) {

    bool hero_protected = false;
    if (minimum_shield_needed != 0
        && get_equipment().has_ability(Ability::SHIELD, minimum_shield_needed)
        && hero.can_use_shield()) {

      // Compute the direction corresponding to the angle between the enemy and the hero.
      double angle = hero.get_angle(*this, nullptr, this_sprite);
      int protected_direction4 = (int) ((angle + Geometry::PI_OVER_2 / 2.0) * 4 / Geometry::TWO_PI);
      protected_direction4 = (protected_direction4 + 4) % 4;

      // Also get the direction of the enemy's sprite.
      int sprite_opposite_direction4 = -1;
      if (this_sprite != nullptr) {
        sprite_opposite_direction4 = (this_sprite->get_current_direction() + 2) % 4;
      }

      // The hero is protected if he is facing the opposite of one of these directions.
      hero_protected = hero.is_facing_direction4(protected_direction4) ||
          hero.is_facing_direction4(sprite_opposite_direction4);
    }

    if (hero_protected) {
      attack_stopped_by_hero_shield();
    }
    else {
      // Let the enemy script handle this if it wants.
      const bool handled = get_lua_context().enemy_on_attacking_hero(
          *this, hero, this_sprite
      );
      if (!handled) {
        // Scripts did not customize the attack:
        // do the built-in hurt state of the hero.
        hero.hurt(*this, this_sprite, damage_on_hero);
      }
    }
  }
}

/**
 * \brief This function is called when the hero stops an attack with his shield.
 *
 * By default, the shield sound is played and the enemy cannot attack again for a while.
 */
void Enemy::attack_stopped_by_hero_shield() {

  Sound::play("shield");

  uint32_t now = System::now();
  can_attack = false;
  can_attack_again_date = now + 1000;

  get_equipment().notify_ability_used(Ability::SHIELD);
}

/**
 * \brief Plays the appropriate sound when the enemy is hurt.
 */
void Enemy::play_hurt_sound() {

  std::string sound_id = "";
  switch (hurt_style) {

    case HurtStyle::NORMAL:
      sound_id = "enemy_hurt";
      break;

    case HurtStyle::MONSTER:
      sound_id = "monster_hurt";
      break;

    case HurtStyle::BOSS:
      sound_id = (life > 0) ? "boss_hurt" : "boss_killed";
      break;

  }

  Sound::play(sound_id);
}

/**
 * \brief Notifies this enemy that it should restart his movement.
 *
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 * By default, the "walking" animation is set on the enemy's sprites.
 *
 * Nothing happens if the enemy is dying.
 */
void Enemy::restart() {

  if (is_dying()) {
    return;
  }

  if (is_immobilized()) {
    stop_immobilized();
  }
  set_animation("walking");
  get_lua_context().enemy_on_restarted(*this);
}

/**
 * \brief Returns whether this enemy is in a normal state.
 *
 * The enemy is considered to be in its normal state if
 * it is not disabled, dying, being hurt or immobilized.
 * When this method returns false, the subclasses of Enemy
 * should not change the enemy properties.
 *
 * \return true if this enemy is in a normal state
 */
bool Enemy::is_in_normal_state() const {
  return is_enabled()
    && !is_being_hurt()
    && get_life() > 0
    && !is_immobilized()
    && !is_being_removed();
}

/**
 * \brief Returns whether this enemy is currently invulnerable.
 * \return \c true if this enemy cannot be hurt for now.
 */
bool Enemy::is_invulnerable() const {
  return invulnerable;
}

/**
 * \brief Makes the enemy receive an attack.
 *
 * He might resist to the attack or get hurt.
 *
 * \param attack type of attack
 * \param source the entity attacking the enemy (often the hero)
 * \param this_sprite the sprite of this enemy that received the attack, or nullptr
 * if the attack comes from a non pixel-precise collision test
 */
void Enemy::try_hurt(EnemyAttack attack, MapEntity& source, Sprite* this_sprite) {

  EnemyReaction::Reaction reaction = get_attack_consequence(attack, this_sprite);
  if (invulnerable || reaction.type == EnemyReaction::ReactionType::IGNORED) {
    // ignore the attack
    return;
  }

  invulnerable = true;
  vulnerable_again_date = System::now() + 500;

  switch (reaction.type) {

    case EnemyReaction::ReactionType::PROTECTED:
      // attack failure sound
      Sound::play("sword_tapping");
      break;

    case EnemyReaction::ReactionType::IMMOBILIZED:
      // get immobilized
      being_hurt = true;
      hurt(source, this_sprite);
      immobilize();
      break;

    case EnemyReaction::ReactionType::CUSTOM:
      // custom attack (defined in the script)
      if (is_in_normal_state()) {
        custom_attack(attack, this_sprite);
      }
      else {
        // no attack was made: notify the source correctly
        reaction.type = EnemyReaction::ReactionType::IGNORED;
        invulnerable = false;
      }
      break;

    case EnemyReaction::ReactionType::HURT:

      if (is_immobilized() && get_sprite().get_current_animation() == "shaking") {
        stop_immobilized();
      }

      // Compute the number of health points lost by the enemy.

      being_hurt = true;
      if (attack == EnemyAttack::SWORD) {

        Hero& hero = static_cast<Hero&>(source);

        // Sword attacks only use pixel-precise collisions.
        Debug::check_assertion(this_sprite != nullptr,
            "Missing enemy sprite for sword attack"
        );

        // For a sword attack, the damage may be something customized.
        bool customized = get_lua_context().enemy_on_hurt_by_sword(
            *this, hero, *this_sprite);

        if (customized) {
          reaction.life_lost = 0;  // Already done by the script.
        }
        else {
          // If this is not customized, the default it to multiply the reaction
          // by a factor that depends on the sword.
          reaction.life_lost *= hero.get_sword_damage_factor();
        }
      }
      else if (attack == EnemyAttack::THROWN_ITEM) {
        reaction.life_lost *= static_cast<CarriedItem&>(source).get_damage_on_enemies();
      }
      life -= reaction.life_lost;

      hurt(source, this_sprite);
      notify_hurt(source, attack);
      break;

    case EnemyReaction::ReactionType::IGNORED:
      return;
  }

  // notify the source
  source.notify_attacked_enemy(
      attack,
      *this,
      this_sprite,
      reaction,
      get_life() <= 0
  );
}

/**
 * \brief Hurts the enemy.
 *
 * Updates its state, its sprite and plays the sound.
 *
 * \param source The entity attacking the enemy (often the hero).
 * \param this_sprite The sprite of this enemy that received the attack, or nullptr
 * if the attack comes from a non pixel-precise collision test.
 */
void Enemy::hurt(MapEntity& source, Sprite* this_sprite) {

  uint32_t now = System::now();

  // update the enemy state
  set_movement_events_enabled(false);

  can_attack = false;
  can_attack_again_date = now + 300;

  // graphics and sounds
  set_animation("hurt");
  play_hurt_sound();

  // stop any movement
  clear_movement();

  // push the enemy back
  if (pushed_back_when_hurt) {
    double angle = source.get_angle(*this, nullptr, this_sprite);
    std::shared_ptr<StraightMovement> movement =
        std::make_shared<StraightMovement>(false, true);
    movement->set_max_distance(24);
    movement->set_speed(120);
    movement->set_angle(angle);
    set_movement(movement);
  }

  stop_hurt_date = now + 300;
}

/**
 * \brief This function is called when the enemy has just been hurt.
 * \param source The source of the attack.
 * \param attack The attack that was just successful.
 */
void Enemy::notify_hurt(MapEntity& /* source */, EnemyAttack attack) {

  get_lua_context().enemy_on_hurt(*this, attack);
  if (get_life() <= 0) {
    get_lua_context().enemy_on_dying(*this);
  }
}

/**
 * \brief This function is called when the enemy has just finished dying.
 */
void Enemy::notify_dead() {

  get_lua_context().enemy_on_dead(*this);
}

/**
 * \brief This function is called when the enemy is immobilized,
 * after the hurt animation.
 */
void Enemy::notify_immobilized() {

  get_lua_context().enemy_on_immobilized(*this);
}

/**
 * \brief Kills the enemy.
 *
 * This function is called when the enemy has no more health points.
 */
void Enemy::kill() {

  // stop any movement and disable attacks
  set_collision_modes(COLLISION_NONE);
  clear_movement();
  invulnerable = true;
  can_attack = false;
  can_attack_again_date = 0;
  dying_animation_started = true;

  if (hurt_style == HurtStyle::BOSS) {
    // A boss: create some explosions.
    exploding = true;
    nb_explosions = 0;
    next_explosion_date = System::now() + 2000;
  }
  else {
    // Replace the enemy sprites.
    clear_sprites();
    switch (get_ground_below()) {

      case Ground::HOLE:
        // TODO animation of falling into a hole.
        Sound::play("jump");
        break;

      case Ground::DEEP_WATER:
        // TODO water animation.
        Sound::play("splash");
        break;

      case Ground::LAVA:
        // TODO lava animation.
        Sound::play("splash");
        break;

      default:
        create_sprite("enemies/enemy_killed");
        Sound::play("enemy_killed");
        break;
    }
  }

  // save the enemy state if required
  if (is_saved()) {
    get_savegame().set_boolean(savegame_variable, true);
  }
}

/**
 * \brief Returns whether the enemy is being hurt.
 * \return true if the enemy is being hurt
 */
bool Enemy::is_being_hurt() const {
  return being_hurt;
}

/**
 * \brief Returns whether the enemy is killed.
 *
 * An enemy is considered as killed if he has no more life and its dying
 * animation has started.
 *
 * \return true if the enemy is killed
 */
bool Enemy::is_killed() const {
  return life <= 0 && dying_animation_started;
}

/**
 * \brief When the enemy is killed, returns whether the dying animation is finished.
 * \return true if the dying animation is finished
 */
bool Enemy::is_dying_animation_finished() const {

  if (!is_dying()) {
    // The enemy is still alive.
    return false;
  }

  if (nb_explosions > 0) {
    // The dying animation is some explosions.
    return !exploding;
  }

  if (has_sprite()) {
    // The dying animation is the usual "enemy_killed" sprite.
    return get_sprite().is_animation_finished();
  }

  // There is no dying animation (case of holes, water and lava for now).
  return true;
}

/**
 * \brief Returns whether the enemy is dying, i.e. his life has reached zero and the dying animation is playing.
 * \return true if the enemy is dying
 */
bool Enemy::is_dying() const {

  return get_life() <= 0;
}

/**
 * \brief Returns the treasure dropped by this enemy.
 * \return The treasure.
 */
const Treasure& Enemy::get_treasure() const {
  return treasure;
}

/**
 * \brief Sets the treasure dropped by this enemy.
 * \param treasure The treasure to set.
 */
void Enemy::set_treasure(const Treasure& treasure) {
  this->treasure = treasure;
}

/**
 * \brief Returns true if the current sprite animation is finished or is looping.
 * \return true if the current sprite animation is finished or is looping
 */
bool Enemy::is_sprite_finished_or_looping() const {

  const Sprite& sprite = get_sprite();
  return sprite.is_animation_finished() || sprite.is_animation_looping();
}

/**
 * \brief Immobilizes this enemy.
 */
void Enemy::immobilize() {

  immobilized = true;
  start_shaking_date = System::now() + 5000;
}

/**
 * \brief Stops immobilizing the enemy.
 */
void Enemy::stop_immobilized() {

  immobilized = false;
  end_shaking_date = 0;
}

/**
 * \brief Returns whether this enemy is immobilized.
 * \return true if this enemy is immobilized
 */
bool Enemy::is_immobilized() const {

  return immobilized;
}

/**
 * \brief This function is called when the enemy is attacked by a custom effect attack.
 *
 * Redefine this function to handle the attack.
 *
 * \param attack the attack
 * \param this_sprite the sprite of this enemy subject to the attack, or nullptr
 * if the attack does not come from a pixel-precise collision test
 */
void Enemy::custom_attack(EnemyAttack attack, Sprite* this_sprite) {

  get_lua_context().enemy_on_custom_attack_received(*this, attack, this_sprite);
}

}

