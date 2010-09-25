/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "entities/InteractiveEntity.h"
#include "entities/Hero.h"
#include "movements/PathMovement.h"
#include "movements/RandomWalkMovement.h"
#include "movements/JumpMovement.h"
#include "lua/Scripts.h"
#include "Game.h"
#include "DialogBox.h"
#include "Map.h"
#include "Sprite.h"
#include "Equipment.h"
#include "InventoryItem.h"
#include "ItemProperties.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"

/**
 * @brief Action key effect depending on the type of interaction.
 */
const KeysEffect::ActionKeyEffect InteractiveEntity::action_key_effects[] = {

  KeysEffect::ACTION_KEY_LOOK,
  KeysEffect::ACTION_KEY_SPEAK, // NPC
  KeysEffect::ACTION_KEY_LOOK,
  KeysEffect::ACTION_KEY_LOOK,
};

/**
 * @brief Indicates the direction of an NPC's animation (from 0 to 3)
 * depending on the movement direction.
 */
const int InteractiveEntity::animation_directions[] = {
  0, // right
  0,
  1,
  2,
  2,
  2,
  3,
  0
};

/**
 * @brief Creates an interactive entity.
 * @param name name identifying this interactive entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param subtype the subtype of interaction
 * @param y y coordinate of the entity to create
 * @param sprite_name sprite animation set of the entity, or "_none" to create no sprite
 * @param initial_direction direction of the entity's sprite (only used if the entity has a sprite)
 * @param message_to_show id of the message to show when the player presses the action key in front
 * of this entity, or "_none" to call the script instead (with an event_hero_interaction() call)
 */
InteractiveEntity::InteractiveEntity(const std::string &name, Layer layer, int x, int y,
				     Subtype subtype, SpriteAnimationSetId sprite_name,
				     int initial_direction, MessageId message_to_show):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 0, 0),
  subtype(subtype), message_to_show(message_to_show) {

  switch (subtype) {

  case CUSTOM:
    initialize_sprite(sprite_name, 0); // the direction is ignored
    set_size(16, 16);
    set_origin(8, 13);
    break;

  case NON_PLAYING_CHARACTER:
    initialize_sprite(sprite_name, initial_direction);
    set_size(16, 16);
    set_origin(8, 13);
    break;

  case SIGN:
    create_sprite("entities/sign");
    set_size(16, 16);
    set_origin(8, 13);
    break;

  case WATER_FOR_BOTTLE:
    set_size(16, 16);
    break;
  }
}

/**
 * @brief Destructor.
 */
InteractiveEntity::~InteractiveEntity() {

}

/**
 * @brief Creates an instance from an input stream.
 *
 * The input stream must respect the syntax of this entity type.
 *
 * @param game the game that will contain the entity created
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * InteractiveEntity::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  int direction, subtype;
  std::string name;
  SpriteAnimationSetId sprite_name;
  MessageId message_to_show;

  FileTools::read(is, name);
  FileTools::read(is, direction);
  FileTools::read(is, subtype);
  FileTools::read(is, sprite_name);
  FileTools::read(is, message_to_show);

  return new InteractiveEntity(name, Layer(layer), x, y, Subtype(subtype), sprite_name, direction, message_to_show);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType InteractiveEntity::get_type() {
  return INTERACTIVE_ENTITY;
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * This function returns whether an entity of this type should be displayed above
 * the hero and other entities when it is in front of them.
 *
 * @return true if this entity is displayed at the same level as the hero
 */
bool InteractiveEntity::is_displayed_in_y_order() {
  return subtype == NON_PLAYING_CHARACTER;
}

/**
 * @brief Creates the sprite specified.
 * @param sprite_name sprite animation set of the entity, or "_none" to create no sprite
 * @param initial_direction direction of the entity's sprite (ignored if there is no sprite)
 */
