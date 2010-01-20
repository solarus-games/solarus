/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
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
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "ResourceManager.h"
#include "Map.h"
#include "MapScript.h"
#include "movements/StraightMovement.h"
#include "movements/FallingHeight.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Random.h"
#include "lowlevel/System.h"
#include "enemies/SimpleGreenSoldier.h"
#include "enemies/Bubble.h"
#include "enemies/Tentacle.h"
#include "enemies/Minillosaur.h"
#include "enemies/PapillosaurKing.h"
#include "enemies/Khorneth.h"

/**
 * Creates an enemy.
 * This constructor can only be called by the subclasses. Use the static method create() to create enemies.
 * @param params the name and position of the enemy
 */
Enemy::Enemy(const ConstructionParameters &params):
  Detector(COLLISION_RECTANGLE | COLLISION_SPRITE, params.name, params.layer, params.x, params.y, 0, 0),
  being_hurt(false), stop_hurt_date(0), normal_movement(NULL), invulnerable(false), vulnerable_again_date(0),
  can_attack(true), can_attack_again_date(0), immobilized(false),
  start_shaking_date(0), end_shaking_date(0), exploding(false), nb_explosions(0), next_explosion_date(0) {

}

/**
 * Destructor.
 */
Enemy::~Enemy(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Enemy::get_type() {
  return ENEMY;
}

/**
 * Creates an instance from an input stream.
 * The input stream must respect the syntax of this entity type.
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * Enemy::parse(std::istream &is, Layer layer, int x, int y) {

  int direction, subtype, rank, savegame_variable, pickable_item_type, pickable_item_savegame_variable;
  std::string name;

  FileTools::read(is, name);
  FileTools::read(is, direction);
  FileTools::read(is, subtype);
  FileTools::read(is, rank);
  FileTools::read(is, savegame_variable);
  FileTools::read(is, pickable_item_type);
  FileTools::read(is, pickable_item_savegame_variable);

  return create(Subtype(subtype), Enemy::Rank(rank), savegame_variable, name, Layer(layer), x, y, direction, 
      PickableItem::Subtype(pickable_item_type), pickable_item_savegame_variable);
}

/**
 * Creates an enemy with the specified type.
 * This method acts like a constructor, except that it returns an object from a
 * subclass of Enemy.
 * @param type type of enemy to create
 * @param name a name identifying the enemy
 * @param rank rank of the enemy: normal, miniboss or boss
 * @param savegame_variable index of the boolean variable indicating that
 * @param layer layer of the enemy on the map
 * @param x x position of the enemy on the map
 * @param y y position of the enemy on the map
 * @param direction initial direction of the enemy on the map (0 to 3)
 * this enemy is killed, or -1 if this enemy is not saved
 * @param pickable_item_subtype subtype of pickable item the enemy drops
 * @param pickable_item_savegame_variable index of the boolean variable
 * @return the enemy created
 */
MapEntity * Enemy::create(Subtype type, Rank rank, int savegame_variable,
    const std::string &name, Layer layer, int x, int y, int direction,
    PickableItem::Subtype pickable_item_subtype, int pickable_item_savegame_variable) {

  // create the enemy
  Enemy *enemy;
  const ConstructionParameters params = {name, layer, x, y};

  switch (type) {
    
  case SIMPLE_GREEN_SOLDIER: enemy = new SimpleGreenSoldier(params); break;
  case BUBBLE:               enemy = new Bubble(params);             break;
  case TENTACLE:             enemy = new Tentacle(params);           break;
  case MINILLOSAUR:          enemy = new Minillosaur(params);        break;
  case PAPILLOSAUR_KING:     enemy = new PapillosaurKing(params);    break;
  case KHORNETH:             enemy = new Khorneth(params);    break;

  default:
    DIE("Unknown enemy type '" << type << "'");
  }

  // initialize the fields
  enemy->set_direction(direction);
  enemy->rank = rank;
  enemy->enabled = (rank == RANK_NORMAL);
  enemy->savegame_variable = savegame_variable;
  enemy->pickable_item_subtype = pickable_item_subtype;
  enemy->pickable_item_savegame_variable = pickable_item_savegame_variable;

  // set the default enemy features
  enemy->damage_on_hero = 1;
  enemy->magic_damage_on_hero = 0;
  enemy->life = 1;
  enemy->hurt_sound_style = (rank == RANK_NORMAL) ? HURT_SOUND_NORMAL : HURT_SOUND_BOSS;
  enemy->pushed_back_when_hurt = true;
  enemy->push_back_hero_on_sword = false;
  enemy->minimum_shield_needed = 0;

  enemy->set_default_attack_consequences();

  return enemy;
}

/**
 * Just after the entity is created, this function is called once to check whether
 * the entity created can be added to the specified map and in the current game state.
 * @param map the map where this entity is about to be added
 * @return true if the entity can be added, false otherwise
 */
bool Enemy::can_be_added(Map *map) {

  Savegame *savegame = map->get_game()->get_savegame();

  // see if the enemy is dead
  if (savegame_variable != -1 && savegame->get_boolean(savegame_variable)) {

    // the enemy is dead and saved: checked whether it releases a pickable item saved
    if (pickable_item_savegame_variable != -1) {
      // create the pickable item that the player has forgotten after he killed the enemy
      PickableItem *item = PickableItem::create(get_layer(), get_x(), get_y(), pickable_item_subtype, pickable_item_savegame_variable, FALLING_NONE, false);
      map->get_entities()->add_entity(item);
    }
    return false;
  }

  return true;
}

/**
 * Sets the map.
 * @param map the map
 */
void Enemy::set_map(Map *map) {

  MapEntity::set_map(map);

  if (is_enabled()) {
    // let the subclass initialize the enemy
    initialize();
    enable_pixel_collisions();
    restart();
  }
}

/**
 * Returns the rank of the enemy.
 * @return the enemy rank
 */
Enemy::Rank Enemy::get_rank(void) {
  return rank;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Enemy::is_obstacle_for(MapEntity *other) {
  return is_enabled() && other->is_enemy_obstacle(this);
}

/**
 * Returns whether a sensor is currently considered as an obstacle for this entity.
 * @param sensor a sensor
 * @return true if the sensor is currently an obstacle this entity
 */
bool Enemy::is_sensor_obstacle(Sensor *sensor) {
  return true;
}

/**
 * Sets the amount of damage this kind of enemy can make to the hero.
 * @param damage_on_hero number of heart quarters the player loses
 */
void Enemy::set_damage(int damage_on_hero) {
  this->damage_on_hero = damage_on_hero;
}

/**
 * Sets the amount of damage this kind of enemy can make to the hero.
 * @param damage_on_hero number of heart quarters the player loses
 * @param magic_damage_on_hero number of magic points the player loses
 */
void Enemy::set_damage(int damage_on_hero, int magic_damage_on_hero) {
  set_damage(damage_on_hero);
  this->magic_damage_on_hero = magic_damage_on_hero;
}

/**
 * Sets the number of health points of the enemy.
 * @param life number of health points of the enemy
 */
void Enemy::set_life(int life) {
  this->life = life;
}

/**
 * Returns the number of health points of the enemy.
 * @return number of health points of the enemy
 */
int Enemy::get_life(void) {
  return life;
}
 
/**
 * Sets whether the enemy is pushed back when it gets hurt by the hero
 * @param pushed_back_when_hurt true to make the enemy pushed back when it gets hury
 */
void Enemy::set_pushed_back_when_hurt(bool pushed_back_when_hurt) {
  this->pushed_back_when_hurt = pushed_back_when_hurt;
}

/**
 * Sets whether the hero is pushed when he hurts the enemy with his sword.
 * @param push_back_hero_on_sword true to make the hero pushed back when he hurts the enemy with his sword
 */
void Enemy::set_push_back_hero_on_sword(bool push_back_hero_on_sword) {
  this->push_back_hero_on_sword = push_back_hero_on_sword;
}

/**
 * Sets some features of this type of enemy.
 * @param damage_on_hero number of heart quarters the player loses
 * @param life number of health points of the enemy
 */
void Enemy::set_features(int damage_on_hero, int life) {
  set_damage(damage_on_hero);
  set_life(life);
}

/**
 * Sets some features of this type of enemy.
 * @param damage_on_hero number of heart quarters the player loses
 * @param life number of health points of the enemy
 * @param hurt_sound_style the sound played when this kind of enemy gets hurt by the hero
 */
void Enemy::set_features(int damage_on_hero, int life, HurtSoundStyle hurt_sound_style) {
  set_features(damage_on_hero, life);
  this->hurt_sound_style = hurt_sound_style;
}

/**
 * Sets all features of this type of enemy.
 * @param damage_on_hero number of heart quarters the player loses
 * @param life number of health points of the enemy
 * @param hurt_sound_style the sound played when this kind of enemy gets hurt by the hero
 * @param pushed_back_when_hurt indicates whether the enemy is pushed back when it gets hurt by the hero
 * @param push_back_hero_on_sword indicates whether the hero is pushed back when he hurts the enemy with his sword
 * @param minimum_shield_needed shield number needed by the hero to avoid the attack of this enemy,
 * or 0 to make the attack unavoidable
 */
void Enemy::set_features(int damage_on_hero, int life, HurtSoundStyle hurt_sound_style,
			 bool pushed_back_when_hurt, bool push_back_hero_on_sword, int minimum_shield_needed) {
  set_features(damage_on_hero, life, hurt_sound_style);
  this->pushed_back_when_hurt = pushed_back_when_hurt;
  this->push_back_hero_on_sword = push_back_hero_on_sword;
  this->minimum_shield_needed = minimum_shield_needed;
}

/**
 * Sets whether the enemy is vulnerable to a specified attack.
 * @param attack an attack
 * @param consequence how the enemy will react
 */
void Enemy::set_attack_consequence(EnemyAttack attack, int consequence) {
  attack_consequences[attack] = consequence;
}

/**
 * Sets the enemy insensible to all attacks.
 */
void Enemy::set_no_attack_consequences(void) {
  for (int i = 0; i < ATTACK_NUMBER; i++) {
    set_attack_consequence(EnemyAttack(i), 0);
  }
}

/**
 * Set some default values for the consequences of the attacks
 * this enemy can be subject to.
 */
void Enemy::set_default_attack_consequences(void) {

  for (int i = 0; i < ATTACK_NUMBER; i++) {
    set_attack_consequence(EnemyAttack(i), 1);
  }
  set_attack_consequence(ATTACK_EXPLOSION, 2);
  set_attack_consequence(ATTACK_HOOKSHOT, -2);
  set_attack_consequence(ATTACK_BOOMERANG, -2);
}

/**
 * Returns the current animation of the first sprite of the enemy.
 * This function is useful when the enemy has several sprites.
 * @return name of the current animation of the first sprite
 */
const std::string & Enemy::get_animation(void) {
  return get_sprite()->get_current_animation();
}

/**
 * Changes the animation of this enemy's sprites.
 * This function is useful when the enemy has several sprites.
 * @param animation name of the animation to set
 */
void Enemy::set_animation(const std::string &animation) {
  
  std::map<std::string, Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {
    it->second->set_current_animation(animation);
  }
}

/**
 * Updates the enemy.
 */
void Enemy::update(void) {
  MapEntity::update();

  if (suspended || !is_enabled()) {
    return;
  }

  uint32_t now = System::now();

  if (being_hurt) {
    
    // see if we should stop the animation "hurt"
    bool stop_hurt = false;
    if (pushed_back_when_hurt) {
      stop_hurt = get_movement()->is_finished() && is_sprite_finished_or_looping();
    }
    else {
      stop_hurt = now >= stop_hurt_date;
    }

    if (stop_hurt) {
      being_hurt = false;

      if (life <= 0) {
	kill();
      }
      else if (is_immobilized()) {
	clear_movement();
	set_animation("immobilized");
      }
      else {
	clear_movement();
	restore_movement(); // restore the previous movement
	restart();
      }
    }
  }

  if (life > 0 && invulnerable && now >= vulnerable_again_date && !being_hurt) {
    invulnerable = false;
  }

  if (life > 0 && !can_attack && !is_immobilized() && now >= can_attack_again_date) {
    can_attack = true;
  }

  if (is_immobilized() && !is_killed() && now >= end_shaking_date &&
      get_sprite()->get_current_animation() == "shaking") {

    stop_immobilized();
  }

  if (is_immobilized() && !is_killed() && !is_being_hurt() && now >= start_shaking_date &&
      get_sprite()->get_current_animation() != "shaking") {

    end_shaking_date = now + 2000;
    for (int i = 0; i < get_nb_sprites(); i++) {
      get_sprite()->set_current_animation("shaking");
    }
  }

  if (exploding) {
    uint32_t now = System::now();
    if (now >= next_explosion_date) {

      // create an explosion
      Rectangle xy;
      xy.set_x(get_top_left_x() + Random::get_number(get_width()));
      xy.set_y(get_top_left_y() + Random::get_number(get_height()));
      map->get_entities()->add_entity(new Explosion(LAYER_HIGH, xy, false));

      next_explosion_date = now + 200;
      nb_explosions++;

      if (nb_explosions >= 15) {
        exploding = false;
      }
    }
  }

  if (is_killed() && is_dying_animation_finished()) {

    // create the pickable item
    if (pickable_item_subtype != PickableItem::NONE) {
      bool will_disappear = PickableItem::can_disappear(pickable_item_subtype);
      map->get_entities()->add_entity(PickableItem::create(get_layer(), get_x(), get_y(), pickable_item_subtype,
	    pickable_item_savegame_variable, FALLING_HIGH,
	    will_disappear));
    }

    // notify the enemy
    just_dead();

    // remove the enemy
    map->get_entities()->remove_entity(this);

    // notify the script
    map->get_script()->event_enemy_dead(get_name());
  }
}

/**
 * This function is called when the enemy has just finished dying.
 */
void Enemy::just_dead(void) {

}

/**
 * Suspends or resumes the entity.
 * @param suspended true to suspend the entity, false to resume it
 */
void Enemy::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended);

  if (!suspended) {
    uint32_t diff = System::now() - when_suspended;
    stop_hurt_date += diff;
    vulnerable_again_date += diff;
    can_attack_again_date += diff;
    start_shaking_date += diff;
    end_shaking_date += diff;
    next_explosion_date += diff;
  }
}

