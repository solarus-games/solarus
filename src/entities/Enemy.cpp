/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/Enemy.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "entities/CarriedItem.h"
#include "entities/Pickable.h"
#include "entities/Destructible.h"
#include "entities/Fire.h"
#include "lua/LuaContext.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "Map.h"
#include "movements/StraightMovement.h"
#include "movements/FallingHeight.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Random.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"

/**
 * @brief Creates an enemy.
 * @param game The game.
 * @param name Id of the enemy.
 * @param layer The layer on the map.
 * @param x X position on the map.
 * @param y Y position on the map.
 * @param breed Breed of the enemy.
 * @param treasure Treasure dropped when the enemy is killed.
 */
Enemy::Enemy(Game& game, const std::string& name, Layer layer, int x, int y,
    const std::string& breed, const Treasure& treasure):

  Detector(COLLISION_RECTANGLE | COLLISION_SPRITE,
      name, layer, x, y, 0, 0),

  breed(breed),
  damage_on_hero(1),
  magic_damage_on_hero(0),
  life(1),
  hurt_style(HURT_NORMAL),
  pushed_back_when_hurt(true),
  push_hero_on_sword(false),
  can_hurt_hero_running(false),
  minimum_shield_needed(0),
  rank(RANK_NORMAL),
  savegame_variable(),
  obstacle_behavior(OBSTACLE_BEHAVIOR_NORMAL),
  drawn_in_y_order(true),
  initialized(false),
  being_hurt(false),
  stop_hurt_date(0),
  invulnerable(false),
  vulnerable_again_date(0),
  can_attack(true),
  can_attack_again_date(0),
  immobilized(false),
  start_shaking_date(0),
  end_shaking_date(0),
  treasure(treasure),
  exploding(false),
  nb_explosions(0),
  next_explosion_date(0) {

}

/**
 * @brief Destructor.
 */
Enemy::~Enemy() {

}

/**
 * @brief Creates an enemy.
 *
 * This method acts like a constructor, and usually returns an enemy.
 * However, if the enemy is already dead and cannot be killed again,
 * this function returns:
 * - NULL if the enemy has no treasure (or its treasure was already picked)
 * - or a pickable treasure if the enemy has one
 *
 * @param game the current game
 * @param breed breed of the enemy to create
 * @param name a name identifying the enemy
 * @param rank rank of the enemy: normal, miniboss or boss
 * @param savegame_variable name of the boolean variable indicating that the enemy is dead
 * @param layer layer of the enemy on the map
 * @param x x position of the enemy on the map
 * @param y y position of the enemy on the map
 * @param direction initial direction of the enemy on the map (0 to 3)
 * this enemy is killed, or -1 if this enemy is not saved
 * @param treasure the pickable item that the enemy drops
 * @return the enemy created (may also be a Pickable or NULL)
 */
