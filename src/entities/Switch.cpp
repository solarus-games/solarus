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
#include "Sprite.h"
#include "Game.h"
#include "Map.h"
#include "MapScript.h"
#include "lowlevel/FileTools.h"

/**
 * Constructor.
 * @param name name of the entity
 * @param layer layer of the entity
 * @param x x position of the entity's rectangle
 * @param y y position of the entity's rectangle
 * @param subtype the subtype of switch
 * @param needs_block true if a block is required to enabled this switch
 * @param disable_when_leaving true to disable the switch when the hero or 
 * the block leaves it
 */
Switch::Switch(const std::string &name, Layer layer, int x, int y,
	       Subtype subtype, bool needs_block, bool disable_when_leaving):
  Detector(COLLISION_NONE, name, layer, x, y, 16, 16),
  subtype(subtype), needs_block(needs_block), disable_when_leaving(disable_when_leaving),
  enabled(false), locked(false), entity_overlapping(NULL) {

  if (subtype == WALKABLE_INVISIBLE && needs_block) {
    DIE("The switch '" << name << "' is invisible but needs a block");
  }

  // sprite
  if (subtype == WALKABLE_VISIBLE) {
    create_sprite("entities/switch");
    get_sprite()->set_current_animation("disabled");
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
 * Destructor.
 */
Switch::~Switch(void) {

}

/**
 * Creates an instance from an input stream.
 * The input stream must respect the syntax of this entity type.
 * @param game the game that will contain the entity created
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * Switch::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  std::string name;
  int subtype, needs_block, disabled_when_leaving;

  FileTools::read(is, name);
  FileTools::read(is, subtype);
  FileTools::read(is, needs_block);
  FileTools::read(is, disabled_when_leaving);

  return new Switch(name, Layer(layer), x, y, Subtype(subtype), needs_block != 0, disabled_when_leaving != 0);
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Switch::get_type() {
  return SWITCH;
}

/**
 * Returns wether this switch is a walkable switch.
 * @return true if the subtype of switch is WALKABLE_INVISIBLE or WALKABLE_VISIBLE
 */
bool Switch::is_walkable(void) {
  return subtype == WALKABLE_INVISIBLE || subtype == WALKABLE_VISIBLE;
}

/**
 * Returns whether this switch is currently enabled.
 * @return true if the switch is enabled
 */
bool Switch::is_enabled(void) {
  return enabled;
}

/**
 * Enables or disables the switch, not playing any sound.
 * This function can change the switch state even if the switch is locked.
 * @param enabled true to make the switch enabled, false to make it disabled
 */
void Switch::set_enabled(bool enabled) {

  if (enabled != this->enabled) {
    this->enabled = enabled;

    if (subtype == WALKABLE_VISIBLE) {
      if (enabled) {
        get_sprite()->set_current_animation("enabled");
      }
      else {
        get_sprite()->set_current_animation("disabled");
      }
    }
  }
}

/**
 * Locks this switch is its current state or unlocks it.
 * When the switch is locked, it cannot be enabled or disabled by other entities.
 * However, the state can still be changed manually by calling set_enabled().
 * @param locked true to lock the switch in its current state, false to unlock it
 */
void Switch::set_locked(bool locked) {
  this->locked = locked;
}

/**
 * Updates this switch.
 */
void Switch::update(void) {

  if (entity_overlapping != NULL) {

    // if an entity was on the switch, see if it is still there
    entity_overlapping_still_present = false;
    check_collision(entity_overlapping);

    if (!entity_overlapping_still_present) {
      // the entity just left the switch

      entity_overlapping = NULL;
      if (is_enabled() && disable_when_leaving && !locked) {
        set_enabled(false);
	map->get_script()->event_switch_disabled(get_name());
      }
      map->get_script()->event_switch_left(get_name());
    }
  }
}

/**
 * Tests whether an entity's collides with this entity.
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool Switch::test_collision_custom(MapEntity *entity) {

  // this collision test is performed for walkable switches only

  const Rectangle &entity_rectangle = entity->get_bounding_box();
  int x1 = entity_rectangle.get_x() + 4;
  int x2 = x1 + entity_rectangle.get_width() - 9;
  int y1 = entity_rectangle.get_y() + 4;
  int y2 = y1 + entity_rectangle.get_height() - 9;

  return overlaps(x1, y1) && overlaps(x2, y1) &&
    overlaps(x1, y2) && overlaps(x2, y2);
}

/**
 * This function is called by the engine when an entity overlaps the switch.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Switch::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping == this->entity_overlapping) {
    // already overlapping
    entity_overlapping_still_present = true;
    return;
  }

  if (enabled || locked) {
    return;
  }

  if (is_walkable()) {
    // walkable switch: allow the hero or a block
    if (entity_overlapping->is_hero()) {
      set_enabled(!needs_block);
    }
    else if (entity_overlapping->get_type() == BLOCK) {
      // blocks can only enable walkable, visible switches
      set_enabled(subtype == WALKABLE_VISIBLE);
    }
  }
  else if (subtype == ARROW_TARGET && entity_overlapping->get_type() == ARROW) {
    // arrow target: only allow an arrow
    Arrow *arrow = (Arrow*) entity_overlapping;
    set_enabled(arrow->is_stopped());
  }

  if (enabled) {

    this->entity_overlapping = entity_overlapping;
    if (subtype == WALKABLE_VISIBLE) {
      game->play_sound("switch");
    }

    map->get_script()->event_switch_enabled(get_name());
  }
}

