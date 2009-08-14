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
#ifndef ZSDX_SWITCH_H
#define ZSDX_SWITCH_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * A switch is a button that the hero can push.
 * Some switches require a block to be enabled.
 * Some switches become disabled when the hero or the block leave it.
 * A switch can be visible or invisible.
 */
class Switch: public Detector {

 public:

  /**
   * Subtypes of teletransporters.
   */
  enum Subtype {
    INVISIBLE = 0, /**< an invisible switch, usually to detect the hero position */
    NORMAL    = 1, /**< a classical switch */
  };

 private:

  Subtype subtype;           /**< subtype of teletransporter */
  bool needs_block;          /**< indicates that a block or a statue is required to enable to switch */
  bool disable_when_leaving; /**< indicates that the switch becomes disabled when the hero or the block leaves it */

  bool enabled;              /**< indicates that the switch is currently enabled */

 public:

  Switch(const std::string &name, Layer layer, int x, int y,
	 Subtype subtype, bool needs_block, bool disabled_when_leaving);
  ~Switch(void);
  static CreationFunction parse;

  EntityType get_type(void);

  void set_enabled(bool enabled);
  bool check_collision_custom(MapEntity *entity);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
};

#endif