MapEntity* Enemy::create(Game &game, const std::string& breed, Rank rank,
    const std::string& savegame_variable, const std::string &name, Layer layer, int x, int y,
    int direction, const Treasure& treasure) {

  // see if the enemy is dead
  if (!savegame_variable.empty()
      && game.get_savegame().get_boolean(savegame_variable)) {

    // the enemy is dead: create its pickable treasure (if any) instead
    if (treasure.is_saved() && !game.get_savegame().get_boolean(treasure.get_savegame_variable())) {
      return Pickable::create(game, "", layer, x, y, treasure, FALLING_NONE, true);
    }
    return NULL;
  }

  // create the enemy
  Enemy *enemy = new Enemy(game, name, layer, x, y, breed, treasure);

  // initialize the fields
  enemy->set_direction(direction);
  enemy->rank = rank;
  enemy->savegame_variable = savegame_variable;

  if (rank != RANK_NORMAL) {
    enemy->set_enabled(false);
    enemy->hurt_style = HURT_BOSS;
  }

  enemy->set_default_attack_consequences();

  return enemy;
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Enemy::get_type() {
  return ENEMY;
}

/**
 * @brief Initializes the enemy.
 */
void Enemy::initialize() {

  if (!initialized) {
    initialized = true;
    get_lua_context().run_enemy(*this);
  }
}

/**
 * @brief Sets the map.
 *
 * Warning: when this function is called during the map initialization,
 * the current map of the game is still the old one.
 *
 * @param map The map.
 */
void Enemy::set_map(Map& map) {

  Detector::set_map(map);

  if (is_enabled()) {
    initialize();
    enable_pixel_collisions();
  }

  if (map.is_loaded()) {
    // We are not during the map initialization phase.
    restart();
  }
}

/**
 * @brief Notifies this entity that its map has just become active.
 */
void Enemy::notify_map_started() {

  MapEntity::notify_map_started();

  // give the sprite their initial direction
  int initial_direction = get_direction();
  std::list<Sprite*>::iterator it;
  for (it = get_sprites().begin(); it != get_sprites().end(); it++) {
    (*it)->set_current_direction(initial_direction);
  }
}

/**
 * @brief Notifies this entity that its map has just become active.
 */
void Enemy::notify_map_opening_transition_finished() {

  Detector::notify_map_opening_transition_finished();

  if (is_enabled() && is_in_normal_state()) {
    restart();
  }
}

/**
 * @brief Returns the breed of this enemy.
 * @return The breed.
 */
const std::string& Enemy::get_breed() {
  return breed;
}

/**
 * @brief Returns the rank of the enemy.
 * @return the enemy rank
 */
Enemy::Rank Enemy::get_rank() {
  return rank;
}

/**
 * @brief Returns whether the state of this enemy is saved.
 * @return true if this enemy is saved.
 */
bool Enemy::is_saved() {
  return !savegame_variable.empty();
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Enemy::is_obstacle_for(MapEntity& other) {

  return is_enabled() && other.is_enemy_obstacle(*this);
}

/**
 * @brief Returns whether a destructible item is currently considered as an obstacle for this entity.
 * @param destructible a destructible item
 * @return true if the destructible item is currently an obstacle this entity
 */
bool Enemy::is_destructible_obstacle(Destructible& destructible) {

  // the destructible item is an obstacle unless the enemy is already overlapping it,
  // which is possible with bomb flowers
  if (this->overlaps(destructible)) {
    return false;
  }
  return obstacle_behavior != OBSTACLE_BEHAVIOR_FLYING;
}

/**
 * @brief Returns whether a teletransporter is currently considered as an obstacle.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Enemy::is_teletransporter_obstacle(Teletransporter& teletransporter) {
  return false;
}

/**
 * @brief Returns whether a deep water tile is currently considered as an obstacle by this entity.
 * @return true if the deep water tiles are currently an obstacle for this entity
 */
bool Enemy::is_deep_water_obstacle() {
  return obstacle_behavior != OBSTACLE_BEHAVIOR_FLYING
      && obstacle_behavior != OBSTACLE_BEHAVIOR_SWIMMING;
}

/**
 * @brief Returns whether a shallow water tile is currently considered as an obstacle by this entity.
 * @return true if the shallow water tiles are currently an obstacle for this entity
 */
bool Enemy::is_shallow_water_obstacle() {
  return false;
}

/**
 * @brief Returns whether a hole is currently considered as an obstacle by this entity.
 * @return true if the holes are currently an obstacle for this entity
 */
bool Enemy::is_hole_obstacle() {
  return obstacle_behavior != OBSTACLE_BEHAVIOR_FLYING;
}

/**
 * @brief Returns whether prickles are currently considered as obstacle by this entity.
 * @return true if prickles are currently obstacle for this entity
 */
bool Enemy::is_prickle_obstacle() {
  return false;
}

/**
 * @brief Returns whether lava is currently considered as obstacle by this entity.
 * @return true if lava is currently obstacle for this entity
 */
bool Enemy::is_lava_obstacle() {
  return obstacle_behavior != OBSTACLE_BEHAVIOR_FLYING;
}

/**
 * @brief Returns the amount of damage this kind of enemy can make to the hero.
 * @return Number of life points the player loses.
 */
int Enemy::get_damage() {
  return damage_on_hero;
}

/**
 * @brief Sets the amount of damage this kind of enemy can make to the hero.
 * @param damage_on_hero Number of life points the player loses.
 */
void Enemy::set_damage(int damage_on_hero) {
  this->damage_on_hero = damage_on_hero;
}

/**
 * @brief Returns the amount of magic damage this kind of enemy can make to the hero.
 * @return Number of magic points the player loses.
 */
int Enemy::get_magic_damage() {
  return magic_damage_on_hero;
}

/**
 * @brief Sets the amount of magic damage this kind of enemy can make to the hero.
 * @param magic_damage_on_hero Number of magic points the player loses.
 */
void Enemy::set_magic_damage(int magic_damage_on_hero) {
  this->magic_damage_on_hero = magic_damage_on_hero;
}

/**
 * @brief Returns the number of health points of the enemy.
 * @return number of health points of the enemy
 */
int Enemy::get_life() {
  return life;
}

/**
 * @brief Sets the number of health points of the enemy.
 * @param life number of health points of the enemy
 */
void Enemy::set_life(int life) {

  this->life = life;
  if (!being_hurt && this->life <= 0) {
    kill();
  }
}

/**
 * @brief Returns the style of sounds and animations when this enemy is hurt.
 * @return The style when hurt.
 */
Enemy::HurtStyle Enemy::get_hurt_style() {
  return hurt_style;
}

/**
 * @brief Sets the style of sounds and animations when this enemy is hurt.
 * @param hurt_style The style when hurt.
 */
void Enemy::set_hurt_style(HurtStyle hurt_style) {
  this->hurt_style = hurt_style;
}

/**
 * @brief Returns whether this enemy can currently attack the hero.
 * @return true if this enemy can currently attack the hero
 */
bool Enemy::get_can_attack() {
  return can_attack;
}

/**
 * @brief Sets whether this enemy can attack the hero.
 * @param can_attack true to allow this enemy to attack the hero
 */
void Enemy::set_can_attack(bool can_attack) {

  this->can_attack = can_attack;
  if (!can_attack) {
    can_attack_again_date = 0;
  }
}

/**
 * @brief Returns the current behavior with obstacles for this enemy.
 * @return the behavior with obstacles
 */
Enemy::ObstacleBehavior Enemy::get_obstacle_behavior() {
  return obstacle_behavior;
}

/**
 * @brief Sets the behavior with obstacles for this enemy.
 * @param obstacle_behavior the behavior with obstacles
 */
void Enemy::set_obstacle_behavior(ObstacleBehavior obstacle_behavior) {
  this->obstacle_behavior = obstacle_behavior;
}

/**
 * @brief Returns whether the enemy is pushed back when it gets hurt by the hero.
 * @return \c true if the enemy is pushed back when it gets hurt.
 */
bool Enemy::get_pushed_back_when_hurt() {
  return pushed_back_when_hurt;
}


/**
 * @brief Sets whether the enemy is pushed back when it gets hurt by the hero.
 * @param pushed_back_when_hurt true to make the enemy pushed back when it gets hurt.
 */
void Enemy::set_pushed_back_when_hurt(bool pushed_back_when_hurt) {
  this->pushed_back_when_hurt = pushed_back_when_hurt;
}

/**
 * @brief Returns whether the hero is pushed when he strikes the enemy with his sword.
 * @return true if the hero is pushed away when he strkes the enemy with his sword
 */
bool Enemy::get_push_hero_on_sword() {
  return push_hero_on_sword;
}

/**
 * @brief Sets whether the hero is pushed when he hurts the enemy with his sword.
 * @param push_hero_on_sword true to make the hero pushed back when he hurts the enemy with his sword
 */
void Enemy::set_push_hero_on_sword(bool push_hero_on_sword) {
  this->push_hero_on_sword = push_hero_on_sword;
}

/**
 * @brief Returns whether this enemy can hurt the hero even if the hero is running.
 * @return true if this enemy can attack the hero while he is running
 */
bool Enemy::get_can_hurt_hero_running() {
  return can_hurt_hero_running;
}

/**
 * @brief Sets whether this enemy can hurt the hero even if the hero is running.
 * @param can_hurt_hero_running true to allow this enemy to attack the hero while he is running
 */
void Enemy::set_can_hurt_hero_running(bool can_hurt_hero_running) {
  this->can_hurt_hero_running = can_hurt_hero_running;
}

/**
 * @brief Returns the shield level required to avoid attacks of this enemy.
 * @return The minimum shield needed (\c 0 means unavoidable attacks).
 */
int Enemy::get_minimum_shield_needed() {
  return minimum_shield_needed;
}

/**
 * @brief Sets the shield level required to avoid attacks of this enemy.
 * @param minimum_shield_needed The minimum shield needed (\c 0 means unavoidable attacks).
 */
void Enemy::set_minimum_shield_needed(int minimum_shield_needed) {
  this->minimum_shield_needed = minimum_shield_needed;
}

/**
 * @brief Returns the reaction corresponding to an attack on a sprite of this enemy.
 * @param attack an attack this enemy receives
 * @param this_sprite the sprite attacked, or NULL if the attack does not come from
 * a pixel-precise collision test
 * @return the corresponding reaction
 */
const EnemyReaction::Reaction& Enemy::get_attack_consequence(EnemyAttack attack, Sprite *this_sprite) {

  return attack_reactions[attack].get_reaction(this_sprite);
}

/**
 * @brief Sets how the enemy reacts to an attack.
 *
 * This reaction will be used unless the attack is received by a sprite
 * that has a specific reaction (see set_attack_consequence_sprite()).
 *
 * @param attack an attack
 * @param reaction how the enemy will react
 * @param life_lost number of life points lost with this attack (possibly zero)
 */
void Enemy::set_attack_consequence(EnemyAttack attack,
    EnemyReaction::ReactionType reaction, int life_lost) {

  Debug::check_assertion(life_lost >= 0, StringConcat() << "Invalid amount of life: " << life_lost);
  attack_reactions[attack].set_general_reaction(reaction, life_lost);
}

/**
 * @brief Sets how the enemy reacts to an attack on a particular sprite.
 * @param sprite a sprite of this enemy
 * @param attack an attack
 * @param reaction how the enemy will react
 * @param life_lost number of life points lost with this attack (possibly zero)
 */
void Enemy::set_attack_consequence_sprite(Sprite& sprite, EnemyAttack attack,
    EnemyReaction::ReactionType reaction, int life_lost) {

  Debug::check_assertion(life_lost >= 0, StringConcat() << "Invalid amount of life: " << life_lost);
  attack_reactions[attack].set_sprite_reaction(&sprite, reaction, life_lost);
}

/**
 * @brief Sets the enemy insensible to all attacks.
 */
void Enemy::set_no_attack_consequences() {

  for (int i = 0; i < ATTACK_NUMBER; i++) {
    set_attack_consequence(EnemyAttack(i), EnemyReaction::IGNORED);
  }
}

/**
 * @brief Sets a particular sprite of the enemy insensible to all attacks.
 * @param sprite a sprite of this enemy
 */
void Enemy::set_no_attack_consequences_sprite(Sprite& sprite) {

  for (int i = 0; i < ATTACK_NUMBER; i++) {
    set_attack_consequence_sprite(sprite, EnemyAttack(i), EnemyReaction::IGNORED);
  }
}

/**
 * @brief Set some default values for the reactions of the attacks.
 */
void Enemy::set_default_attack_consequences() {

  for (int i = 0; i < ATTACK_NUMBER; i++) {
    attack_reactions[i].set_default_reaction();
  }
  set_attack_consequence(ATTACK_SWORD, EnemyReaction::HURT, 1); // multiplied by the sword damage factor
  set_attack_consequence(ATTACK_THROWN_ITEM, EnemyReaction::HURT, 1); // multiplied depending on the item
  set_attack_consequence(ATTACK_EXPLOSION, EnemyReaction::HURT, 2);
  set_attack_consequence(ATTACK_ARROW, EnemyReaction::HURT, 2);
  set_attack_consequence(ATTACK_HOOKSHOT, EnemyReaction::IMMOBILIZED);
  set_attack_consequence(ATTACK_BOOMERANG, EnemyReaction::IMMOBILIZED);
  set_attack_consequence(ATTACK_FIRE, EnemyReaction::HURT, 3);
}

/**
 * @brief Set some default values for the reactions of the attacks
 * on a particular sprite of this enemy.
 * @param sprite a sprite of this enemy
 */
void Enemy::set_default_attack_consequences_sprite(Sprite& sprite) {

  for (int i = 0; i < ATTACK_NUMBER; i++) {
    set_attack_consequence_sprite(sprite, EnemyAttack(i), EnemyReaction::HURT, 1);
  }
  set_attack_consequence_sprite(sprite, ATTACK_EXPLOSION, EnemyReaction::HURT, 2);
  set_attack_consequence_sprite(sprite, ATTACK_HOOKSHOT, EnemyReaction::IMMOBILIZED);
  set_attack_consequence_sprite(sprite, ATTACK_BOOMERANG, EnemyReaction::IMMOBILIZED);
}

/**
 * @brief Returns the current animation of the first sprite of the enemy.
 *
 * This function is useful when the enemy has several sprites.
 *
 * @return name of the current animation of the first sprite
 */
const std::string& Enemy::get_animation() {

  return get_sprite().get_current_animation();
}

/**
 * @brief Changes the animation of this enemy's sprites.
 *
 * This function is useful when the enemy has several sprites.
 *
 * @param animation name of the animation to set
 */
void Enemy::set_animation(const std::string& animation) {

  std::list<Sprite*>::iterator it;
  for (it = get_sprites().begin(); it != get_sprites().end(); it++) {
    (*it)->set_current_animation(animation);
  }
}

/**
 * @brief Returns whether this entity has to be drawn in y order.
 * @return \c true if this type of entity should be drawn at the same level
 * as the hero.
 */
bool Enemy::is_drawn_in_y_order() {
  return drawn_in_y_order;
}

/**
 * @brief Updates the enemy.
 */
void Enemy::update() {

  MapEntity::update();

  if (suspended || !is_enabled()) {
    return;
  }

  uint32_t now = System::now();

  if (being_hurt) {

    // see if we should stop the animation "hurt"
    if (now >= stop_hurt_date) {
      being_hurt = false;

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
      Rectangle xy;
      xy.set_x(get_top_left_x() + Random::get_number(get_width()));
      xy.set_y(get_top_left_y() + Random::get_number(get_height()));
      get_entities().add_entity(new Explosion("", LAYER_HIGH, xy, false));
      Sound::play("explosion");

      next_explosion_date = now + 200;
      nb_explosions++;

      if (nb_explosions >= 15) {
        exploding = false;
      }
    }
  }

  if (is_killed() && is_dying_animation_finished()) {

    // create the pickable treasure if any
    get_entities().add_entity(Pickable::create(get_game(),
        "", get_layer(), get_x(), get_y(),
        treasure, FALLING_HIGH, false));

    // notify Lua
    notify_dead();

    // remove the enemy
    remove_from_map();
  }

  get_lua_context().enemy_on_update(*this);
}

/**
 * @brief Suspends or resumes the entity.
 * @param suspended true to suspend the entity, false to resume it
 */
void Enemy::set_suspended(bool suspended) {

  Detector::set_suspended(suspended);

  if (!suspended) {
    uint32_t diff = System::now() - when_suspended;
    stop_hurt_date += diff;
    vulnerable_again_date += diff;
    can_attack_again_date += diff;
    start_shaking_date += diff;
    end_shaking_date += diff;
    next_explosion_date += diff;
  }
  get_lua_context().enemy_on_suspended(*this, suspended);
}

/**
 * @brief Draws the entity on the map.
 */
void Enemy::draw_on_map() {

  if (is_visible()) {
    get_lua_context().enemy_on_pre_draw(*this);
  }

  Detector::draw_on_map();

  if (is_visible()) {
    get_lua_context().enemy_on_post_draw(*this);
  }
}

/**
 * @brief Notifies this entity that it was just enabled or disabled.
 * @param enabled true if the entity is now enabled
 */
void Enemy::notify_enabled(bool enabled) {

  Detector::notify_enabled(enabled);

  if (enabled) {
    if (!initialized) {
      initialize();
    }
    restart();
    get_lua_context().enemy_on_enabled(*this);
  }
  else {
    get_lua_context().enemy_on_disabled(*this);
  }
}

/**
 * @brief Notifies this entity that it has just failed to change its position
 * because of obstacles.
 */
void Enemy::notify_obstacle_reached() {

  Detector::notify_obstacle_reached();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_obstacle_reached(*this, *get_movement());
  }
}

/**
 * @brief This function is called when the entity has just moved.
 */
void Enemy::notify_position_changed() {

  Detector::notify_position_changed();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_position_changed(*this, get_xy(), get_layer());
  }
}

