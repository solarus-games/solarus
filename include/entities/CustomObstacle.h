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
#ifndef ZSDX_CUSTOM_OBSTACLE_H
#define ZSDX_CUSTOM_OBSTACLE_H

#include "Common.h"
#include "entities/MapEntity.h"
#include <map>

/**
 * This type of entity is an invisible obstacle for some other types of entities.
 */
class CustomObstacle: public MapEntity {

 private:

  std::map<int, bool> entity_types_stopped;

 public:

  CustomObstacle(const std::string &name, Layer layer, int x, int y, int width, int height,
		 bool stops_hero, bool stops_enemies, bool stops_npcs, bool stops_blocks);
  ~CustomObstacle(void);
  static CreationFunction parse;

  EntityType get_type(void);
  bool is_obstacle_for(MapEntity *other);
};

#endif
