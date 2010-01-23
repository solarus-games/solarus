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
#include "entities/DestinationPoint.h"
#include "Sprite.h"
#include "Game.h"
#include "lowlevel/FileTools.h"

/**
 * Constructor.
 * @param name name of the destination point to create
 * @param layer the layer
 * @param x x position of the destination point to create
 * @param y y position of the destination point to create
 * @param hero_direction initial direction of the hero in this state
 * (0 to 3, or -1 to indicate that the hero's direction is not changed)
 * @param is_visible true to make the destination point visible
 */
DestinationPoint::DestinationPoint(const std::string &name, Layer layer, int x, int y,
				   int hero_direction, bool is_visible):
  MapEntity(name, 0, layer, x, y, 16, 16) {

  set_origin(8, 13);

  this->change_direction = (direction != -1);
  if (change_direction) {
    set_direction(hero_direction);
  }

  if (is_visible) {
    create_sprite("entities/teletransporter");
    get_sprite()->set_current_animation("destination");
  }
}

/**
 * Destructor.
 */
DestinationPoint::~DestinationPoint(void) {

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
MapEntity * DestinationPoint::parse(Game *game, std::istream &is, Layer layer, int x, int y) {
	
  std::string name;
  int direction, is_visible;

  FileTools::read(is, name);
  FileTools::read(is, direction);
  FileTools::read(is, is_visible);
 
  return new DestinationPoint(name, layer, x, y, direction, (is_visible != 0));
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType DestinationPoint::get_type() {
  return DESTINATION_POINT;
}