/**
 * Enables or disables the enemy.
 * @param enabled true to enable it, false to disable it
 */
void Enemy::set_enabled(bool enabled) {

  if (enabled != this->enabled) {
    this->enabled = enabled;

    if (enabled) {
      initialize();
      restart();
    }
  }
}

/**
 * Returns whether the enemy is enabled.
 * @return true if the enemy is enabled
 */
bool Enemy::is_enabled(void) {
  return enabled;
}

/**
 * Returns whether this enemy is in a normal state, i.e.
 * it is not disabled, dying, being hurt or immobilized.
 * When this method returns false, the subclasses of Enemy
 * should not change the enemy properties.
 */
bool Enemy::is_in_normal_state(void) {
  return is_enabled() && !is_being_hurt() && get_life() > 0 && !is_immobilized();
}

/**
 * This function is called when the enemy needs to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 * By default, the "walking" animation is set on the enemy's sprites.
 */
void Enemy::restart(void) {
  set_animation("walking");
}

/**
 * This function is called when the enemy collides with another entity.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void Enemy::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (is_enabled()) {
    entity_overlapping->notify_collision_with_enemy(this);
  }
}

/**
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void Enemy::notify_collision(MapEntity *other_entity, Sprite *other_sprite, Sprite *this_sprite) {

  if (is_enabled()) {
    other_entity->notify_collision_with_enemy(this, this_sprite, other_sprite);
  }
}

/**
 * This function is called when an explosion's sprite detects a collision with a sprite of this enemy.
 * @param explosion the explosion
 * @param sprite_overlapping the sprite of this enemy that collides with the explosion
 */
