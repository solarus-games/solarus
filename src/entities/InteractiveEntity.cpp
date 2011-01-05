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
#include "entities/CarriedItem.h"
#include "movements/Movement.h"
#include "lua/MapScript.h"
#include "lua/ItemScript.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"
#include "Game.h"
#include "DialogBox.h"
#include "Map.h"
#include "Sprite.h"
#include "Equipment.h"
#include "InventoryItem.h"
#include "ItemProperties.h"

/**
 * @brief Creates an interactive entity.
 * @param name name identifying this interactive entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param subtype the subtype of interaction
 * @param y y coordinate of the entity to create
 * @param sprite_name sprite animation set of the entity, or "_none" to create no sprite
 * @param direction for a custom interactive entity: direction for which the interactions are allowed (0 to 4, or -1 for any direction),
 * for an NPC: initial direction of the NPC's sprite
 * @param behavior_string indicates what happens when the hero interacts with this entity:
 * "message#XXX" to start the dialog XXX, "map" to call the map script (with an event_hero_interaction() call)
 * or "item#XXX" to call the script of item XXX  (with an event_hero_interaction() call)
 */
InteractiveEntity::InteractiveEntity(Game &game, const std::string &name, Layer layer, int x, int y,
				     Subtype subtype, SpriteAnimationSetId sprite_name,
				     int direction, const std::string &behavior_string):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 0, 0),
  subtype(subtype),
  message_to_show(""),
  script_to_call(NULL) {

  switch (subtype) {

  case CUSTOM:
    initialize_sprite(sprite_name, 0);
    set_size(16, 16);
    set_origin(8, 13);
    set_direction(direction);
    break;

  case NON_PLAYING_CHARACTER:
    initialize_sprite(sprite_name, direction);
    set_size(16, 16);
    set_origin(8, 13);
    break;
  }

  // behavior
  if (behavior_string == "map") {
    behavior = BEHAVIOR_MAP_SCRIPT;
    script_to_call = NULL; // the map script may be not available yet
  }
  else if (behavior_string.substr(0, 5) == "item#") {
    behavior = BEHAVIOR_ITEM_SCRIPT;
    const std::string &item_name = behavior_string.substr(5);
    script_to_call = &game.get_equipment().get_item_script(item_name);
  }
  else if (behavior_string.substr(0, 7) == "dialog#") {
    behavior = BEHAVIOR_DIALOG;
    message_to_show = behavior_string.substr(7);
  }
  else {
    Debug::die(StringConcat() << "Invalid behavior string for interactive entity '" << name
	<< "': '" << behavior_string << "'");
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
MapEntity* InteractiveEntity::parse(Game &game, std::istream &is, Layer layer, int x, int y) {

  int direction, subtype;
  std::string name;
  SpriteAnimationSetId sprite_name;
  std::string behavior;

  FileTools::read(is, name);
  FileTools::read(is, direction);
  FileTools::read(is, subtype);
  FileTools::read(is, sprite_name);
  FileTools::read(is, behavior);

  return new InteractiveEntity(game, name, Layer(layer), x, y, Subtype(subtype), sprite_name, direction, behavior);
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
void InteractiveEntity::initialize_sprite(SpriteAnimationSetId &sprite_name, int initial_direction) {

  if (sprite_name != "_none") {
    create_sprite(sprite_name);
    get_sprite().set_current_direction(initial_direction);
  }
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool InteractiveEntity::is_obstacle_for(MapEntity &other) {

  if (subtype == NON_PLAYING_CHARACTER) {
    // only NPCs have specific collisions
    return other.is_npc_obstacle(*this);
  }

  // other interactive entities are always obstacles
  return true;
}

/**
 * @brief Returns whether the hero is currently considered as an obstacle by this entity.
 * @param hero the hero
 * @return true if the hero is an obstacle for this entity.
 */
bool InteractiveEntity::is_hero_obstacle(Hero &hero) {
  return true;
}

/**
 * @brief Returns whether a non-playing character is currently considered as an obstacle by this entity.
 * @param npc a non-playing character
 * @return true if this NPC is currently considered as an obstacle by this entity.
 */
bool InteractiveEntity::is_npc_obstacle(InteractiveEntity &npc) {
  return true;
}

/**
 * @brief Returns whether an enemy character is currently considered as an obstacle by this entity.
 * @param enemy an enemy
 * @return true if this enemy is currently considered as an obstacle by this entity.
 */
bool InteractiveEntity::is_enemy_obstacle(Enemy &enemy) {
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
void InteractiveEntity::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping.is_hero()) {

    Hero &hero = (Hero&) entity_overlapping;

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero.is_free()) {

      if (subtype == NON_PLAYING_CHARACTER
	  || get_direction() == -1
	  || hero.is_facing_direction4((get_direction() + 2) % 4)) {

	// show the appropriate action icon
	get_keys_effect().set_action_key_effect(subtype == NON_PLAYING_CHARACTER ?
	    KeysEffect::ACTION_KEY_SPEAK : KeysEffect::ACTION_KEY_LOOK);
      }
      else if (can_be_lifted()) {
	get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LIFT);
      }
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

  Hero& hero = get_hero();
  if (hero.is_free()) {

    KeysEffect::ActionKeyEffect effect = get_keys_effect().get_action_key_effect();
    get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

    // if this is an NPC, look towards the hero
    if (subtype == NON_PLAYING_CHARACTER) {
      int direction = (get_hero().get_animation_direction() + 2) % 4;
      get_sprite().set_current_direction(direction);
    }

    if (effect != KeysEffect::ACTION_KEY_LIFT) {
      // start the normal behavior
      if (behavior == BEHAVIOR_DIALOG) {
	get_dialog_box().start_dialog(message_to_show);
      }
      else {
	call_script();
      }
    }
    else {
      // lift the entity
      if (get_equipment().has_ability("lift")) {

	hero.start_lifting(new CarriedItem(hero, get_xy(), get_size(), get_origin(),
	    get_sprite().get_animation_set_id(), "stone", 2, false));
	Sound::play("lift");
	remove_from_map();
      }
    }
  }
}

/**
 * @brief Notifies the appropriate script that the player is interacting with this entity.
 */
void InteractiveEntity::call_script() {

  if (behavior == BEHAVIOR_MAP_SCRIPT && script_to_call == NULL) { // first time
    script_to_call = &get_map_script();
  }

  if (subtype == NON_PLAYING_CHARACTER) {
    script_to_call->event_npc_dialog(get_name());
  }
  else {
    script_to_call->event_hero_interaction(get_name());
    get_map_script().event_hero_interaction_finished(get_name()); // always notify the map script when finished
  }
}

/**
 * @brief Notifies this detector that the player is interacting by using an inventory item.
 *
 * This function is called when the player uses an inventory item
 * while the hero is facing this interactive entity.
 *
 * @param item the inventory item used
 * @return true if an interaction occured
 */
bool InteractiveEntity::interaction_with_inventory_item(InventoryItem &item) {

  bool interaction_occured;
  Script *script = behavior == BEHAVIOR_ITEM_SCRIPT ? script_to_call : &get_map_script();
  if (subtype == NON_PLAYING_CHARACTER) {
    interaction_occured = script->event_npc_dialog_item(get_name(), item.get_name(), item.get_variant());
  }
  else {
    interaction_occured = script->event_hero_interaction_item(get_name(), item.get_name(), item.get_variant());
  }

  if (interaction_occured) {
    // always notify the map script when finished
    get_map_script().event_hero_interaction_item_finished(get_name(), item.get_name(), item.get_variant());
  }
  return interaction_occured;
}

/**
 * @brief Updates the entity.
 */
void InteractiveEntity::update() {

  Detector::update();

  if (subtype == NON_PLAYING_CHARACTER && get_movement() != NULL) {

    if (get_movement()->is_finished()) {
      get_sprite().set_current_animation("stopped");
      clear_movement();
      get_map_script().event_npc_movement_finished(get_name());
    }
  }
}

/**
 * @brief This function is called when the entity has just moved.
 *
 * If it is an NPC, its sprite's direction is updated.
 */
void InteractiveEntity::notify_position_changed() {

  if (subtype == NON_PLAYING_CHARACTER) {

    if (get_sprite().get_current_animation() == "walking") {
      int direction4 = get_movement()->get_displayed_direction4();
      get_sprite().set_current_direction(direction4);
    }

    if (get_hero().get_facing_entity() == this &&
	get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_SPEAK &&
	!get_hero().is_facing_point_in(get_bounding_box())) {

      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    }
  }
}

/**
 * @brief Returns whether this interactive entity can be lifted.
 */
bool InteractiveEntity::can_be_lifted() {

  // there is currently no way to specify from the data file of the map
  // that an interactive entity can be lifted (nor its weight, damage, sound…) so this is hardcoded
  // TODO: specify the possibility to lift and the weight from the map script?
  return has_sprite() && get_sprite().get_animation_set_id() == "entities/sign";
}

