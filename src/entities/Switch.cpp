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
#include "entities/Switch.h"
#include "entities/Arrow.h"
#include "entities/Block.h"
#include "entities/Hero.h"
#include "Sprite.h"
#include "Game.h"
#include "Map.h"
#include "lua/MapScript.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"

/**
 * @brief Constructor.
 * @param name name of the entity
 * @param layer layer of the entity
 * @param x x position of the entity's rectangle
 * @param y y position of the entity's rectangle
 * @param subtype the subtype of switch
 * @param needs_block true if a block is required to activate this switch
 * @param inactivate_when_leaving true to inactivate the switch when the hero or
 * the block leaves it
 */
Switch::Switch(const std::string &name, Layer layer, int x, int y,
	       Subtype subtype, bool needs_block, bool inactivate_when_leaving):
  Detector(COLLISION_NONE, name, layer, x, y, 16, 16),
  subtype(subtype), activated(false), locked(false),
  needs_block(needs_block), inactivate_when_leaving(inactivate_when_leaving),
  entity_overlapping(NULL) {

  Debug::check_assertion(subtype != WALKABLE_INVISIBLE || !needs_block,
      StringConcat() << "The switch '" << name << "' is invisible but needs a block");

  // sprite
  if (subtype == WALKABLE_VISIBLE) {
    create_sprite("entities/switch");
    get_sprite().set_current_animation("inactivated");
  }

  // collisions
  if (is_walkable()) {
    set_collision_modes(COLLISION_CUSTOM);
  }
  else {
    set_collision_modes(COLLISION_FACING_POINT);
  }
}

/**
 * @brief Destructor.
 */
Switch::~Switch() {

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
MapEntity* Switch::parse(Game &game, std::istream &is, Layer layer, int x, int y) {

  std::string name;
  int subtype, needs_block, inactivate_when_leaving;

  FileTools::read(is, name);
  FileTools::read(is, subtype);
  FileTools::read(is, needs_block);
  FileTools::read(is, inactivate_when_leaving);

  return new Switch(name, Layer(layer), x, y, Subtype(subtype), needs_block != 0, inactivate_when_leaving != 0);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Switch::get_type() {
  return SWITCH;
}

/**
 * @brief Returns wether this switch is a walkable switch.
 * @return true if the subtype of switch is WALKABLE_INVISIBLE or WALKABLE_VISIBLE
 */
bool Switch::is_walkable() {
  return subtype == WALKABLE_INVISIBLE || subtype == WALKABLE_VISIBLE;
}

/**
 * @brief Returns whether this switch is currently activated.
 *
 * @return true if the switch is activated
 */
bool Switch::is_activated() {
  return activated;
}

/**
 * @brief Activates the switch, playing a sound and notifying the map script.
 *
 * This function does nothing if the switch is locked or already activated.
 */
void Switch::activate() {

  if (!activated && !locked) {

    set_activated(true);

    if (subtype == WALKABLE_VISIBLE) {
      Sound::play("switch");
    }

    get_map_script().event_switch_activated(get_name());
  }
}

/**
 * @brief Activates or inactivates the switch, not playing any sound.
 *
 * This function can change the switch state even if the switch is locked.
 *
 * @param activated true to make the switch on, false to make it off
 */
void Switch::set_activated(bool activated) {

  if (activated != this->activated) {
    this->activated = activated;

    if (subtype == WALKABLE_VISIBLE) {
      if (activated) {
        get_sprite().set_current_animation("activated");
      }
      else {
        get_sprite().set_current_animation("inactivated");
      }
    }
  }
}

/**
 * @brief Locks this switch is its current state or unlocks it.
 *
 * When the switch is locked, it cannot be activated or inactivated by other entities.
 * However, the state can still be changed manually by calling set_activated().
 *
 * @param locked true to lock the switch in its current state, false to unlock it
 */
void Switch::set_locked(bool locked) {
  this->locked = locked;
}

/**
 * @brief Updates this switch.
 */
void Switch::update() {

  if (is_enabled() && is_walkable() && entity_overlapping != NULL) {

    // if an entity was on the switch, see if it is still there
    entity_overlapping_still_present = false;
    check_collision(*entity_overlapping);

    if (!entity_overlapping_still_present) {
      // the entity just left the switch or disappeared from the map
      // (it may even have been freed)

      entity_overlapping = NULL;
      if (is_activated() && inactivate_when_leaving && !locked) {
        set_activated(false);
        get_map_script().event_switch_inactivated(get_name());
      }
      get_map_script().event_switch_left(get_name());
    }
  }
}

/**
 * @brief Tests whether an entity's collides with this entity.
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool Switch::test_collision_custom(MapEntity &entity) {

  // this collision test is performed for walkable switches only

  const Rectangle &entity_rectangle = entity.get_bounding_box();
  int x1 = entity_rectangle.get_x() + 4;
  int x2 = x1 + entity_rectangle.get_width() - 9;
  int y1 = entity_rectangle.get_y() + 4;
  int y2 = y1 + entity_rectangle.get_height() - 9;

  return overlaps(x1, y1) && overlaps(x2, y1) &&
    overlaps(x1, y2) && overlaps(x2, y2);
}

/**
 * @brief This function is called by the engine when an entity overlaps the switch.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Switch::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  if (&entity_overlapping == this->entity_overlapping) {
    // already overlapping
    entity_overlapping_still_present = true;
    return;
  }

  if (!activated && !locked) {
    entity_overlapping.notify_collision_with_switch(*this);
  }
}

/**
 * @brief This function is called when the hero overlaps this switch.
 *
 * The switch is activated if its properties allow it.
 *
 * @param hero the hero
 */
void Switch::try_activate(Hero &hero) {

  if (is_walkable() && !needs_block) {
    // this switch allows the hero to activate it
    activate();
  }
  this->entity_overlapping = &hero;
}

/**
 * @brief This function is called when a block overlaps this switch.
 *
 * The switch is activated if its properties allow it.
 *
 * @param block the block overlapping this switch
 */
void Switch::try_activate(Block &block) {

  if (subtype == WALKABLE_VISIBLE) {
    // blocks can only activate walkable, visible switches
    activate();
  }
  this->entity_overlapping = &block;
}

/**
 * @brief This function is called when an arrow overlaps this switch.
 *
 * The switch is activated if its properties allow it.
 *
 * @param arrow the arrow overlapping this switch
 */
void Switch::try_activate(Arrow &arrow) {

  if (subtype == ARROW_TARGET) {
    // arrows can only activate arrow targets
    activate();
  }
}

