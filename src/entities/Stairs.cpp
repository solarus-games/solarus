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
#include "entities/Stairs.h"
#include "lowlevel/FileTools.h"
#include "Game.h"

/**
 * Creates a new stairs entity.
 * @param layer layer of the entity to create on the map
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param direction direction of the stairs (0 to 3)
 * @param subtype the subtype of stairs
 */
Stairs::Stairs(Layer layer, int x, int y, int direction, Subtype subtype):
  Detector(COLLISION_FACING_POINT | COLLISION_RECTANGLE, "", layer, x, y, 16, 16),
  subtype(subtype) {

  if (is_inside_floor() && layer == LAYER_HIGH) {
    DIE("Cannot put single floor stairs on the high layer");
  }

  set_direction(direction);

  if (!is_inside_floor()) {
    set_size(16, 8);
    if (direction == 3) { // down
      set_origin(0, -8);
    }
  }
}

/**
 * Destructor.
 */
Stairs::~Stairs(void) {

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
MapEntity * Stairs::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  int direction, subtype;
  FileTools::read(is, direction);
  FileTools::read(is, subtype);
  return new Stairs(Layer(layer), x, y, direction, Subtype(subtype));
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Stairs::get_type() {
  return STAIRS;
}

/**
 * Returns whether the subtype of these stairs is INSIDE_FLOOR.
 * @return true if the subtype if INSIDE_FLOOR
 */
bool Stairs::is_inside_floor(void) {
  return subtype == INSIDE_FLOOR;
}

/**
 * Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * @return true if this entity can collide with entities that are on another layer
 */
bool Stairs::has_layer_independent_collisions(void) {
  return is_inside_floor();
}

/**
 * Returns true if this entity does not react to the sword.
 * If true is returned, nothing will happen when the hero hits this entity with the sword.
 * @return true if the sword is ignored
 */
bool Stairs::is_sword_ignored(void) {
  return true;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Stairs::is_obstacle_for(MapEntity *other) {
  return other->is_stairs_obstacle(this);
}

/**
 * This function is called when another entity overlaps this entity.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void Stairs::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  entity_overlapping->notify_collision_with_stairs(this);
}

/**
 * Returns the direction of the movement an entity would take when activating these stairs.
 * If the stairs are not inside a single map, the direction is simply north or south
 * depending on the stairs direction.
 * Otherwise, the direction depends on the entity's layer.
 * If the entity's layer is LAYER_LOW, then the entity would go upstairs and this function returns 
 * the direction property of the stairs (as returned by MapEntity::get_direction()).
 * If the entity's layer is LAYER_INTERMEDIATE, then the entity would go downstairs and the opposite
 * direction is returned.
 * This function can be used to determine whether an entity is taking the correct direction
 * to take the stairs.
 * @param initial_layer the layer from which you intend to take the stairs
 * @return the movement direction an entity on this layer should take on these stairs (0 to 7)
 */
int Stairs::get_movement_direction(Layer initial_layer) {

  int movement_direction = get_direction() * 2;
  if (is_inside_floor() && initial_layer != LAYER_LOW) {
    movement_direction = (movement_direction + 4) % 8;
  }

  return movement_direction;
}

/**
 * Returns the direction of the animation an entity should take when activating these stairs.
 * For spiral stairs, the direction returned is diagonal.
 * @return the direction of animation (0 to 7)
 */
int Stairs::get_animation_direction(void) {

  int basic_direction = get_direction();
  int result = basic_direction;

  if (subtype == SPIRAL_UPSTAIRS) {
    result = (basic_direction == 1) ? 1 : 5;
  }
  else if (subtype == SPIRAL_DOWNSTAIRS) {
    result = (basic_direction == 1) ? 3 : 7;
  }

  return result;
}

/**
 * When an entity collides with the stairs (usually the hero),
 * it can call this function to play the stairs sound.
 * @param entity_overlapping the entity taking the stairs
 */
void Stairs::play_sound(MapEntity *entity_overlapping) {

  if (is_inside_floor()) {
    if (entity_overlapping->get_layer() == LAYER_LOW) {
      game->play_sound("stairs_up_end");
    }
    else {
      game->play_sound("stairs_down_end");
    }
  }
  else {
    // TODO
    game->play_sound("stairs_down_start");
  }
}

/**
 * When an entity collides with the stairs (usually the hero),
 * it can call this function to know the path it should take to make the appropriate movement.
 * @param entity_overlapping the entity taking the stairs
 * @return the corresponding path to make
 */
std::string Stairs::get_path(MapEntity *entity_overlapping) {

  // determine the movement direction
  int movement_direction = get_movement_direction(entity_overlapping->get_layer());
  std::string path = "";
  char path_element;
  int nb_steps;
  if (is_inside_floor()) {
    nb_steps = 5;
  }
  else {
    nb_steps = (get_direction() == 1) ? 1 : 2;
  }
  path_element = '0' + movement_direction;
  for (int i = 0; i < nb_steps; i++) {
    path += path_element;
  }

  return path;
}