/**
 * @brief This function is called when the layer of this entity has just changed.
 *
 * Redefine it if you need to be notified.
 */
void Enemy::notify_layer_changed() {

  Detector::notify_layer_changed();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_position_changed(*this, get_xy(), get_layer());
  }
}

/**
 * @brief This function can be called by the movement object
 * to notify the entity when the movement has just changed
 * (e.g. the speed, the angle or the trajectory).
 */
void Enemy::notify_movement_changed() {

  Detector::notify_movement_changed();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_movement_changed(*this, *get_movement());
  }
}

/**
 * @brief This function is called when the movement of the entity is finished.
 */
void Enemy::notify_movement_finished() {

  Detector::notify_movement_finished();

  if (!is_being_hurt()) {
    get_lua_context().enemy_on_movement_finished(*this);
  }
}

/**
 * @brief Notifies the enemy that a collision was just detected with another entity
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void Enemy::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_enemy(*this);
}

/**
 * @brief Notifies this enemy that a pixel-precise collision was just detected with another sprite.
 *
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this enemy.
 *
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void Enemy::notify_collision(MapEntity &other_entity, Sprite &other_sprite, Sprite &this_sprite) {

  other_entity.notify_collision_with_enemy(*this, this_sprite, other_sprite);
}

/**
 * @brief This function is called when an explosion's sprite detects a collision
 * with a sprite of this enemy.
 * @param explosion the explosion
 * @param sprite_overlapping the sprite of this enemy that collides with the explosion
 */
