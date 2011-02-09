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
#include "entities/DestructibleItem.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "entities/PickableItem.h"
#include "entities/CarriedItem.h"
#include "movements/FallingHeight.h"
#include "Game.h"
#include "DialogBox.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "ItemProperties.h"
#include "Treasure.h"
#include "Map.h"
#include "Sprite.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"

/**
 * @brief Features of each type of destructible item.
 */
const DestructibleItem::Features DestructibleItem::features[] = {
  // animation set, sound, can be lifted, can be cut, can_explode, can_regenerate, weight, damage, special ground
  {"entities/pot",               "stone", true,  false, false, false, 0, 2, GROUND_NORMAL},
  {"entities/skull",             "stone", true,  false, false, false, 0, 2, GROUND_NORMAL},
  {"entities/bush",              "bush",  true,  true,  false, false, 1, 1, GROUND_NORMAL},
  {"entities/stone_small_white", "stone", true,  false, false, false, 1, 2, GROUND_NORMAL},
  {"entities/stone_small_black", "stone", true,  false, false, false, 2, 4, GROUND_NORMAL},
  {"entities/grass",             "bush",  false, true,  false, false, 0, 0, GROUND_GRASS},
  {"entities/bomb_flower",       "bush",  true,   true, true,  true,  1, 1, GROUND_NORMAL},
};

/**
 * @brief Creates a new destructible item with the specified subtype.
 * @param layer layer of the destructible item to create on the map
 * @param x x coordinate of the destructible item to create
 * @param y y coordinate of the destructible item to create
 * @param subtype subtype of destructible item to create
 * @param treasure the pickable item that appears when the destructible
 * item is lifted or cut
 */
DestructibleItem::DestructibleItem(Layer layer, int x, int y,
    DestructibleItem::Subtype subtype, const Treasure &treasure):

  Detector(COLLISION_NONE, "", layer, x, y, 16, 16),
  subtype(subtype),
  treasure(treasure),
  is_being_cut(false),
  regeneration_date(0),
  is_regenerating(false) {

  set_origin(8, 13);
  create_sprite(get_animation_set_id());

  // set the collision mode
  if (features[subtype].can_be_lifted) {
    add_collision_mode(COLLISION_FACING_POINT);
  }

  if (features[subtype].can_be_cut
      || features[subtype].can_explode) {
    add_collision_mode(COLLISION_SPRITE);
  }

  if (has_special_ground()) { // display a special ground under the hero
    add_collision_mode(COLLISION_CUSTOM);
  }
}

/**
 * @brief Destructor.
 */
