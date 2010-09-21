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
#include "entities/CustomObstacle.h"
#include "lowlevel/FileTools.h"

/**
 * @brief Constructor.
 * @param name name of the custom obstacle to create
 * @param layer layer of the  to create
 * @param x x position of the  to create
 * @param y y position of the  to create
 * @param width width of the custom obstacle to create
 * @param height height of the custom obstacle to create
 * @param stops_hero true to make this entity an obstacle for the hero
 * @param stops_enemies true to make this entity an obstacle for the enemies
 * @param stops_npcs true to make this entity an obstacle for the NPCs
 * @param stops_blocks true to make this entity an obstacle for the blocks and statues
 */
CustomObstacle::CustomObstacle(const std::string &name, Layer layer, int x, int y, int width, int height,
			       bool stops_hero, bool stops_enemies, bool stops_npcs, bool stops_blocks):
  MapEntity(name, 0, layer, x, y, width, height) {

  entity_types_stopped[HERO] = stops_hero;
  entity_types_stopped[ENEMY] = stops_enemies;
  entity_types_stopped[INTERACTIVE_ENTITY] = stops_npcs;
  entity_types_stopped[BLOCK] = stops_blocks;
}

/**
 * @brief Destructor.
 */
CustomObstacle::~CustomObstacle() {

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
MapEntity * CustomObstacle::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  std::string name;
  int width, height, stops_hero, stops_enemies, stops_npcs, stops_blocks;

  FileTools::read(is, width);
  FileTools::read(is, height);
  FileTools::read(is, name);
  FileTools::read(is, stops_hero);
  FileTools::read(is, stops_enemies);
  FileTools::read(is, stops_npcs);
  FileTools::read(is, stops_blocks);

  return new CustomObstacle(name, Layer(layer), x, y, width, height,
      stops_hero != 0, stops_enemies != 0, stops_npcs != 0, stops_blocks != 0);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType CustomObstacle::get_type() {
  return CUSTOM_OBSTACLE;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool CustomObstacle::is_obstacle_for(MapEntity *other) {
  return entity_types_stopped[other->get_type()];
}