void Enemy::notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping) {

  explosion.try_attack_enemy(*this, sprite_overlapping);
}

/**
 * @brief Notifies this entity that a sprite of fire
 * detects a pixel-precise collision with a sprite of this entity.
 * @param fire the fire
 * @param sprite_overlapping the sprite of the current entity that collides with the fire
 */
void Enemy::notify_collision_with_fire(Fire& fire, Sprite& sprite_overlapping) {

  try_hurt(ATTACK_FIRE, fire, &sprite_overlapping);
}

/**
 * @brief This function is called when this enemy detects a collision with another enemy.
 * @param other the other enemy
 * @param other_sprite the other enemy's sprite that overlaps a sprite of this enemy
 * @param this_sprite this enemy's sprite that overlaps the other
 */
void Enemy::notify_collision_with_enemy(Enemy& other,
    Sprite& other_sprite, Sprite& this_sprite) {

  if (is_in_normal_state()) {
    get_lua_context().enemy_on_collision_enemy(
        *this, other, other_sprite, this_sprite);
  }
}

/**
 * @brief Attacks the hero if possible.
 *
 * This function is called when there is a collision between the enemy and the hero.
 *
 * @param hero the hero
 * @param this_sprite the sprite of this enemy that detected the collision with the hero,
 * or NULL if it was not a pixel-precise collision.
 */
