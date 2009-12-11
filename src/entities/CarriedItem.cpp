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
#include "entities/CarriedItem.h"
#include "entities/DestructibleItem.h"
#include "entities/Hero.h"
#include "entities/Enemy.h"
#include "entities/Explosion.h"
#include "entities/MapEntities.h"
#include "movements/PixelMovement.h"
#include "movements/FollowMovement.h"
#include "movements/CollisionMovement.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "Map.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"

/**
 * Movement of the item when the hero is lifting it.
 */
static const Rectangle lifting_translations[4][6] = {
  { Rectangle(0, 0), Rectangle(0, 0), Rectangle(-3, -3), Rectangle(-5, -3), Rectangle(-5,-2) },
  { Rectangle(0, 0), Rectangle(0, 0), Rectangle( 0, -1), Rectangle( 0, -1), Rectangle( 0, 0) },
  { Rectangle(0, 0), Rectangle(0, 0), Rectangle( 3, -3), Rectangle( 5, -3), Rectangle( 5,-2) },
  { Rectangle(0, 0), Rectangle(0, 0), Rectangle( 0,-10), Rectangle( 0,-12), Rectangle( 0, 0) },
};

/**
 * Creates a carried item (i.e. an item carried by the hero)
 * based on a destructible item (i.e. the item placed on the map
 * before the hero lifts it).
 * @param hero the hero
 * @param destructible_item a destructible item
 */
CarriedItem::CarriedItem(Hero *hero, DestructibleItem *destructible_item):
  MapEntity(), hero(hero), is_lifting(true), is_throwing(false), is_breaking(false), explosion_date(0) {

  // put the item on the hero's layer
  set_layer(hero->get_layer());

  // align correctly the item with the hero
  int direction = hero->get_animation_direction();
  if (direction % 2 == 0) {
    set_xy(destructible_item->get_x(), hero->get_y());
  }
  else {
    set_xy(hero->get_x(), destructible_item->get_y());
  }
  set_origin(destructible_item->get_origin());
  set_size(destructible_item->get_width(), destructible_item->get_height());

  // create the lift movement and the sprite
  PixelMovement *movement = new PixelMovement(lifting_translations[direction], 6, 100, false, false);
  create_sprite(destructible_item->get_animation_set_id());
  get_sprite()->set_current_animation("stopped");
  set_movement(movement);

  // create the breaking sound
  destruction_sound = destructible_item->get_destruction_sound();

  // create the shadow (not visible yet)
  shadow_sprite = new Sprite("entities/shadow");
  shadow_sprite->set_current_animation("big");

  // damage on enemies
  damage_on_enemies = destructible_item->get_damage_on_enemies();

  // explosion
  if (destructible_item->can_explode()) {
    explosion_date = System::now() + 6000;
  }
}

/**
 * Destructor.
 */
CarriedItem::~CarriedItem(void) {
  delete shadow_sprite;
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType CarriedItem::get_type() {
  return CARRIED_ITEM;
}

/**
 * Returns the damage this item can cause to ennemies.
 * @return the damage on enemies
 */
int CarriedItem::get_damage_on_enemies(void) {
  return damage_on_enemies;
}

/**
 * This function is called when the hero stops walking while
 * carrying the item. The item also stops moving.
 */
void CarriedItem::set_animation_stopped(void) {

  if (!is_lifting && !is_throwing) {
    std::string animation = will_explode_soon() ? "stopped_explosion_soon" : "stopped";
    get_sprite()->set_current_animation(animation);
  }
}

/**
 * This function is called when the hero starts walking while
 * carrying the item. The item moves like him.
 */
void CarriedItem::set_animation_walking(void) {
  if (!is_lifting && !is_throwing) {
    std::string animation = will_explode_soon() ? "walking_explosion_soon" : "walking";
    get_sprite()->set_current_animation(animation);
  }
}

/**
 * Throws the item.
 * @param map the current map (can be different from the map where the item was lifted)
 * @param direction direction where the hero throws the item (0 to 3)
 */
void CarriedItem::throw_item(Map *map, int direction) {

  this->map = map;
  this->throwing_direction = direction;
  is_lifting = false;
  is_throwing = true;

  // play the sound
  ResourceManager::get_sound("throw")->play();

  // stop the sprite animation
  Sprite *sprite = get_sprite();
  sprite->set_current_animation("stopped");

  // set the movement of the item sprite
  set_y(hero->get_y());
  CollisionMovement *movement = new CollisionMovement();
  movement->set_speed(20);
  movement->set_direction(direction * 90);
  clear_movement();
  set_movement(movement);

  this->y_increment = -2;
  this->next_down_date = System::now() + 40;
  this->item_height = 18;
}

/**
 * Returns whether the item is being thrown.
 * @return true if the item is being thrown
 */
bool CarriedItem::is_being_thrown(void) {
  return is_throwing;
}
 
/**
 * Returns whether the item is about to explode.
 * @return true if the item is about to explode 
 */
bool CarriedItem::will_explode_soon(void) {
  return explosion_date != 0 && System::now() >= explosion_date - 1500;
}

/**
 * Destroys the item when it is being thrown.
 */
void CarriedItem::break_item(void) {

  if (is_throwing && throwing_direction != 3) {
    // destroy the item where it is actually displayed
    set_y(get_y() - item_height);
  }

  is_throwing = false;
  is_breaking = true;
  movement->stop();

  if (explosion_date == 0) {
    destruction_sound->play();
    get_sprite()->set_current_animation("destroy");
  }
  else {
    map->get_entities()->add_entity(new Explosion(get_layer(), get_xy(), true));
  }
}

/**
 * Returns whether the item is broken.
 * When this function returns true, the item can be deleted from memory.
 * @return true if the item is broken
 */
bool CarriedItem::is_broken(void) {
  return is_breaking && (get_sprite()->is_animation_finished() || explosion_date != 0);
}

/**
 * This function is called by the map when the game is suspended or resumed.
 * This is a redefinition of MapEntity::set_suspended() to suspend the movement
 * of the shadow when the item is being thrown.
 * @param suspended true to suspend the entity, false to resume it
 */
void CarriedItem::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement

  if (is_throwing) {
    // suspend the shadow
    shadow_sprite->set_suspended(suspended);
  }

  if (!suspended && when_suspended != 0) {
    // recalculate the timers
    if (is_throwing) {
      next_down_date += System::now() - when_suspended;
    }
    if (explosion_date != 0) {
      explosion_date += System::now() - when_suspended;
    }
  }
}

