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
#include "entities/DestructibleItem.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "movements/FallingHeight.h"
#include "ResourceManager.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "Map.h"
#include "Sound.h"
#include "ZSDX.h"
#include "Sprite.h"
#include "FileTools.h"

/**
 * Features of each type of destructible item.
 */
const DestructibleItem::Features DestructibleItem::features[] = {
  // animation set, sound, can be lifted, can be cut, can_explode, can_regenerate, weight, damage, special ground
  {"entities/pot",               "stone", true,  false, false, false, 0, 2, GROUND_NORMAL},
  {"entities/skull",             "stone", true,  false, false, false, 0, 2, GROUND_NORMAL},
  {"entities/bush",              "bush",  true,  true,  false, false, 1, 1, GROUND_NORMAL},
  {"entities/stone_small_white", "stone", true,  false, false, false, 1, 2, GROUND_NORMAL},
  {"entities/stone_small_black", "stone", true,  false, false, false, 2, 4, GROUND_NORMAL},
  {"entities/grass",             "bush",  false, true,  false, false, 0, 0, GROUND_GRASS},
  {"entities/bomb_flower",       "bush",  true,  false, true,  true,  1, 1, GROUND_NORMAL},
};

/**
 * Creates a new destructible item with the specified subtype.
 * @param layer layer of the destructible item to create on the map
 * @param x x coordinate of the destructible item to create
 * @param y y coordinate of the destructible item to create
 * @param subtype subtype of destructible item to create
 * @param pickable_item the subtype of pickable item that appears when the destructible
 * item is lifted or cut
 * @param pickable_item_savegame_variable index of the savegame boolean variable
 * storing the possession state of the pickable item,
 * for certain kinds of pickable items only (a key, a piece of heart...)
 */
DestructibleItem::DestructibleItem(Layer layer, int x, int y, DestructibleItem::Subtype subtype,
    PickableItem::Subtype pickable_item, int pickable_item_savegame_variable):
  Detector(COLLISION_NONE, "", layer, x, y, 16, 16),
  subtype(subtype), pickable_item(pickable_item),
  pickable_item_savegame_variable(pickable_item_savegame_variable),
  is_being_cut(false), regeneration_date(0) {

  set_origin(8, 13);
  create_sprite(get_animation_set_id());

  // set the collision mode
  if (features[subtype].can_be_lifted) {
    add_collision_mode(COLLISION_FACING_POINT);
  }

  if (features[subtype].can_be_cut) {
    add_collision_mode(COLLISION_SPRITE);
  }

  if (has_special_ground()) { // display a special ground under the hero
    add_collision_mode(COLLISION_ORIGIN_POINT);
  }
}

/**
 * Destructor.
 */