void Enemy::attack_hero(Hero &hero, Sprite *this_sprite) {

  if (!is_immobilized() && can_attack && hero.can_be_hurt(this)) {

    bool hero_protected = false;
    if (minimum_shield_needed != 0
        && get_equipment().has_ability("shield", minimum_shield_needed)) {

      // compute the direction corresponding to the angle between the enemy and the hero
      double angle = hero.get_angle(*this);
      int protected_direction4 = (int) ((angle + Geometry::PI_OVER_2 / 2.0) * 4 / Geometry::TWO_PI);
      protected_direction4 = (protected_direction4 + 4) % 4;

      // also get the direction of the enemy's sprite
      int sprite_opposite_direction4 = -1;
      if (this_sprite != NULL) {
        sprite_opposite_direction4 = (this_sprite->get_current_direction() + 2) % 4;
      }

      // the hero is protected if he is facing the opposite of one of these directions
      hero_protected = hero.is_facing_direction4(protected_direction4) ||
          hero.is_facing_direction4(sprite_opposite_direction4);
    }

    if (hero_protected) {
      attack_stopped_by_hero_shield();
    }
    else {
      hero.hurt(*this, damage_on_hero, magic_damage_on_hero);
    }
  }
}

/**
 * @brief This function is called when the hero stops an attack with his shield.
 *
 * By default, the shield sound is played and the enemy cannot attack again for a while.
 */