/**
 * This function is called repeatedly by the hero.
 * When the lift movement finishes, the hero starts carrying the item.
 */
void CarriedItem::update(void) {

  // update the sprite and the position
  MapEntity::update();

  if (suspended) {
    return;
  }

  // when the hero finishes lifting the item, start carrying it
  if (is_lifting && get_movement()->is_finished()) {
    is_lifting = false;

    // make the hero carry the item
    hero->start_carrying();

    // make the item follow the hero
    clear_movement();
    set_movement(new FollowMovement(hero, 0, -18, false));
  }

  // when the item has finished flying, destroy it
  else if (explosion_date != 0 && !is_breaking) {
    
    uint32_t now = System::now();
    
    if (now >= explosion_date) {
      break_item();
    }
    else if (will_explode_soon()) {

      std::string animation = get_sprite()->get_current_animation();
      if (animation == "stopped") {
	get_sprite()->set_current_animation("stopped_explosion_soon");
      }
      else if (animation == "walking") {
	get_sprite()->set_current_animation("walking_explosion_soon");
      }
    }
  }

  if (is_throwing) {
    shadow_sprite->update();

    if (movement->is_stopped() || y_increment >= 7) {
      break_item();
    }
    else {
      uint32_t now = System::now();
      while (now >= next_down_date) {
	next_down_date += 40;
	item_height -= y_increment;
	y_increment++;
      }
    }
  }
}

/**
 * Displays the carried item on the map.
 * This is a redefinition of MapEntity::display_on_map
 * to display the shadow independently of the item movement.
 */
void CarriedItem::display_on_map(void) {

  if (!is_throwing) {
    // display the sprite normally
    MapEntity::display_on_map();
  }
  else {
    // when the item is being thrown, display the shadow and the item separately
    map->display_sprite(shadow_sprite, get_x(), get_y());
    map->display_sprite(get_sprite(), get_x(), get_y() - item_height);
  }
}

/**
 * This function is called when this carried item collides an enemy.
 * @param enemy the enemy
 */
void CarriedItem::notify_collision_with_enemy(Enemy *enemy) {

  if (is_throwing && explosion_date == 0) {
    enemy->try_hurt(ATTACK_THROWN_ITEM, this, NULL);
  }
}

/**
 * Notifies this entity that it has just attacked an enemy
 * (even if this attack was not successful).
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack:
 * - a number greater than 0 represents the number of health points the enemy has just lost
 * - a value of 0 means that the attack was just ignored 
 * - a value of -1 means that the enemy was protected against the attack
 * - a value of -2 means that the attack immobilized the enemy
 */
void CarriedItem::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result) {
  if (result != 0) {
    break_item();
  }
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool CarriedItem::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return false;
}

/**
 * Returns whether a conveyor belt is currently considered as an obstacle for this entity.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool CarriedItem::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return false;
}

/**
 * Returns whether a water tile is currently considered as an obstacle for this entity.
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool CarriedItem::is_water_obstacle(void) {
  return false;
}

/**
 * Returns whether a hole is currently considered as an obstacle for this entity.
 * @return true if the holes are currently an obstacle for this entity
 */
bool CarriedItem::is_hole_obstacle(void) {
  return false;
}

/**
 * Returns whether a ladder is currently considered as an obstacle for this entity.
 * @return true if the ladders are currently an obstacle for this entity
 */
bool CarriedItem::is_ladder_obstacle(void) {
  return false;
}

/**
 * Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * @param raised_block a crystal switch block raised
 * @return false 
 */
bool CarriedItem::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {
  // the thrown items can traverse the crystal switch blocks
  return false;
}

/**
 * Returns whether a crystal switch is currently considered as an obstacle for this entity.
 * @param crystal_switch a crystal switch
 * @return true if the crystal switch is currently an obstacle for this entity
 */
bool CarriedItem::is_crystal_switch_obstacle(CrystalSwitch *crystal_switch) {
  return !is_being_thrown();
}

/**
 * Returns whether a non-playing character is currently considered as an obstacle for this entity.
 * @param npc a non-playing character
 * @return true if the NPC is currently an obstacle for this entity
 */
bool CarriedItem::is_npc_obstacle(InteractiveEntity *npc) {
  return false;
}

/**
 * Returns whether an enemy character is considered as an obstacle for this entity.
 * @param enemy an enemy
 * @return true if this enemy is considered as an obstacle for this entity.
 */
bool CarriedItem::is_enemy_obstacle(Enemy *enemy) {
  // if this item explodes when reaching an obstacle, then we consider enemies as obstacles
  return explosion_date != 0;
}

