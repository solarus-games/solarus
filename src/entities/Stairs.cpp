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
#include "Map.h"

/**
 * @brief Creates a new stairs entity.
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
 * @brief Destructor.
 */
Stairs::~Stairs(void) {

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
MapEntity * Stairs::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  int direction, subtype;
  FileTools::read(is, direction);
  FileTools::read(is, subtype);
  return new Stairs(Layer(layer), x, y, direction, Subtype(subtype));
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Stairs::get_type() {
  return STAIRS;
}

/**
 * @brief Returns whether the subtype of these stairs is INSIDE_FLOOR.
 * @return true if the subtype if INSIDE_FLOOR
 */
bool Stairs::is_inside_floor(void) {
  return subtype == INSIDE_FLOOR;
}

/**
 * @brief Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * @return true if this entity can collide with entities that are on another layer
 */
bool Stairs::has_layer_independent_collisions(void) {
  return is_inside_floor();
}

/**
 * @brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 *
 * @return true if the sword is ignored
 */
bool Stairs::is_sword_ignored(void) {
  return true;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Stairs::is_obstacle_for(MapEntity *other) {
  return other->is_stairs_obstacle(this);
}

/**
 * @brief This function is called when another entity overlaps this entity.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void Stairs::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  entity_overlapping->notify_collision_with_stairs(this, collision_mode);
}

/**
 * @brief Returns the direction of the movement an entity would take when activating these stairs.
 * @param way the way you intend to take these stairs
 * @return the movement direction an entity should take on these stairs (0 to 7)
 */
int Stairs::get_movement_direction(Way way) {

  int movement_direction = get_direction() * 2;
  if (way == REVERSE_WAY) {
    movement_direction = (movement_direction + 4) % 8;
  }

  return movement_direction;
}

/**
 * @brief Returns the direction of the animation an entity should take when walking these stairs.
 *
 * For spiral stairs, the direction returned is diagonal.
 *
 * @param way the way you are taking these stairs
 * @return the direction of animation (0 to 7)
 */
int Stairs::get_animation_direction(Way way) {

  int basic_direction = get_direction() * 2;
  int result = basic_direction;

  if (subtype == SPIRAL_UPSTAIRS) {
    result = (basic_direction == 2) ? 1 : 5;
  }
  else if (subtype == SPIRAL_DOWNSTAIRS) {
    result = (basic_direction == 2) ? 3 : 7;
  }

  if (way == REVERSE_WAY) {
    result = (result + 4) % 8;
  }

  return result;
}

/**
 * @brief Plays a sound corresponding to these stairs.
 *
 * When an entity collides with the stairs (usually the hero),
 * it can call this function to play the appropriate stairs sound.
 *
 * @param way the way you are taking these stairs
 */
void Stairs::play_sound(Way way) {

  SoundId sound_id;
  if (is_inside_floor()) {
    // choose the sound depending on whether we are going upstairs or downstairs
    sound_id = (way == NORMAL_WAY) ? "stairs_up_end" : "stairs_down_end";
  }
  else {
    // choose the sound depending on whether we are on the old floor or the new floor, and
    // going upstairs or downstairs
    if (subtype == SPIRAL_UPSTAIRS || subtype == STRAIGHT_UPSTAIRS) {
      sound_id = (way == NORMAL_WAY) ? "stairs_up_start" : "stairs_down_end";
    }
    else {
      sound_id = (way == NORMAL_WAY) ? "stairs_down_start" : "stairs_up_end";
    }
  }
  game->play_sound(sound_id);
}

/**
 * @brief Returns the path an entity should follow to take these stairs.
 *
 * When an entity collides with the stairs (usually the hero),
 * it can call this function to know the path it should take to make the appropriate movement.
 *
 * @param way the way you are taking these stairs
 * @return the corresponding path to make
 */
std::string Stairs::get_path(Way way) {

  // determine the movement direction
  int initial_direction = get_direction() * 2;
  std::string path = "";
  int nb_steps;
  if (is_inside_floor()) {
    nb_steps = 5;
  }
  else {
    nb_steps = (get_direction() == 1) ? 1 : 2;
  }
  for (int i = 0; i < nb_steps; i++) {
    path += '0' + initial_direction;
  }

  if (!is_inside_floor()) {

    static const int second_directions[] = {
      0, 4, 2, 2 // second direction to take for each subtype of stairs (assuming the direction is north)
    };
    int second_direction = second_directions[subtype];
    if (get_direction() == 3) { // direction south
      second_direction = (second_direction + 4) % 8;
    }
    char c = '0' + second_direction;
    path = path + c;
    if (subtype == SPIRAL_UPSTAIRS || subtype == SPIRAL_DOWNSTAIRS) {
      path = path + c;
    }
  }

  if (way == REVERSE_WAY) {
    std::string inverse_path = "";
    std::string::reverse_iterator it;
    int direction = 0;
    for (it = path.rbegin(); it != path.rend(); it++) {
      direction = *it - '0';
      direction = (direction + 4) % 8;
      inverse_path += '0' + direction;
    }
    path = inverse_path;
//    path += '0' + direction;
  }

  return path;
}

/**
 * @brief Returns the subarea in which an entity tooking these stairs can be displayed.
 * @param way the way you are taking these stairs
 * @return the subarea of the map where the entity displaying should be restricted to
 */
Rectangle Stairs::get_clipping_rectangle(Way way) {

  Rectangle clipping_rectangle(get_top_left_x(), 0, 16, map->get_height());

  if (subtype == STRAIGHT_DOWNSTAIRS) {
    
    if (get_direction() == 1) {
      clipping_rectangle.set_y(get_top_left_y() - 8);
      clipping_rectangle.set_height(48);
    }
    else {
      clipping_rectangle.set_y(0);
      clipping_rectangle.set_height(get_top_left_y() + 16);
    }
  }

  return clipping_rectangle;
}

