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
#include "entities/CrystalSwitch.h"
#include "entities/CarriedItem.h"
#include "entities/Boomerang.h"
#include "entities/Arrow.h"
#include "entities/Hero.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "ResourceManager.h"
#include "KeysEffect.h"
#include "lowlevel/Sound.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/System.h"
#include "lowlevel/Random.h"

/**
 * Creates a new crystal switch.
 * @param layer layer of the entity to create on the map
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 */
CrystalSwitch::CrystalSwitch(Layer layer, int x, int y):
  Detector(COLLISION_SPRITE | COLLISION_RECTANGLE | COLLISION_FACING_POINT,
	   "", layer, x, y, 16, 16),
  state(false), next_possible_hit_date(System::now()) {

  set_origin(8, 13);
  create_sprite("entities/crystal_switch", true);
  star_sprite = new Sprite("entities/star");
  twinkle();
}

/**
 * Destructor.
 */
CrystalSwitch::~CrystalSwitch(void) {

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
MapEntity * CrystalSwitch::parse(std::istream &is, Layer layer, int x, int y) {
  return new CrystalSwitch(layer, x, y);
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType CrystalSwitch::get_type() {
  return CRYSTAL_SWITCH;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one 
 */
bool CrystalSwitch::is_obstacle_for(MapEntity *other) {
  return other->is_crystal_switch_obstacle(this);
}

/**
 * This function is called when another entity collides with this crystal switch.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void CrystalSwitch::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->get_type() == CARRIED_ITEM && collision_mode == COLLISION_RECTANGLE) {

    CarriedItem *item = (CarriedItem*) entity_overlapping;
    if (item->is_being_thrown()) {
      activate();
      item->break_item();
    }
  }
  else if (entity_overlapping->get_type() == BOOMERANG && collision_mode == COLLISION_RECTANGLE) {

    Boomerang *boomerang = (Boomerang*) entity_overlapping;
    activate();
    if (!boomerang->is_going_back()) {
      boomerang->go_back();
    }
  }
  else if (entity_overlapping->get_type() == ARROW && collision_mode == COLLISION_RECTANGLE) {

    Arrow *arrow = (Arrow*) entity_overlapping;
    if (arrow->is_flying()) {
      activate();
      arrow->attach_to(this);
    }
  }
  else if (entity_overlapping->is_hero() && collision_mode == COLLISION_FACING_POINT) {

    Hero *hero = (Hero*) entity_overlapping;
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE) {

      // we show the action icon
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void CrystalSwitch::notify_collision(MapEntity *other_entity, Sprite *other_sprite, Sprite *this_sprite) {

  if (other_entity->is_hero() &&
      other_sprite->contains("sword")) {
    // the hero's sword is overlapping the crystal switch

    Hero *hero = (Hero*) other_entity;
    if (hero->get_state() != Hero::SWORD_LOADING && get_distance(hero) < 32) {
      activate();
    }
  }
  else if (other_entity->get_type() == EXPLOSION) {
    activate();
  }
}

/**
 * This function is called when the player presses the action key
 * while the hero is facing this detector, and the action icon lets him do this.
 */
void CrystalSwitch::action_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  Hero *hero = zsdx->game->get_hero();

  if (hero->get_state() == Hero::FREE) {
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

    // start a dialog
    zsdx->game->show_message("_crystal_switch");
  }
}

/**
 * Activates the crystal switch if the delay since the last activation allows it.
 */
void CrystalSwitch::activate(void) {

  uint32_t now = System::now();
  if (now >= next_possible_hit_date) {
    ResourceManager::get_sound("switch")->play();
    zsdx->game->change_crystal_switch_state();
    next_possible_hit_date = now + 1000;
  }
}

/**
 * Makes a star twinkle on the crystal switch at a random position.
 */
void CrystalSwitch::twinkle(void) {

  star_xy.set_xy(Random::get_number(3, 13), Random::get_number(3, 13));
  star_sprite->restart_animation();
}

/**
 * Updates the entity.
 */
void CrystalSwitch::update(void) {

  bool state = zsdx->game->get_crystal_switch_state();
  if (state != this->state) {

    this->state = state;
    get_sprite()->set_current_animation(state ? "blue_lowered" : "orange_lowered");
  }

  star_sprite->update();
  if (star_sprite->is_animation_finished()) {
    twinkle();
  }

  MapEntity::update();
}

/**
 * Displays the entity on the map.
 * This is a redefinition of MapEntity::display_on_map to also display the twinkling star
 * which has a special position.
 */
void CrystalSwitch::display_on_map(void) {

  MapEntity::display_on_map();

  map->display_sprite(star_sprite, get_top_left_x() + star_xy.get_x(), get_top_left_y() + star_xy.get_y());
}

/**
 * Suspends or resumes the entity.
 * @param suspended true to suspend the entity, false to resume it
 */
void CrystalSwitch::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended);

  if (!suspended) {
    next_possible_hit_date += System::now() - when_suspended;
  }
}