DestructibleItem::~DestructibleItem() {
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
MapEntity* DestructibleItem::parse(Game &game, std::istream &is, Layer layer, int x, int y) {

  std::string treasure_name;
  int subtype, treasure_variant, treasure_savegame_variable;

  FileTools::read(is, subtype);
  FileTools::read(is, treasure_name);
  FileTools::read(is, treasure_variant);
  FileTools::read(is, treasure_savegame_variable);

  return new DestructibleItem(Layer(layer), x, y, Subtype(subtype),
      Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable));
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType DestructibleItem::get_type() {
  return DESTRUCTIBLE_ITEM;
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * Rhis function returns whether an entity of this type should be displayed above
 * the hero and other entities when it is in front of them.
 *
 * @return true if this entity is displayed at the same level as the hero
 */
bool DestructibleItem::is_displayed_in_y_order() {
  return false;
}


/**
 * @brief Returns the damage this destructible item can cause to enemies
 * @return the damage on enemies
 */
int DestructibleItem::get_damage_on_enemies() {
  return features[subtype].damage_on_enemies;
}

/**
 * @brief Returns the animation set of this destructible item.
 * @return the animations of the sprite
 */
const std::string& DestructibleItem::get_animation_set_id() {
  return features[subtype].animation_set_id;
}

/**
 * @brief Returns the id of the sound to play when this item is destroyed.
 * @return the destruction sound id
 */
const SoundId& DestructibleItem::get_destruction_sound_id() {
  return features[subtype].destruction_sound_id;
}

/**
 * @brief Returns the special ground to display when walking on this destructible item.
 * @return the ground, or GROUND_NORMAL if there is no special ground to display
 */
Ground DestructibleItem::get_special_ground() {
  return features[subtype].special_ground;
}

/**
 * @brief Returns whether there is a special ground to display when walking on this destructible item.
 * @return true if there is a special ground
 */
bool DestructibleItem::has_special_ground() {
  return get_special_ground() != GROUND_NORMAL;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 *
 * For a destructible item, this does not depend on the other
 * entity but only on the subtype of destructible item.
 *
 * @param other another entity
 * @return true if this entity is an obstacle for others
 */
bool DestructibleItem::is_obstacle_for(MapEntity &other) {
  return features[subtype].can_be_lifted && !is_being_cut && other.is_destructible_item_obstacle(*this);
}

/**
 * @brief Tests whether an entity's collides with this entity.
 *
 * This custom collision test is used for destructible items that change the ground displayed under the hero.
 *
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool DestructibleItem::test_collision_custom(MapEntity &entity) {
  return overlaps(entity.get_x(), entity.get_y() - 2);
}

/**
 * @brief Adds to the map the pickable treasure (if any) hidden under this destructible item.
 */
void DestructibleItem::create_pickable_item() {

  treasure.decide_content();
  get_entities().add_entity(PickableItem::create(get_game(), get_layer(), get_x(), get_y(),
      treasure, FALLING_MEDIUM, false));
}

/**
 * @brief This function is called by the engine when an entity overlaps the destructible item.
 *
 * If the entity is the hero, we allow him to lift the item.
 *
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void DestructibleItem::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_destructible_item(*this, collision_mode);
}

/**
 * @brief This function is called when this entity detects a collision with the hero.
 * @param hero the hero
 * @param collision_mode the collision mode that detected the collision
 */
void DestructibleItem::notify_collision_with_hero(Hero &hero, CollisionMode collision_mode) {

  if (features[subtype].can_be_lifted
      && !is_being_cut
      && !is_disabled()
      && get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
      && hero.is_free()) {

    int weight = features[subtype].weight;
    if (get_equipment().has_ability("lift", weight)) {
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LIFT);
    }
    else {
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }

  else if (collision_mode == COLLISION_CUSTOM && has_special_ground() && !is_being_cut) {
    hero.set_ground(get_special_ground());
  }
}

/**
 * @brief Notifies this entity that another sprite is overlapping it.
 *
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 *
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void DestructibleItem::notify_collision(MapEntity &other_entity, Sprite &other_sprite, Sprite &this_sprite) {

  if (features[subtype].can_be_cut
      && !is_being_cut
      && !is_disabled()
      && other_entity.is_hero()
      && other_sprite.contains("sword")) {

    Hero &hero = (Hero&) other_entity;
    if (hero.is_striking_with_sword(*this)) {

      play_destroy_animation();
      hero.check_position(); // to update the ground under the hero
      create_pickable_item();

      if (can_explode()) {
	explode();
      }
    }
  }

  // TODO use dynamic dispatch
  if (other_entity.get_type() == EXPLOSION
      && can_explode()
      && !is_being_cut
      && !is_disabled()) {

    play_destroy_animation();
    create_pickable_item();
    explode();
  }
}

/**
 * @brief Notifies this entity that the player is interacting with it.
 *
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * The hero lifts the item if possible.
 */
void DestructibleItem::action_key_pressed() {

  KeysEffect::ActionKeyEffect effect = get_keys_effect().get_action_key_effect();

  if ((effect == KeysEffect::ACTION_KEY_LIFT || effect == KeysEffect::ACTION_KEY_LOOK)
      && features[subtype].can_be_lifted
      && !is_being_cut
      && !is_disabled()) {

    int weight = features[subtype].weight;

    if (get_equipment().has_ability("lift", weight)) {

      uint32_t explosion_date = can_explode() ? System::now() + 6000 : 0;
      get_hero().start_lifting(new CarriedItem(get_hero(), *this,
	  get_animation_set_id(), get_destruction_sound_id(), get_damage_on_enemies(), explosion_date));

      // play the sound
      Sound::play("lift");

      // create the pickable item
      create_pickable_item();

      // remove the item from the map
      if (!features[subtype].can_regenerate) {
	remove_from_map();
      }
      else {
	// the item can actually regenerate
	play_destroy_animation();
      }
    }
    else {
      if (features[subtype].can_be_cut) {
        get_dialog_box().start_dialog("_cannot_lift_should_cut");
      }
      else if (!get_equipment().has_ability("lift", 1)) {
	get_dialog_box().start_dialog("_cannot_lift_too_heavy");
      }
      else {
	get_dialog_box().start_dialog("_cannot_lift_still_too_heavy");
      }
    }
  }
}

/**
 * @brief Plays the animation destroy of this item.
 */
void DestructibleItem::play_destroy_animation() {

  is_being_cut = true;
  Sound::play(get_destruction_sound_id());
  get_sprite().set_current_animation("destroy");
  if (!is_displayed_in_y_order()) {
    get_entities().bring_to_front(this); // show animation destroy to front
  }
}

/**
 * @brief Returns whether the item is disabled.
 *
 * The item is disabled if it was lifted and is about to regenerate.
 *
 * @return true if the item is disabled
 */
bool DestructibleItem::is_disabled() {
  return regeneration_date != 0 && !is_regenerating;
}

/**
 * @brief Returns whether the item can explode.
 * @return true if the item will explode
 */
bool DestructibleItem::can_explode() {
  return features[subtype].can_explode;
}

/**
 * @brief Creates an explosion on the item.
 */
void DestructibleItem::explode() {
  get_entities().add_entity(new Explosion(get_layer(), get_xy(), true));
}

/**
 * @brief This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the entity, false to resume it
 */
void DestructibleItem::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement

  if (!suspended && regeneration_date != 0) {
    // recalculate the timer
    regeneration_date += System::now() - when_suspended;
  }
}

/**
 * @brief Updates the item.
 */
void DestructibleItem::update() {

  MapEntity::update();

  if (suspended) {
    return;
  }

  if (is_being_cut && get_sprite().is_animation_finished()) {

    if (!features[subtype].can_regenerate) {
      // remove the item from the map
      remove_from_map();
    }
    else {
      is_being_cut = false;
      regeneration_date = System::now() + 10000;
    }
  }

  else if (is_disabled() && System::now() >= regeneration_date && !overlaps(get_hero())) {
    get_sprite().set_current_animation("regenerating");
    is_regenerating = true;
    regeneration_date = 0;
  }
  else if (is_regenerating && get_sprite().is_animation_finished()) {
    get_sprite().set_current_animation("on_ground");
    is_regenerating = false;
  }
}