void Enemy::attack_stopped_by_hero_shield() {

  Sound::play("shield");

  uint32_t now = System::now();
  can_attack = false;
  can_attack_again_date = now + 1000;

  get_equipment().notify_ability_used("shield");
}

/**
 * @brief Plays the appropriate sound when the enemy is hurt.
 */
void Enemy::play_hurt_sound() {

  std::string sound_id = "";
  switch (hurt_style) {

    case HURT_NORMAL:
      sound_id = "enemy_hurt";
      break;

    case HURT_MONSTER:
      sound_id = "monster_hurt";
      break;

    case HURT_BOSS:
      sound_id = (life > 0) ? "boss_hurt" : "boss_killed";
      break;

    case HURT_NUMBER:
      Debug::die(StringConcat() << "Invalid hurt style" << hurt_style);
      break;
  }

  Sound::play(sound_id);
}

/**
 * @brief Notifies this enemy that it should restart his movement.
 *
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 * By default, the "walking" animation is set on the enemy's sprites.
 */
void Enemy::restart() {

  if (is_immobilized()) {
    stop_immobilized();
  }
  set_animation("walking");
  get_lua_context().enemy_on_restarted(*this);
}

/**
 * @brief Returns whether this enemy is in a normal state.
 *
 * The enemy is considered to be in its normal state if
 * it is not disabled, dying, being hurt or immobilized.
 * When this method returns false, the subclasses of Enemy
 * should not change the enemy properties.
 *
 * @return true if this enemy is in a normal state
 */