void InteractiveEntity::initialize_sprite(SpriteAnimationSetId sprite_name, int initial_direction) {

  if (sprite_name != "_none") {
    create_sprite(sprite_name);
    get_sprite()->set_current_direction(initial_direction);
  }
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool InteractiveEntity::is_obstacle_for(MapEntity *other) {

  if (subtype == NON_PLAYING_CHARACTER) {
    // only NPCs have specific collisions
    return other->is_npc_obstacle(this);
  }

  // other interactive entities are always obstacles
  return true;
}

/**
 * @brief Returns whether the hero is currently considered as an obstacle by this entity.
 * @param hero the hero
 * @return true if the hero is an obstacle for this entity.
 */
bool InteractiveEntity::is_hero_obstacle(Hero *hero) {
  return true;
}

/**
 * @brief Returns whether a non-playing character is currently considered as an obstacle by this entity.
 * @param npc a non-playing character
 * @return true if this NPC is currently considered as an obstacle by this entity.
 */
bool InteractiveEntity::is_npc_obstacle(InteractiveEntity *npc) {
  return subtype != NON_PLAYING_CHARACTER;
}

/**
 * @brief Returns whether an enemy character is currently considered as an obstacle by this entity.
 * @param enemy an enemy
 * @return true if this enemy is currently considered as an obstacle by this entity.
 */
bool InteractiveEntity::is_enemy_obstacle(Enemy *enemy) {
  return subtype != NON_PLAYING_CHARACTER;
}

/**
 * @brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero hits this entity with the sword.
 *
 * @return true if the sword is ignored
 */
bool InteractiveEntity::is_sword_ignored() {
  return subtype == NON_PLAYING_CHARACTER;
}

/**
 * @brief This function is called by the engine when there is a collision with another entity.
 *
 * If the entity is the hero, we allow him to interact with this entity.
 *
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void InteractiveEntity::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero()) {

    Hero *hero = (Hero*) entity_overlapping;
    KeysEffect *keys_effect = game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->is_free()
	&& (subtype != SIGN || hero->is_facing_direction4(1))) { // TODO move to future class Sign

      // we show the action icon
      keys_effect->set_action_key_effect(action_key_effects[subtype]);
    }
  }
}

/**
 * @brief Notifies this detector that the player is interacting by pressing the action key.
 *
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 */
void InteractiveEntity::action_key_pressed() {

  KeysEffect *keys_effect = game->get_keys_effect();
  Hero *hero = game->get_hero();

  if (hero->is_free()) {
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

    // for a place with water: start the dialog
    if (subtype == WATER_FOR_BOTTLE) {
      // TODO game->get_equipment()->found_water();
    }
    else {

      // for an NPC: look in the hero's direction 
      if (subtype == NON_PLAYING_CHARACTER) {
	int direction = (hero->get_animation_direction() + 2) % 4;
	get_sprite()->set_current_direction(direction);
      }

      // start the message or call the script
      if (message_to_show != "_none") {
	game->get_dialog_box()->start_dialog(message_to_show);
      }
      else {
	// there is no message specified: we call the script
	call_script();
      }
    }
  } 
}

/**
 * @brief Notifies this detector that the player is interacting by using an inventory item.
 *
 * This function is called when the player uses an inventory item
 * while the hero is facing this interactive entity.
 * The exact conditions where this function is called depend on the type of inventory item.
 * For some items, the function is called as soon as the player uses the item.
 * For others, it is called after the player confirms the action in a dialog box.
 *
 * @param item the inventory item used
 * @return true if an interaction occured
 */
bool InteractiveEntity::interaction_with_inventory_item(InventoryItem *item) {

  bool interaction = false;

  // if the player uses an empty bottle on a place with water, we let him fill the bottle
  if (subtype == WATER_FOR_BOTTLE
      && item->get_name().substr(0, 7) == "bottle_" // TODO make a script
      && item->get_variant() == 1) {

    // TODO game->get_equipment()->found_water();
    interaction = true;
  }
  else {
    // in other cases, nothing is predefined in the engine: we call the script
    interaction = game->get_scripts().event_hero_interaction_item(get_name(), item->get_name(), item->get_variant());
  }

  return interaction;
}

/**
 * @brief Notifies the script that the player is interacting with this entity.
 */
void InteractiveEntity::call_script() {

  if (subtype == NON_PLAYING_CHARACTER) {
    map->get_scripts().event_npc_dialog(get_name());
  }
  else {
    map->get_scripts().event_hero_interaction(get_name());
  }
}

/**
 * @brief Updates the entity.
 */
void InteractiveEntity::update() {

  Detector::update();

  if (subtype == NON_PLAYING_CHARACTER && get_movement() != NULL) {

    if (get_movement()->is_finished()) {
      get_sprite()->set_current_animation("stopped");
      clear_movement();
      map->get_scripts().event_npc_movement_finished(get_name());
    }
  }
}

/**
 * @brief Makes the entity walk (only for an NPC).
 *
 * The NPC's sprite must have an animation "walking".
 *
 * @param path the path to follow (see class PathMovement)
 * @param loop true to make the movement loop
 * @param ignore_obstacles true to make the movement sensitive to obstacles
 */
void InteractiveEntity::walk(std::string path, bool loop, bool ignore_obstacles) {

  Debug::assert(subtype == NON_PLAYING_CHARACTER, "This entity is not a non-playing character");

  clear_movement();
  set_movement(new PathMovement(path, 6, loop, ignore_obstacles, false));
  get_sprite()->set_current_animation("walking");
}

/**
 * @brief Makes the entity walk randomly (only for an NPC).
 *
 * The NPC's sprite must have an animation "walking".
 */
void InteractiveEntity::walk_random() {

  Debug::assert(subtype == NON_PLAYING_CHARACTER, "This entity is not a non-playing character");

  clear_movement();
  set_movement(new RandomWalkMovement(3));
  get_sprite()->set_current_animation("walking");
}

/**
 * @brief Makes the entity jump into a direction (only for an NPC).
 *
 * The NPC's sprite must have an animation "jumping".
 *
 * @param direction direction of the movement (0 to 7)
 * @param length length of the jump in pixels
 * @param ignore_obstacles true to make the movement sensitive to obstacles
 */
void InteractiveEntity::jump(int direction, int length, bool ignore_obstacles) {

  Debug::assert(subtype == NON_PLAYING_CHARACTER, "This entity is not a non-playing character");

  get_sprite()->set_current_animation("jumping");
  clear_movement();
  JumpMovement *movement = new JumpMovement(direction, length, ignore_obstacles);
  movement->set_delay(20);
  set_movement(movement);
}

/**
 * @brief This function is called when the entity has just moved.
 *
 * If it is an NPC, its sprite's direction is updated.
 */
void InteractiveEntity::notify_position_changed() {

  if (subtype == NON_PLAYING_CHARACTER) {

    if (get_sprite()->get_current_animation() == "walking") {
      PathMovement *movement = (PathMovement*) get_movement();
      int movement_direction = movement->get_current_direction();
      get_sprite()->set_current_direction(animation_directions[movement_direction]);
    }

    Hero *hero = game->get_hero();
    KeysEffect *keys_effect = game->get_keys_effect();
    if (hero->get_facing_entity() == this &&
	keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_SPEAK &&
	!hero->is_facing_point_in(get_bounding_box())) {

      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    }
  }
}

/**
 * @brief Sets the direction of the entity's sprite.
 * @param direction a direction between 0 and 3
 */
void InteractiveEntity::set_sprite_direction(int direction) {
  get_sprite()->set_current_direction(direction);
}

/**
 * @brief Displays the entity on the map.
 *
 * This is a redefinition of MapEntity::display_on_map() to handle the special
 * display when the entity is jumping.
 */
void InteractiveEntity::display_on_map() {

  if (subtype == NON_PLAYING_CHARACTER &&
      get_sprite()->get_current_animation() == "jumping") {

    int jump_height = ((JumpMovement*) get_movement())->get_jump_height();
    map->display_sprite(get_sprite(), get_x(), get_y() - jump_height);
  }
  else {
    MapEntity::display_on_map();
  }
}