void Enemy::notify_collision_with_explosion(Explosion *explosion, Sprite *sprite_overlapping) {
  explosion->try_attack_enemy(this, sprite_overlapping);
}

/**
 * Stops the movement temporarily.
 */
void Enemy::stop_movement(void) {
  
  if (get_movement() != NULL) {
    normal_movement = get_movement();
  }
  set_movement(NULL);
}

/**
 * Restores the movement previously stopped with stop_movement().
 */
void Enemy::restore_movement(void) {

  if (normal_movement != NULL) {
    set_movement(normal_movement);
    normal_movement = NULL;
  }
}

/**
 * Attacks the hero if possible.
 * This function is called when there is a collision between the enemy and the hero.
 * @param hero the hero
 * @param this_sprite the sprite of this enemy that detected the collision with the hero,
 * or NULL if it was not a pixel-perfect collision.
 */
void Enemy::attack_hero(Hero *hero, Sprite *this_sprite) {

  if (is_enabled() && !is_immobilized() && can_attack) {

    bool hero_protected = false;
    if (minimum_shield_needed != 0 &&
	game->get_equipment()->get_shield() >= minimum_shield_needed) {

      double angle = hero->get_vector_angle(this);
      int protected_direction = (int) ((angle + Geometry::PI_OVER_2 / 2.0) * 4 / Geometry::TWO_PI);
      protected_direction = (protected_direction + 4) % 4;

      hero_protected = (hero->get_animation_direction() == protected_direction);
    }

    if (hero_protected) {
      attack_stopped_by_hero_shield();
    }
    else {
      hero->hurt(this, damage_on_hero, magic_damage_on_hero);
    }
  }
}

