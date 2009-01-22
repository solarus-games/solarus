#include "entities/Enemy.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "enemies/SimpleGreenSoldier.h"

/**
 * Creates an enemy.
 * This constructor can only be called by the subclasses. Use the static method create() to create enemies.
 * @param params the name and position of the enemy
 */
Enemy::Enemy(const ConstructionParameters &params):
  Detector(COLLISION_RECTANGLE | COLLISION_SPRITE, params.name, params.layer, params.x, params.y, 0, 0) {

}

/**
 * Destructor.
 */
Enemy::~Enemy(void) {

}

/**
 * Creates an enemy with the specified type.
 * This method acts like a constructor, except that it can return NULL if the enemy
 * is already dead and cannot be killed again (e.g. a boss).
 * @param type type of enemy to create
 * @param name a name identifying the enemy
 * @param rank rank of the enemy: normal, miniboss or boss
 * @param savegame_variable index of the boolean variable indicating that
 * @param layer layer of the enemy on the map
 * @param x x position of the enemy on the map
 * @param y y position of the enemy on the map
 * @param direction initial direction of the enemy on the map (0 to 3)
 * this enemy is killed, or -1 if this enemy is not saved
 * @param pickable_item_type type of pickable item the enemy drops
 * @param pickable_item_savegame_variable index of the boolean variable
 */
Enemy * Enemy::create(EnemyType type, Rank rank, int savegame_variable,
		      string name, Layer layer, int x, int y, int direction,
		      PickableItem::ItemType pickable_item_type, int pickable_item_savegame_variable) {

  // see if the enemy is alive
  if (savegame_variable != -1 && zsdx->game->get_savegame()->get_boolean(savegame_variable)) {
    return NULL;
  }

  // create the enemy
  Enemy *enemy;
  const ConstructionParameters params = {name, layer, x, y};

  switch (type) {
    
  case SIMPLE_GREEN_SOLDIER:
    enemy = new SimpleGreenSoldier(params);
    break;

  default:
    DIE("Unknown enemy type '" << type << "'");
  }

  // initialize the fields
  enemy->set_direction(direction);
  enemy->rank = rank;
  enemy->savegame_variable = savegame_variable;
  enemy->pickable_item_type = pickable_item_type;
  enemy->pickable_item_savegame_variable = pickable_item_savegame_variable;

  // set the default properties
  enemy->damage_on_hero = 1;
  enemy->life = 1;
  enemy->hurt_sound_style = HURT_SOUND_NORMAL;
  enemy->pushed_back_when_hurt = true;
  enemy->push_back_hero_on_sword = false;
  enemy->minimum_shield_needed = 0;

  for (int i = 0; i < ATTACK_NUMBER; i++) {
    enemy->vulnerabilities[i] = 1;
  }
  enemy->vulnerabilities[ATTACK_HOOKSHOT] = -2;
  enemy->vulnerabilities[ATTACK_BOOMERANG] = -2;

  return enemy;
}

/**
 * Sets the map.
 * @param map the map
 */
void Enemy::set_map(Map *map) {

  MapEntity::set_map(map);

  // let the subclass initialize the enemy
  initialize();

  for (unsigned int i = 0; i < sprites.size(); i++) {
    get_sprite(i)->get_animation_set()->enable_pixel_collisions();
  }
}

/**
 * Sets the amount of damage this kind of enemy can make to the hero
 * @param damage_on_hero number of heart quarters the player loses
 */
void Enemy::set_damage(int damage_on_hero) {
  this->damage_on_hero = damage_on_hero;
}

/**
 * Sets the number of health points of the enemy.
 * @param life number of health points of the enemy
 */
void Enemy::set_life(int life) {
  this->life = life;
}

/**
 * Sets some properties of this type of enemy.
 * @param damage_on_hero number of heart quarters the player loses
 * @param life number of health points of the enemy
 */
void Enemy::set_properties(int damage_on_hero, int life) {
  set_damage(damage_on_hero);
  set_life(life);
}

/**
 * Sets some properties of this type of enemy.
 * @param damage_on_hero number of heart quarters the player loses
 * @param life number of health points of the enemy
 * @param hurt_sound_style the sound played when this kind of enemy gets hurt by the hero
 */
void Enemy::set_properties(int damage_on_hero, int life, HurtSoundStyle hurt_sound_style) {
  set_properties(damage_on_hero, life);
  this->hurt_sound_style = hurt_sound_style;
}

/**
 * Sets all properties of this type of enemy.
 * @param damage_on_hero number of heart quarters the player loses
 * @param life number of health points of the enemy
 * @param hurt_sound_style the sound played when this kind of enemy gets hurt by the hero
 * @param pushed_back_when_hurt indicates whether the enemy is pushed back when it gets hurt by the hero
 * @param push_back_hero_on_sword indicates whether the hero is pushed back when he hurts the enemy with his sword
 * @param minimum_shield_needed shield number needed by the hero to avoid the attack of this enemy,
 * or 0 to make the attack unavoidable
 */
void Enemy::set_properties(int damage_on_hero, int life, HurtSoundStyle hurt_sound_style,
			   bool pushed_back_when_hurt, bool push_back_hero_on_sword, int minimum_shield_needed) {
  set_properties(damage_on_hero, life, hurt_sound_style);
  this->pushed_back_when_hurt = pushed_back_when_hurt;
  this->push_back_hero_on_sword = push_back_hero_on_sword;
  this->minimum_shield_needed = minimum_shield_needed;
}

/**
 * Sets whether the enemy is vulnerable to a specified attack.
 * @param attack an attack
 * @param reaction how the enemy will react
 */
void Enemy::set_vulnerability(Attack attack, int reaction) {
  vulnerabilities[attack] = reaction;
}

/**
 * Updates the enemy.
 */
void Enemy::update(void) {
  MapEntity::update();
}

/**
 * Suspends or resumes the enemy.
 */
void Enemy::set_suspended(bool suspended) {
  MapEntity::set_suspended(suspended);
}

/**
 * This function is called when the enemy collides with another entity.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void Enemy::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero()) {
    //    std::cout << "Link is hurt!\n";
  }
}

/**
 * This function is called when the enemy's sprite collides with another
 * entity's sprite.
 * @param entity_overlapping the other entity
 * @param sprite_overlapping the sprite of this entity that is overlapping the enemy
 */
void Enemy::collision(MapEntity *entity, Sprite *sprite_overlapping) {
  
  if (entity->is_hero() && sprite_overlapping->get_animation_set_id().find("sword") != string::npos) {
    //    std::cout << "The enemy is hurt!\n";
  }
}