bool Enemy::is_in_normal_state() {
  return is_enabled()
    && !is_being_hurt()
    && get_life() > 0
    && !is_immobilized()
    && !is_being_removed();
}

/**
 * @brief Returns whether this enemy is currently invulnerable.
 * @return \c true if this enemy cannot be hurt for now.
 */
bool Enemy::is_invulnerable() {
  return invulnerable;
}

/**
 * @brief Makes the enemy receive an attack.
 *
 * He might resist to the attack or get hurt.
 *
 * @param attack type of attack
 * @param source the entity attacking the enemy (often the hero)
 * @param this_sprite the sprite of this enemy that received the attack, or NULL
 * if the attack comes from a non pixel-precise collision test
 */
void Enemy::try_hurt(EnemyAttack attack, MapEntity& source, Sprite* this_sprite) {

  EnemyReaction::Reaction reaction = get_attack_consequence(attack, this_sprite);
  if (invulnerable || reaction.type == EnemyReaction::IGNORED) {
    // ignore the attack
    return;
  }

  invulnerable = true;
  vulnerable_again_date = System::now() + 500;

  switch (reaction.type) {

    case EnemyReaction::PROTECTED:
      // attack failure sound
      Sound::play("sword_tapping");
      break;

    case EnemyReaction::IMMOBILIZED:
      // get immobilized
      hurt(source);
      immobilize();
      notify_hurt(source, attack, 0);
      break;

    case EnemyReaction::CUSTOM:
      // custom attack (defined in the script)
      if (is_in_normal_state()) {
        custom_attack(attack, this_sprite);
      }
      else {
        // no attack was made: notify the source correctly
        reaction.type = EnemyReaction::IGNORED;
        invulnerable = false;
      }
      break;

    case EnemyReaction::HURT:

      if (is_immobilized() && get_sprite().get_current_animation() == "shaking") {
        stop_immobilized();
      }

      // compute the number of health points lost by the enemy

      if (attack == ATTACK_SWORD) {

        // for a sword attack, the damage depends on the sword and the variant of sword attack used
        int damage_multiplicator = ((Hero&) source).get_sword_damage_factor();
        reaction.life_lost *= damage_multiplicator;
      }
      else if (attack == ATTACK_THROWN_ITEM) {
        reaction.life_lost *= ((CarriedItem&) source).get_damage_on_enemies();
      }
      life -= reaction.life_lost;

      hurt(source);
      notify_hurt(source, attack, reaction.life_lost);
      break;

    case EnemyReaction::IGNORED:
    case EnemyReaction::REACTION_NUMBER:
      Debug::die(StringConcat() << "Invalid enemy reaction" << reaction.type);
      break;
  }

  // notify the source
  source.notify_attacked_enemy(attack, *this, reaction, get_life() <= 0);
}

/**
 * @brief Hurts the enemy.
 *
 * Updates its state, its sprite and plays the sound.
 *
 * @param source the entity attacking the enemy (often the hero)
 */
void Enemy::hurt(MapEntity &source) {

  uint32_t now = System::now();

  // update the enemy state
  being_hurt = true;

  can_attack = false;
  can_attack_again_date = now + 300;

  // graphics and sounds
  set_animation("hurt");
  play_hurt_sound();

  // stop any movement
  clear_movement();

  // push the enemy back
  if (pushed_back_when_hurt) {
    double angle = source.get_angle(*this);
    StraightMovement* movement = new StraightMovement(false, true);
    movement->set_max_distance(24);
    movement->set_speed(120);
    movement->set_angle(angle);
    set_movement(movement);
  }

  stop_hurt_date = now + 300;
}

/**
 * @brief This function is called when the enemy has just been hurt.
 * @param source the source of the attack
 * @param attack the attack that was just successful
 * @param life_points the number of life points lost by this enemy
 */
