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
#include "entities/Door.h"
#include "entities/Hero.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "Equipment.h"
#include "DungeonEquipment.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Savegame.h"

const std::string Door::animations[] = {
  "closed", "small_key", "small_key_block", "big_key", "boss_key", "weak", "very_weak", ""
};

const MessageId key_required_message_ids[] = {
  "", "_small_key_required", "_small_key_required", "_big_key_required", "_boss_key_required"
};

/**
 * Creates a door.
 * @param name name identifying this entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param direction direction of the door
 * @param subtype the subtype of door
 * @param savegame_variable variable where the door's state is saved
 * (can be -1 for the subtype CLOSED)
 */
Door::Door(const std::string &name, Layer layer, int x, int y,
	     int direction, Subtype subtype, int savegame_variable):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  subtype(subtype), savegame_variable(savegame_variable), door_open(true), changing(false) {

  if (subtype == SMALL_KEY_BLOCK) {
    set_size(16, 16);
  }
  else if (direction % 2 == 0) {
    set_size(16, 32);
  }
  else {
    set_size(32, 16);
  }

  if (subtype != WEAK_INVISIBLE) {
    create_sprite("entities/door");
  }

  if (savegame_variable != -1) {
    set_open(zsdx->game->get_savegame()->get_boolean(savegame_variable));
  }
  else {
    set_open(false);
  }
  get_sprite()->set_current_direction(direction);
}

/**
 * Destructor.
 */
Door::~Door(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Door::get_type(void) {
  return DOOR;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool Door::is_obstacle_for(MapEntity *other) {
  return !is_open();
}

/**
 * Returns whether this door is open.
 * @return true if this door is open
 */
bool Door::is_open(void) {
  return door_open;
}

/**
 * Makes the door open or closed.
 * @param door_open true to make it open, false to make it closed
 */
void Door::set_open(bool door_open) {
  this->door_open = door_open;

  if (!door_open) {
    get_sprite()->set_current_animation(animations[subtype]);
  }
}

/**
 * This function is called by the engine when there is a collision with another entity.
 * This is a redefinition of Detector::collision().
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Door::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (!is_open() && entity_overlapping->is_hero() && has_action_key_effect()) {

    Hero *hero = (Hero*) entity_overlapping;
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE) {

      // we show the action icon
      keys_effect->set_action_key_effect(can_open() ? KeysEffect::ACTION_KEY_OPEN : KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * Returns whether the player can use the action key when touching this door.
 * @return true if there is an action key effect for this door
 */
bool Door::has_action_key_effect(void) {
  return requires_small_key() || subtype == BIG_KEY || subtype == BOSS_KEY;
}

/**
 * Returns whether this door must be open with a small key.
 * @return true if this door must be open with a small key
 */
bool Door::requires_small_key(void) {
  return subtype == SMALL_KEY || subtype == SMALL_KEY_BLOCK;
}

/**
 * Returns whether the player has the right key to open this door.
 * If the door cannot be open with a key, false is returned.
 * @return true if the player has the key corresponding to this door
 */
bool Door::can_open(void) {

  Equipment *equipment = zsdx->game->get_equipment();
  DungeonEquipment *dungeon_equipment = zsdx->game->get_dungeon_equipment();

  return (requires_small_key() && equipment->has_small_key())
    || (subtype == BIG_KEY && dungeon_equipment->has_big_key())
    || (subtype == BOSS_KEY && dungeon_equipment->has_boss_key());
}

/**
 * Suspends or resumes the entity.
 * @param suspended true to suspend the entity
 */
void Door::set_suspended(bool suspended) {
  // we never suspend a door to allow the sprite animation even when the camera is moving
}

/**
 * Updates the entity.
 */
void Door::update(void) {
  Detector::update();

  if (!is_suspended() && changing && get_sprite()->is_animation_finished()) {
    changing = false;
    set_open(!is_open());
  }
}

/**
 * Displays the entity on the map.
 */
void Door::display_on_map(void) {
  if (has_sprite() && !is_open()) {
    Detector::display_on_map();
  }
}

/**
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * The hero opens the door if possible, otherwise a message is shown.
 */
void Door::action_key_pressed(void) {

  Hero *hero = zsdx->game->get_hero();
  Equipment *equipment = zsdx->game->get_equipment();

  if (hero->get_state() == Hero::FREE) {
    if (can_open()) {
      ResourceManager::get_sound("door_unlocked")->play();
      ResourceManager::get_sound("door_open")->play();

      if (subtype == SMALL_KEY_BLOCK) {
	set_open(true);
      }
      else {
        get_sprite()->set_current_animation("opening_key");
      }

      if (requires_small_key()) {
	equipment->remove_small_key();
      }
      changing = true;
    }
    else {
      ResourceManager::get_sound("wrong")->play();
      zsdx->game->show_message(key_required_message_ids[subtype]);
    }
  }
}

/**
 * Opens the door.
 * This function can be called only for a door with subtype CLOSED.
 */
void Door::open(void) {

  if (subtype != CLOSED) {
    DIE("This kind of door cannot be open or closed directly");
  }

  if (is_open()) {
    DIE("This door is already open");
  }

  get_sprite()->set_current_animation("opening");
  changing = true;
}

/**
 * Closes the door.
 * This function can be called only for a door with subtype CLOSED.
 */
void Door::close(void) {

  if (subtype != CLOSED) {
    DIE("This kind of door cannot be open or closed directly");
  }

  if (!is_open()) {
    DIE("This door is already closed");
  }

  get_sprite()->set_current_animation("opening");
  changing = true;
}

