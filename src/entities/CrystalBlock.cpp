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
#include "entities/CrystalBlock.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "movements/PlayerMovement.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Sound.h"
#include <list>

/**
 * @brief Creates a new crystal block.
 * @param game the current game
 * @param layer layer of the entity to create on the map
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param width width of the block (the pattern can be repeated)
 * @param height height of the block (the pattern can be repeated)
 * @param subtype subtype of raised block
 */
CrystalBlock::CrystalBlock(Game &game, Layer layer, int x, int y, int width, int height, Subtype subtype):
  Detector(COLLISION_RECTANGLE, "", layer, x, y, width, height),
  subtype(subtype) {

  create_sprite("entities/crystal_block");

  this->orange_raised = game.get_crystal_state();

  if (subtype == ORANGE) {
    get_sprite().set_current_animation(orange_raised ? "orange_raised" : "orange_lowered");
  }
  else {
    get_sprite().set_current_animation(orange_raised ? "blue_lowered" : "blue_raised");
  }
  get_sprite().set_current_frame(3); // to avoid the animations at the map beginning
}

/**
 * @brief Destructor.
 */
CrystalBlock::~CrystalBlock() {

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
MapEntity* CrystalBlock::parse(Game &game, std::istream &is, Layer layer, int x, int y) {

  int width, height, subtype;

  FileTools::read(is, width);
  FileTools::read(is, height);
  FileTools::read(is, subtype);

  return new CrystalBlock(game, Layer(layer), x, y, width, height, Subtype(subtype));
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType CrystalBlock::get_type() {
  return CRYSTAL_BLOCK;
}

/**
 * @brief Returns whether this block is raised.
 * @return true if this block is raised
 */
bool CrystalBlock::is_raised() {

  return (subtype == ORANGE && orange_raised) ||
    (subtype == BLUE && !orange_raised);
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool CrystalBlock::is_obstacle_for(MapEntity &other) {

  // if this block is lowered, it is obviously not an obstacle
  if (!is_raised()) {
    return false;
  }

  // if the block is raised, only some specific entities may overlap it
  return other.is_raised_block_obstacle(*this);
}

/**
 * @brief This function is called when another entity overlaps this crystal block.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void CrystalBlock::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping.is_hero() && is_raised()) {

    // see if we have to make fim fall

    Hero &hero = (Hero&) entity_overlapping;
    if (hero.can_control_movement()) {

      Rectangle collision_box = hero.get_bounding_box();
      int x1 = get_top_left_x();
      int x2 = x1 + get_width();
      int y1 = get_top_left_y();
      int y2 = y1 + get_height();
      int jump_direction = 0;
      int jump_length = 0;
      bool jumped = false;

      const Rectangle &hero_center = hero.get_center_point();

      if (hero_center.get_y() < y1) {
	// fall to the north
	collision_box.set_y(y1 - 16);
	jump_direction = 2;
	jump_length = hero.get_top_left_y() + 16 - y1;
	jumped = try_jump(hero, collision_box, jump_direction, jump_length);
      }
      else if (hero_center.get_y() >= y2) {
	// fall to the south
	collision_box.set_y(y2);
	jump_direction = 6;
	jump_length = y2 - hero.get_top_left_y();
	jumped = try_jump(hero, collision_box, jump_direction, jump_length);
      }

      if (!jumped) {
	if (hero_center.get_x() >= x2) {
	  // fall to the east
	  collision_box.set_x(x2);
	  jump_direction = 0;
	  jump_length = x2 - hero.get_top_left_x();
	  try_jump(hero, collision_box, jump_direction, jump_length);
	}
	else if (hero_center.get_x() < x1) {
	  // fall to the west
	  collision_box.set_x(x1 - 16);
	  jump_direction = 4;
	  jump_length = hero.get_top_left_x() + 16 - x1;
	  try_jump(hero, collision_box, jump_direction, jump_length);
	}
      }
    }
  }
}

/**
 * @brief Makes the hero leave this raised crystal block if possible.
 * @param hero the hero
 * @param collision_box destination wanted for the jump
 * @param jump_direction direction of the jump
 * @param jump_length length of the jump
 * @return true if the jump was done, i.e. if the collision box was overlapping
 * no obstacle and no raised crystal block.
 */
bool CrystalBlock::try_jump(Hero &hero, const Rectangle &collision_box,
				  int jump_direction, int jump_length) {

  // jump if there is no collision and no other raised crystal blocks
  if (!get_map().test_collision_with_obstacles(get_layer(), collision_box, hero)
      && !get_entities().overlaps_raised_blocks(get_layer(), collision_box)) {

    hero.start_jumping(jump_direction, jump_length, true, false);
    Sound::play("hero_lands");
    return true;
  }

  return false;
}

/**
 * @brief Updates the entity.
 */
void CrystalBlock::update() {

  // see if the state has to be changed
  bool orange_raised = get_game().get_crystal_state();
  if (orange_raised != this->orange_raised) {

    this->orange_raised = orange_raised;

    if (subtype == ORANGE) {
      get_sprite().set_current_animation(orange_raised ? "orange_raised" : "orange_lowered");
    }
    else {
      get_sprite().set_current_animation(orange_raised ? "blue_lowered" : "blue_raised");
    }
  }
  get_sprite().update();

  MapEntity::update();
}

/**
 * @brief Displays the entity on the map.
 * This is a redefinition of MapEntity::display_on_map to repeat the block pattern.
 */
void CrystalBlock::display_on_map() {

  Sprite &sprite = get_sprite();

  int x1 = get_top_left_x();
  int y1 = get_top_left_y();
  int x2 = x1 + get_width();
  int y2 = y1 + get_height();

  for (int y = y1; y < y2; y += 16) {
    for (int x = x1; x < x2; x += 16) {
      get_map().display_sprite(sprite, x, y);
    }
  }
}