DestructibleItem::~DestructibleItem(void) {

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
MapEntity * DestructibleItem::parse(std::istream &is, Layer layer, int x, int y) {

  int subtype, pickable_item_subtype, savegame_variable;

  FileTools::read(is, subtype);
  FileTools::read(is, pickable_item_subtype);
  FileTools::read(is, savegame_variable);

  return new DestructibleItem(Layer(layer), x, y, Subtype(subtype),
      PickableItem::Subtype(pickable_item_subtype), savegame_variable);
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType DestructibleItem::get_type() {
  return DESTRUCTIBLE_ITEM;
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities when it is in front of them.
 * @return true if this entity is displayed at the same level as the hero
 */
bool DestructibleItem::is_displayed_in_y_order(void) {
  const SDL_Rect& size = get_sprite()->get_size();
  return size.h > 16;
}


/**
 * Returns the damage this destructible item can cause to enemies
 * @return the damage on enemies
 */
int DestructibleItem::get_damage_on_enemies(void) {
  return features[subtype].damage_on_enemies;
}

/**
 * Returns the animation set of this destructible item.
 * @return the animations of the sprite
 */
const std::string& DestructibleItem::get_animation_set_id(void) {
  return features[subtype].animation_set_id;
}

/**
 * Returns the sound to play when this item is destroyed.
 * @return the sound to play when this item is destroyed
 */
Sound * DestructibleItem::get_destruction_sound(void) {
  return ResourceManager::get_sound(features[subtype].destruction_sound_id);
}

/**
 * Returns the special ground to display when walking on this destructible item.
 * @return the ground, or GROUND_NORMAL if there is no special ground to display
 */
Ground DestructibleItem::get_special_ground(void) {
  return features[subtype].special_ground;
}

/**
 * Returns whether there is a special ground to display when walking on this destructible item.
 * @return true if there is a special ground
 */
bool DestructibleItem::has_special_ground(void) {
  return get_special_ground() != GROUND_NORMAL;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * For a destructible item, this does not depend on the other
 * entity but only on the subtype of destructible item.
 * @param other another entity
 * @return true if this entity is an obstacle for others
 */
bool DestructibleItem::is_obstacle_for(MapEntity *other) {
  return features[subtype].can_be_lifted && !is_being_cut && !is_disabled() && other->is_destructible_item_obstacle(this);
}

/**
 * This function is called by the engine when an entity overlaps the destructible item.
 * If the entity is the hero, we allow him to lift the item.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void DestructibleItem::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero()) {

    Hero *hero = zsdx->game->get_hero();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (features[subtype].can_be_lifted
	&& !is_being_cut
	&& keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE) {

      Equipment *equipment = zsdx->game->get_equipment();
      int weight = features[subtype].weight;
      if (equipment->can_lift(weight)) {
	keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LIFT);
      }
      else {
	keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
      }
    }

    else if (collision_mode == COLLISION_ORIGIN_POINT && has_special_ground() && !is_being_cut) {
      hero->set_ground(get_special_ground());
    }
  }
}

/**
 * This function is called by the engine when a sprite overlaps the destructible item.
 * If the sprite is the sword and this item can be cut, then the item may be cut.
 * @param entity an entity
 * @param sprite_overlapping the sprite of this entity that is overlapping the detector
 */
void DestructibleItem::notify_collision(MapEntity *entity, Sprite *sprite_overlapping) {

  if (features[subtype].can_be_cut
      && !is_being_cut
      && !is_disabled()
      && entity->is_hero()
      && sprite_overlapping->get_animation_set_id().find("sword") != std::string::npos) {

    Hero *hero = (Hero*) entity;
    if (hero->is_stroke_by_sword(this)) {

      play_destroy_animation();

      hero->just_moved(); // to update the ground under the hero

      if (pickable_item != PickableItem::NONE) {
	bool will_disappear = PickableItem::can_disappear(pickable_item);
	map->get_entities()->add_entity(PickableItem::create(get_layer(), get_x(), get_y(), pickable_item,
							     pickable_item_savegame_variable,
							     FALLING_MEDIUM, will_disappear));
      }
    }
  }
}

/**
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * The hero lifts the item if possible.
 */
void DestructibleItem::action_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  Hero *hero = zsdx->game->get_hero();
  KeysEffect::ActionKeyEffect effect = keys_effect->get_action_key_effect();

  if ((effect == KeysEffect::ACTION_KEY_LIFT || effect == KeysEffect::ACTION_KEY_LOOK)
      && features[subtype].can_be_lifted
      && !is_being_cut
      && !is_disabled()) {

    int weight = features[subtype].weight;
    Equipment *equipment = zsdx->game->get_equipment();

    if (equipment->can_lift(weight)) {
      hero->start_lifting(this);

      // play the sound
      ResourceManager::get_sound("lift")->play();

      // create the pickable item
      if (pickable_item != PickableItem::NONE) {
	bool will_disappear = PickableItem::can_disappear(pickable_item);
	map->get_entities()->add_entity(PickableItem::create(get_layer(), get_x(), get_y(), pickable_item,
	      pickable_item_savegame_variable, FALLING_MEDIUM, will_disappear));
      }

      // remove the item from the map
      if (!features[subtype].can_regenerate) {
	map->get_entities()->remove_entity(this);
      }
      else {
	// the item can actually regenerate
	play_destroy_animation();
      }
    }
    else {
      zsdx->game->show_message("_too_heavy");
    }
  }
}

/**
 * Plays the animation destroy of this item.
 */
void DestructibleItem::play_destroy_animation(void) {

  is_being_cut = true;
  get_destruction_sound()->play();
  get_sprite()->set_current_animation("destroy");
  if (!is_displayed_in_y_order()) {
    map->get_entities()->bring_to_front(this); // show animation destroy to front
  }
}

/**
 * Returns whether the item is disabled, i.e. if it was lifted
 * and is about to regenerate.
 */
bool DestructibleItem::is_disabled(void) {
  return regeneration_date != 0;
}

/**
 * Returns whether the item can explode.
 * @return true if the item will explode
 */
bool DestructibleItem::can_explode(void) {
  return features[subtype].can_explode;
}

/**
 * This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the entity, false to resume it
 */
void DestructibleItem::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement

  if (!suspended && regeneration_date != 0) {
    // recalculate the timer
    regeneration_date += SDL_GetTicks() - when_suspended;
  }
}


/**
 * Updates the item.
 */
void DestructibleItem::update(void) {

  MapEntity::update();

  if (suspended) {
    return;
  }

  if (is_being_cut && get_sprite()->is_animation_finished()) {

    if (!features[subtype].can_regenerate) {
      // remove the item from the map
      map->get_entities()->remove_entity(this);
    }
    else {
      is_being_cut = false;
      regeneration_date = SDL_GetTicks() + 2000;
    }
  }

  if (is_disabled() && SDL_GetTicks() >= regeneration_date && !overlaps(zsdx->game->get_hero())) {
    get_sprite()->set_current_animation("on_ground");
    regeneration_date = 0;
  }
}

