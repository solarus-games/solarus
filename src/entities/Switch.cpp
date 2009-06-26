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
#include "Sound.h"
#include "Sprite.h"
#include "Map.h"
#include "MapScript.h"

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
  Detector(COLLISION_ORIGIN_POINT, name, layer, x, y, 16, 16),
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
 * This function is called by the engine when an entity overlaps the switch.
 * This is a redefinition of Detector::collision().
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Switch::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

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