/**
 * This function is called when the hero stops an attack with his shield.
 * By default, the shield sound is played and the enemy cannot attack again for a while.
 */
void Enemy::attack_stopped_by_hero_shield(void) {
  ResourceManager::get_sound("shield")->play();

  uint32_t now = System::now();
  can_attack = false;
  can_attack_again_date = now + 1000;
}

/**
 * Returns the consequence corresponding to the specified attack.
 * @param attack an attack this enemy is subject to
 * @return the corresponding consequence
 */
int Enemy::get_attack_consequence(EnemyAttack attack) {
  return attack_consequences[attack];
}

/**
 * Returns the consequence corresponding to the specified attack on the specified sprite of this enemy.
 * By default, this function does not take the sprite into account and just calls 
 * get_attack_consequence(EnemyAttack). Redefine it in subclasses of enemies that have to react differently
 * depending on their sprite attacked.
 * @param attack an attack this enemy is subject to
 * @param this_sprite the sprite attacked, or NULL if the attack does not come from
 * a pixel-perfect collision test
 * @return the corresponding attack.
 */
int Enemy::get_attack_consequence(EnemyAttack attack, Sprite *this_sprite) {
  return get_attack_consequence(attack);
}

/**
 * Plays the appropriate sounds the enemy is hurt.
 */
