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
#include "entities/CrystalSwitch.h"
#include "entities/Hero.h"
#include "Game.h"
#include "DialogBox.h"
#include "Map.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "KeysEffect.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/System.h"
#include "lowlevel/Random.h"
#include "lowlevel/Sound.h"

/**
 * @brief Creates a new crystal switch.
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
 * @brief Destructor.
 */
CrystalSwitch::~CrystalSwitch() {
  delete star_sprite;
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
MapEntity* CrystalSwitch::parse(Game &game, std::istream &is, Layer layer, int x, int y) {
  return new CrystalSwitch(layer, x, y);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType CrystalSwitch::get_type() {
  return CRYSTAL_SWITCH;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one 
 */
bool CrystalSwitch::is_obstacle_for(MapEntity *other) {
  return other->is_crystal_switch_obstacle(this);
}

/**
 * @brief This function is called when another entity collides with this crystal switch.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void CrystalSwitch::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  entity_overlapping->notify_collision_with_crystal_switch(this, collision_mode);
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
void CrystalSwitch::notify_collision(MapEntity *other_entity, Sprite *other_sprite, Sprite *this_sprite) {
  other_entity->notify_collision_with_crystal_switch(this, other_sprite);
}

/**
 * @brief This function is called when the player interacts with this entity.
 *
 * This function is called when the player presses the action key
 * while the hero is facing this detector, and the action icon lets him do this.
 */
void CrystalSwitch::action_key_pressed() {

  KeysEffect &keys_effect = game->get_keys_effect();
  Hero &hero = game->get_hero();

  if (hero.is_free()) {
    keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

    // start a dialog
    game->get_dialog_box().start_dialog("_crystal_switch");
  }
}

/**
 * @brief Activates the crystal switch if the delay since the last activation allows it.
 * @param entity_activating the entity that activates this crystal switch
 */
void CrystalSwitch::activate(MapEntity *entity_activating) {

  bool recently_activated = false;
  std::list<MapEntity*>::iterator it;
  for (it = entities_activating.begin(); it != entities_activating.end() && !recently_activated; it++) {
    recently_activated  = (*it == entity_activating);
  }

  uint32_t now = System::now();
  if (!recently_activated || now >= next_possible_hit_date) {
    Sound::play("switch");
    game->change_crystal_switch_state();
    next_possible_hit_date = now + 1000;
    entities_activating.push_back(entity_activating);
  }
}

/**
 * @brief Makes a star twinkle on the crystal switch at a random position.
 */
void CrystalSwitch::twinkle() {

  star_xy.set_xy(Random::get_number(3, 13), Random::get_number(3, 13));
  star_sprite->restart_animation();
}

/**
 * @brief Updates the entity.
 */
void CrystalSwitch::update() {

  bool state = game->get_crystal_switch_state();
  if (state != this->state) {

    this->state = state;
    get_sprite()->set_current_animation(state ? "blue_lowered" : "orange_lowered");
  }

  star_sprite->update();
  if (star_sprite->is_animation_finished()) {
    twinkle();
  }

  uint32_t now = System::now();
  if (now >= next_possible_hit_date) {
    entities_activating.clear();
  }
 
  MapEntity::update();
}

/**
 * @brief Displays the entity on the map.
 *
 * This is a redefinition of MapEntity::display_on_map() to also display the twinkling star
 * which has a special position.
 */
void CrystalSwitch::display_on_map() {

  // display the crystal switch
  MapEntity::display_on_map();

  // display the star
  map->display_sprite(star_sprite, get_top_left_x() + star_xy.get_x(), get_top_left_y() + star_xy.get_y());
}

/**
 * @brief Suspends or resumes the entity.
 * @param suspended true to suspend the entity, false to resume it
 */
void CrystalSwitch::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended);

  if (!suspended) {
    next_possible_hit_date += System::now() - when_suspended;
  }
}

