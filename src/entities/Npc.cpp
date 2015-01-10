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
#include "solarus/entities/Npc.h"
#include "solarus/entities/CarriedItem.h"
#include "solarus/entities/Hero.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/ScopedLuaRef.h"
#include "solarus/movements/Movement.h"
#include "solarus/Equipment.h"
#include "solarus/EquipmentItemUsage.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/Sprite.h"
#include <lua.hpp>

namespace Solarus {

/**
 * \brief Creates an NPC.
 * \param game the game
 * \param name name identifying this NPC
 * \param layer layer of the entity to create
 * \param x x coordinate of the entity to create
 * \param subtype the subtype of interaction
 * \param y y coordinate of the entity to create
 * \param sprite_name sprite animation set of the entity, or an empty string to create no sprite
 * \param direction for a generalized NPC: direction for which the interactions are allowed
 * (0 to 4, or -1 for any direction), for a usual NPC: initial direction of the NPC's sprite
 * \param behavior_string indicates what happens when the hero interacts with this NPC:
 * "dialog#XXX" to start the dialog XXX, "map" to call the map script
 * (with an event_hero_interaction() call) or "item#XXX" to call the script
 * of item XXX  (with an event_hero_interaction() call)
 */
Npc::Npc(
    Game& /* game */,
    const std::string& name,
    Layer layer,
    const Point& xy,
    Subtype subtype,
    const std::string& sprite_name,
    int direction,
    const std::string& behavior_string
):
  Detector(COLLISION_FACING | COLLISION_OVERLAPPING, name, layer, xy, Size(0, 0)),
  subtype(subtype),
  dialog_to_show(""),
  item_name("") {

  initialize_sprite(sprite_name, direction);
  set_size(16, 16);
  set_origin(8, 13);
  set_direction(direction);

  // Usual NPCs are displayed like the hero whereas generalized NPCs are
  // not necessarily people.
  set_drawn_in_y_order(subtype == USUAL_NPC);

  // behavior
  if (behavior_string == "map") {
    behavior = BEHAVIOR_MAP_SCRIPT;
  }
  else if (behavior_string.substr(0, 5) == "item#") {
    behavior = BEHAVIOR_ITEM_SCRIPT;
    item_name = behavior_string.substr(5);
  }
  else if (behavior_string.substr(0, 7) == "dialog#") {
    behavior = BEHAVIOR_DIALOG;
    dialog_to_show = behavior_string.substr(7);
  }
  else {
    Debug::die(std::string("Invalid behavior string for NPC '") + name
        + "': '" + behavior_string + "'");
  }
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Npc::get_type() const {
  return EntityType::NPC;
}

/**
 * \brief Creates the sprite specified.
 * \param sprite_name sprite animation set of the entity, or an empty string to create no sprite
 * \param initial_direction direction of the entity's sprite (ignored if there is no sprite
 * of if the direction specified is -1)
 */
void Npc::initialize_sprite(const std::string& sprite_name, int initial_direction) {

  if (!sprite_name.empty()) {
    create_sprite(sprite_name);
    if (initial_direction != -1) {
      get_sprite().set_current_direction(initial_direction);
    }
  }
}

/**
 * \brief Returns whether this NPC is a solid, non-traversable object.
 *
 * This function can be called by other entities who want to be able to
 * traverse people (usual NPCs) but not solid interactive entities
 * (generalized NPCs).
 *
 * \return true if the NPC is a solid object
 */
bool Npc::is_solid() const {

  return subtype != USUAL_NPC;
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other another entity
 * \return true
 */
bool Npc::is_obstacle_for(MapEntity& other) {

  return other.is_npc_obstacle(*this);
}

/**
 * \brief Returns whether the hero is currently considered as an obstacle by this entity.
 * \param hero the hero
 * \return true if the hero is an obstacle for this entity
 */
bool Npc::is_hero_obstacle(Hero& /* hero */) {
  return true;
}

/**
 * \brief Returns whether an NPC is currently considered as an obstacle by this entity.
 * \param npc an NPC
 * \return true if this NPC is currently considered as an obstacle by this entity
 */
bool Npc::is_npc_obstacle(Npc& npc) {

  // usual NPCs can traverse each other
  return subtype != USUAL_NPC || npc.subtype != USUAL_NPC;
}

/**
 * \brief Returns whether an enemy character is currently considered as an obstacle by this entity.
 * \param enemy an enemy
 * \return true if this enemy is currently considered as an obstacle by this entity
 */
bool Npc::is_enemy_obstacle(Enemy& /* enemy */) {

  // usual NPCs can traverse enemies
  return subtype != USUAL_NPC;
}

/**
 * \brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero hits this entity with the sword.
 *
 * \return true if the sword is ignored
 */
bool Npc::is_sword_ignored() const {

  // usual NPCs ignore the sword (we don't want a sword tapping sound with them)
  return subtype == USUAL_NPC;
}

/**
 * \brief This function is called by the engine when there is a collision with another entity.
 *
 * If the entity is the hero, we allow him to interact with this entity.
 *
 * \param entity_overlapping the entity overlapping the detector
 * \param collision_mode the collision mode that detected the collision
 */
void Npc::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_FACING && entity_overlapping.is_hero()) {

    Hero& hero = static_cast<Hero&>(entity_overlapping);

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
        && hero.is_free()) {

      if (subtype == USUAL_NPC // the hero can talk to usual NPCs from any direction
          || get_direction() == -1
          || hero.is_facing_direction4((get_direction() + 2) % 4)) {

        // show the appropriate action icon
        get_keys_effect().set_action_key_effect(subtype == USUAL_NPC ?
            KeysEffect::ACTION_KEY_SPEAK : KeysEffect::ACTION_KEY_LOOK);
      }
      else if (can_be_lifted() && get_equipment().has_ability(Ability::LIFT)) {
        get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LIFT);
      }
    }
  }
  else if (collision_mode == COLLISION_OVERLAPPING && entity_overlapping.get_type() == EntityType::FIRE) {

    if (behavior == BEHAVIOR_ITEM_SCRIPT) {
      EquipmentItem& item = get_equipment().get_item(item_name);
      get_lua_context().item_on_npc_collision_fire(item, *this);
    }
    else {
      get_lua_context().npc_on_collision_fire(*this);
    }
  }
}