void Enemy::play_hurt_sound(void) {

  SoundId sound_id = "";
  switch (hurt_sound_style) {

    case HURT_SOUND_NORMAL:
      sound_id = "enemy_hurt";
      break;

    case HURT_SOUND_MONSTER:
      sound_id = "monster_hurt";
      break;

    case HURT_SOUND_BOSS:
      sound_id = (life > 0) ? "boss_hurt" : "boss_killed";
      break;
  }

  ResourceManager::get_sound(sound_id)->play();
}

/**
 * Makes the enemy subject to an attack.
 * @param attack type of attack
 * @param source the entity attacking the enemy (often the hero)
 * @param this_sprite the sprite of this enemy that received the attack, or NULL
 * if the attack comes from a non pixel-perfect collision test.
 */
void Enemy::try_hurt(EnemyAttack attack, MapEntity *source, Sprite *this_sprite) {

  if (!is_enabled()) {
    return;
  }

  int result;

  int consequence = get_attack_consequence(attack, this_sprite);
  if (invulnerable || consequence == 0) {
    // ignore the attack
    result = 0;
  }

  else {
    invulnerable = true;
    vulnerable_again_date = System::now() + 500;
    
    if (consequence == -1) {
      // shield sound
      ResourceManager::get_sound("shield")->play();
      result = -1;
    }
    else if (consequence == -2) {
      // get immobilized
      hurt(source);
      immobilize();
      result = -2;
    }
    else if (consequence == -3) {
      // custom attack (defined in the subclass)
      result = custom_attack(attack, this_sprite);
      result = -3;
    }
    else {
      // hurt the enemy

      if (is_immobilized() && get_sprite()->get_current_animation() == "shaking") {
	stop_immobilized();
      }

      // compute the number of health points lost by the enemy
      int life_lost = consequence;

      if (attack == ATTACK_SWORD) {

	// for a sword attack, the damage depends on the sword

	static const int sword_factors[] = {0, 1, 2, 4, 8};
	int sword = game->get_equipment()->get_sword();
	life_lost *= sword_factors[sword];
	if (((Hero*) source)->get_state() == Hero::SPIN_ATTACK) {
	  life_lost *= 2; // muliply by 2 if this is a spin attack
	}
      }
      else if (attack == ATTACK_THROWN_ITEM) {
	life_lost *= ((CarriedItem*) source)->get_damage_on_enemies();
      }
      life -= life_lost;

      hurt(source);
      just_hurt(source, attack, life_lost);

      result = life_lost;
    }

    // notify the source
    source->just_attacked_enemy(attack, this, result, get_life() <= 0);
  }
}

/**
 * Hurts the enemy.
 * Updates its state, its sprite and plays the sound.
 * @param source the entity attacking the enemy (often the hero)
 */
