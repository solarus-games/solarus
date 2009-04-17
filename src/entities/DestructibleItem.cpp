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
#include "movements/FallingOnFloorMovement.h"
#include "ResourceManager.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "Map.h"
#include "Sound.h"
#include "ZSDX.h"
#include "Sprite.h"
using namespace std;

/**
 * Features of each type of destructible item.
 */
const DestructibleItem::Features DestructibleItem::features[] = {
  {"entities/pot", "stone", true, false, 0, 2},
  {"entities/skull", "stone", true, false, 0, 2},
  {"entities/bush", "bush", true, true, 1, 1},
  {"entities/stone_small_white", "stone", true, false, 1, 2},
  {"entities/stone_small_black", "stone", true, false, 2, 4},
  {"entities/grass", "bush", false, true, 0, 0},

  /* not implemented
  {"entities/stone_big_white", "stone", 1},
  {"entities/stone_big_white", "stone", 2},
  */
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
  pickable_item_savegame_variable(pickable_item_savegame_variable), is_being_cut(false) {

  set_origin(8, 13);
  create_sprite(get_animation_set_id());

  // set the collision mode
  if (features[subtype].can_be_lifted) {
    add_collision_mode(COLLISION_FACING_POINT);
  }

  if (features[subtype].can_be_cut) {
    add_collision_mode(COLLISION_SPRITE);
  }

  if (subtype == GRASS) { // to display the grass ground under the hero
    add_collision_mode(COLLISION_ORIGIN_POINT);
  }
}

/**
 * Destructor.
 */
DestructibleItem::~DestructibleItem(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType DestructibleItem::get_type() {
  return DESTRUCTIBLE_ITEM;
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
const string& DestructibleItem::get_animation_set_id(void) {
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
 * Returns whether this entity is an obstacle for another one.
 * For a destructible item, this does not depend on the other
 * entity but only on the subtype of destructible item.
 * @param other another entity
 * @return true if this entity is an obstacle for others
 */
bool DestructibleItem::is_obstacle_for(MapEntity *other) {
  return features[subtype].can_be_lifted && !is_being_cut;
}

/**
 * This function is called by the engine when an entity overlaps the destructible item.
 * This is a redefinition of Detector::collision(MapEntity*).
 * If the entity is the hero, we allow him to lift the item.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void DestructibleItem::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

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

    else if (collision_mode == COLLISION_ORIGIN_POINT && subtype == GRASS && !is_being_cut) {
      hero->set_ground(GROUND_GRASS);
    }
  }
}

/**
 * This function is called by the engine when a sprite overlaps the destructible item.
 * This is a redefinition of Detector::collision(Sprite*).
 * If the sprite is the sword and this item can be cut, then the item may be cut.
 * @param entity an entity
 * @param sprite_overlapping the sprite of this entity that is overlapping the detector
 */
void DestructibleItem::collision(MapEntity *entity, Sprite *sprite_overlapping) {

  if (features[subtype].can_be_cut
      && !is_being_cut
      && entity->is_hero()
      && sprite_overlapping->get_animation_set_id().find("sword") != string::npos) {

    Hero *hero = (Hero*) entity;
    if (hero->is_stroke_by_sword(this)) {

      get_destruction_sound()->play();
      get_sprite()->set_current_animation("destroy");
      is_being_cut = true;

      if (subtype == GRASS) {
	hero->set_ground(GROUND_NORMAL);
      }

      map->get_entities()->bring_to_front(this); // show animation destroy to front

      if (pickable_item != PickableItem::NONE) {
	bool will_disappear = PickableItem::can_disappear(pickable_item);
	map->get_entities()->add_entity(PickableItem::create(get_layer(), get_x(), get_y(), pickable_item,
							     pickable_item_savegame_variable,
							     FallingOnFloorMovement::MEDIUM, will_disappear));
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
      && !is_being_cut) {

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
							     pickable_item_savegame_variable,
							     FallingOnFloorMovement::MEDIUM, will_disappear));
      }

      // remove the item from the map
      map->get_entities()->remove_entity(this);
    }
    else {
      zsdx->game->show_message("_too_heavy");
    }
  }
}

/**
 * Updates the item.
 */
void DestructibleItem::update(void) {

  MapEntity::update();

  if (is_being_cut && get_sprite()->is_animation_finished()) {

    // remove the item from the map
    map->get_entities()->remove_entity(this);
  }
}