/**
 * \copydoc Detector::notify_action_command_pressed
 */
bool Npc::notify_action_command_pressed() {

  Hero& hero = get_hero();
  if (hero.is_free() &&
      get_keys_effect().get_action_key_effect() != KeysEffect::ACTION_KEY_NONE
  ) {

    KeysEffect::ActionKeyEffect effect = get_keys_effect().get_action_key_effect();
    get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

    // if this is a usual NPC, look towards the hero
    if (subtype == USUAL_NPC) {
      int direction = (get_hero().get_animation_direction() + 2) % 4;
      get_sprite().set_current_direction(direction);
    }

    if (effect != KeysEffect::ACTION_KEY_LIFT) {
      // start the normal behavior
      if (behavior == BEHAVIOR_DIALOG) {
        get_game().start_dialog(dialog_to_show, ScopedLuaRef(), ScopedLuaRef());
      }
      else {
        call_script_hero_interaction();
      }
      return true;
    }
    else {
      // lift the entity
      if (get_equipment().has_ability(Ability::LIFT)) {

        hero.start_lifting(std::make_shared<CarriedItem>(
            hero,
            *this,
            get_sprite().get_animation_set_id(),
            "stone",
            2,
            0)
        );
        Sound::play("lift");
        remove_from_map();
        return true;
      }
    }
  }
  return false;
}

/**
 * \brief Notifies the appropriate script that the hero is interacting with this entity.
 */
void Npc::call_script_hero_interaction() {

  if (behavior == BEHAVIOR_MAP_SCRIPT) {
    get_lua_context().entity_on_interaction(*this);
  }
  else {
    EquipmentItem& item = get_equipment().get_item(item_name);
    get_lua_context().item_on_npc_interaction(item, *this);
  }
}

/**
 * \brief Notifies this detector that the player is interacting by using an
 * equipment item.
 *
 * This function is called when the player uses an equipment item
 * while the hero is facing this NPC.
 *
 * \param item_used The equipment item used.
 * \return true if an interaction occured.
 */
bool Npc::interaction_with_item(EquipmentItem& item_used) {

  bool interaction_occured;
  if (behavior == BEHAVIOR_ITEM_SCRIPT) {
    EquipmentItem& item_to_notify = get_equipment().get_item(item_name);
    interaction_occured = get_lua_context().item_on_npc_interaction_item(
        item_to_notify, *this, item_used
    );
  }
  else {
    interaction_occured = get_lua_context().entity_on_interaction_item(
        *this, item_used
    );
  }

  return interaction_occured;
}

/**
 * \brief This function is called when the entity has just moved.
 *
 * If it is an NPC, its sprite's direction is updated.
 */
void Npc::notify_position_changed() {

  Detector::notify_position_changed();

  if (subtype == USUAL_NPC) {

    Sprite& sprite = get_sprite();
    if (get_movement() != nullptr) {
      // The NPC is moving.
      if (sprite.get_current_animation() != "walking") {
        sprite.set_current_animation("walking");
      }

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
 * \brief This function is called when the movement of the entity is finished.
 */
void Npc::notify_movement_finished() {

  Detector::notify_movement_finished();

  if (subtype == USUAL_NPC) {
    get_sprite().set_current_animation("stopped");
  }
}


/**
 * \brief Returns whether this interactive entity can be lifted.
 */
bool Npc::can_be_lifted() const {

  // there is currently no way to specify from the data file of the map
  // that an NPC can be lifted (nor its weight, damage, sound, etc) so this is hardcoded
  // TODO: specify the possibility to lift and the weight from Lua
  // npc:set_weight()?
  return has_sprite() && get_sprite().get_animation_set_id() == "entities/sign";
}

}