void Enemy::hurt(MapEntity *source) {

  if (!is_enabled()) {
    return;
  }

  // update the enemy state
  being_hurt = true;

  //can_attack = false;
  //can_attack_again_date = vulnerable_again_date;

  // graphics and sounds
  set_animation("hurt");
  play_hurt_sound();

  // save the movement
  stop_movement();

  // push the enemy back
  if (pushed_back_when_hurt) {
    double angle = source->get_vector_angle(this);
    set_movement(new StraightMovement(12, angle, 200));
  }
  else {
    stop_hurt_date = System::now() + 300;
  }
}

/**
 * This function is called when the enemy has just been hurt.
 * @param source the source of the attack
 * @param attack the attack that was just successful
 * @param life_points the number of life points lost by this enemy
 */
void Enemy::just_hurt(MapEntity *source, EnemyAttack attack, int life_points) {

}

/**
 * Kills the enemy.
 * This function is called when the enemy has no more health points.
 */
void Enemy::kill(void) {

  // if the enemy is immobilized, give a rupee
  if (rank == RANK_NORMAL && is_immobilized() && pickable_item_savegame_variable == -1) {
    pickable_item_subtype = get_random_rupee();
  }

  // stop any movement and disable attacks
  set_collision_modes(COLLISION_NONE);
  if (pushed_back_when_hurt) {
    delete normal_movement;
  }
  clear_movement();
  invulnerable = true;
  can_attack = false;

  if (rank == RANK_NORMAL) {
    // replace the enemy sprite
    remove_sprites();
    create_sprite("enemies/enemy_killed");
    ResourceManager::get_sound("enemy_killed")->play();
  }
  else {
    // create some explosions
    exploding = true;
    nb_explosions = 0;
    next_explosion_date = System::now() + 2000;
  }

  // save the enemy state if required
  if (savegame_variable != -1) {
    game->get_savegame()->set_boolean(savegame_variable, true);
  }
}

/**
 * Returns whether the enemy is being hurt.
 * @return true if the enemy is being hurt
 */
bool Enemy::is_being_hurt(void) {
  return being_hurt;
}

/**
 * Returns whether the enemy is killed.
 * @return true if the enemy is killed
 */
bool Enemy::is_killed(void) {
  return life <= 0 && (get_sprite()->get_animation_set_id() == "enemies/enemy_killed" || !exploding);
}

/**
 * When the enemy is killed, returns whether the dying animation is finished.
 * @return true if the dying animation is finished
 */
bool Enemy::is_dying_animation_finished(void) {
  
  if (rank == RANK_NORMAL) {
    return get_sprite()->is_animation_finished();
  }

  return nb_explosions > 0 && !exploding;
}

/**
 * Returns whether the enemy is dying, i.e. his life has reached zero and the dying animation is playing.
 * @return true if the enemy is dying
 */
bool Enemy::is_dying(void) {
  return get_life() <= 0;
}

/**
 * Returns true if the current sprite animation is finished or is looping.
 * @return true if the current sprite animation is finished or is looping 
 */
bool Enemy::is_sprite_finished_or_looping(void) {

  Sprite *sprite = get_sprite();
  return sprite->is_animation_finished() || sprite->is_animation_looping();
}

/**
 * Immobilizes this enemy.
 */
void Enemy::immobilize(void) {
  immobilized = true;
  start_shaking_date = System::now() + 5000; 
}

/**
 * Stops immobilizing the enemy.
 */
void Enemy::stop_immobilized(void) {
  immobilized = false;
  end_shaking_date = 0;
  restore_movement();
  restart();
}

/**
 * Returns whether this enemy is immobilized. 
 * @return true if this enemy is immobilized 
 */
bool Enemy::is_immobilized(void) {
  return immobilized;
}

/**
 * This function is called when the enemy is attacked by a custom effect attack.
 * Redefine this function to handle the attack.
 * @param attack the attack
 * @param this_sprite the sprite of this enemy subject to the attack, or NULL
 * if the attack does not come from a pixel-perfect collision test.
 * @return the number of health points lost (can be 0)
 */
int Enemy::custom_attack(EnemyAttack attack, Sprite *this_sprite) {
  DIE("The custom attack for enemy '" << get_name() << "' is not defined");
}

/**
 * Computes randomly a type of pickable rupee.
 * @return a type of pickable rupee
 */
PickableItem::Subtype Enemy::get_random_rupee(void) {

  PickableItem::Subtype rupee;
  int r = Random::get_number(10);

  if (r < 6) {
    rupee = PickableItem::RUPEE_1;
  }
  else if (r < 9) {
    rupee = PickableItem::RUPEE_5;
  }
  else {
    rupee = PickableItem::RUPEE_20;
  }
  return rupee;
}