void Enemy::notify_hurt(MapEntity &source, EnemyAttack attack, int life_points) {

  if (get_life() <= 0) {
    get_lua_context().enemy_on_dying(*this);
  }
  get_lua_context().enemy_on_hurt(*this, attack, life_points);
}

/**
 * @brief This function is called when the enemy has just finished dying.
 */
void Enemy::notify_dead() {

  get_lua_context().enemy_on_dead(*this);
}

/**
 * @brief This function is called when the enemy is immobilized,
 * after the hurt animation.
 */
void Enemy::notify_immobilized() {

  get_lua_context().enemy_on_immobilized(*this);
}

/**
 * @brief Kills the enemy.
 *
 * This function is called when the enemy has no more health points.
 */
void Enemy::kill() {

  // if the enemy is immobilized, give some money
  if (rank == RANK_NORMAL && is_immobilized() && !treasure.is_saved()) {
    // TODO choose random money (can we do this from scripts?)
  }

  // stop any movement and disable attacks
  set_collision_modes(COLLISION_NONE);
  clear_movement();
  invulnerable = true;
  can_attack = false;
  can_attack_again_date = 0;

  if (hurt_style != HURT_BOSS) {
    // replace the enemy sprites
    clear_sprites();
    create_sprite("enemies/enemy_killed");
    Sound::play("enemy_killed");
  }
  else {
    // a boss: create some explosions
    exploding = true;
    nb_explosions = 0;
    next_explosion_date = System::now() + 2000;
  }

  // save the enemy state if required
  if (is_saved()) {
    get_savegame().set_boolean(savegame_variable, true);
  }
}

/**
 * @brief Returns whether the enemy is being hurt.
 * @return true if the enemy is being hurt
 */
bool Enemy::is_being_hurt() {
  return being_hurt;
}

/**
 * @brief Returns whether the enemy is killed.
 *
 * An enemy is considered as killed if he has no more life and its dying
 * animation has started.
 *
 * @return true if the enemy is killed
 */
bool Enemy::is_killed() {
  return life <= 0
    && (get_sprite().get_animation_set_id() == "enemies/enemy_killed" || next_explosion_date > 0);
}

/**
 * @brief When the enemy is killed, returns whether the dying animation is finished.
 * @return true if the dying animation is finished
 */
bool Enemy::is_dying_animation_finished() {
  
  if (hurt_style != HURT_BOSS) {
    return get_sprite().is_animation_finished();
  }

  return nb_explosions > 0 && !exploding;
}

/**
 * @brief Returns whether the enemy is dying, i.e. his life has reached zero and the dying animation is playing.
 * @return true if the enemy is dying
 */
bool Enemy::is_dying() {

  return get_life() <= 0;
}

/**
 * @brief Sets the treasure dropped by this enemy.
 * @param treasure the treasure to set
 */
void Enemy::set_treasure(const Treasure& treasure) {
  this->treasure = treasure;
}

/**
 * @brief Returns true if the current sprite animation is finished or is looping.
 * @return true if the current sprite animation is finished or is looping 
 */
bool Enemy::is_sprite_finished_or_looping() {

  Sprite &sprite = get_sprite();
  return sprite.is_animation_finished() || sprite.is_animation_looping();
}

/**
 * @brief Immobilizes this enemy.
 */
void Enemy::immobilize() {

  immobilized = true;
  start_shaking_date = System::now() + 5000; 
}

/**
 * @brief Stops immobilizing the enemy.
 */
void Enemy::stop_immobilized() {

  immobilized = false;
  end_shaking_date = 0;
}

/**
 * @brief Returns whether this enemy is immobilized. 
 * @return true if this enemy is immobilized 
 */
bool Enemy::is_immobilized() {

  return immobilized;
}

/**
 * @brief This function is called when the enemy is attacked by a custom effect attack.
 *
 * Redefine this function to handle the attack.
 *
 * @param attack the attack
 * @param this_sprite the sprite of this enemy subject to the attack, or NULL
 * if the attack does not come from a pixel-precise collision test
 */
void Enemy::custom_attack(EnemyAttack attack, Sprite* this_sprite) {

  get_lua_context().enemy_on_custom_attack_received(*this, attack, this_sprite);
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return The name identifying this type in Lua.
 */
const std::string& Enemy::get_lua_type_name() const {
  return LuaContext::entity_enemy_module_name;
}

