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
#include "entities/Switch.h"
#include "ResourceManager.h"
#include "lowlevel/Sound.h"
#include "Sprite.h"
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
  Detector(COLLISION_CUSTOM, name, layer, x, y, 16, 16),
  subtype(subtype), needs_block(needs_block), disable_when_leaving(disable_when_leaving),
  enabled(false) {

  if (subtype == INVISIBLE && needs_block) {
    DIE("The switch '" << name << "' is invisible but needs a block");
  }

  if (subtype == NORMAL) {
    create_sprite("entities/switch");
    get_sprite()->set_current_animation("disabled");
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
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * Switch::parse(std::istream &is, Layer layer, int x, int y) {

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
 * Enables or disables the switch, not playing any sound.
 */
void Switch::set_enabled(bool enabled) {

  if (enabled != this->enabled) {
    this->enabled = enabled;

    if (subtype != INVISIBLE) {
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
 * Tests whether an entity's collides with this entity.
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool Switch::test_collision_custom(MapEntity *entity) {

  const SDL_Rect &entity_rectangle = entity->get_rectangle();
  int x1 = entity_rectangle.x + 4;
  int x2 = x1 + entity_rectangle.w - 9;
  int y1 = entity_rectangle.y + 4;
  int y2 = y1 + entity_rectangle.h - 9;

  return overlaps(x1, y1) && overlaps(x2, y1) &&
    overlaps(x1, y2) && overlaps(x2, y2);
}

/**
 * This function is called by the engine when an entity overlaps the switch.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Switch::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (enabled) {
    return;
  }

  if (entity_overlapping->is_hero()) {
    set_enabled(!needs_block);
  }
  else if (entity_overlapping->get_type() == BLOCK) {
    // don't enable an invisible switch with a block
    set_enabled(subtype != INVISIBLE);
  }

  if (enabled) {

    if (subtype != INVISIBLE) {
      ResourceManager::get_sound("switch")->play();
    }

    map->get_script()->event_switch_enabled(get_name());
  }
}

// TODO take into account disable_when_leaving
