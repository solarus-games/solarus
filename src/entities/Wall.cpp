/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Wall.h"
#include "solarus/entities/Hero.h"
#include "solarus/lowlevel/QuestFiles.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param name name of the wall to create
 * \param layer layer of the Wall to create
 * \param xy Coordinates where to create the wall.
 * \param size Size of the Wall to create.
 * \param stops_hero true to make this entity an obstacle for the hero
 * \param stops_enemies true to make this entity an obstacle for enemies
 * \param stops_npcs true to make this entity an obstacle for NPCs
 * \param stops_blocks true to make this entity an obstacle for blocks
 * \param stops_projectiles true to make this entity an obstacle for thrown
 * items, arrows, hookshots and boomerangs.
 */
Wall::Wall(
    const std::string& name,
    Layer layer,
    const Point& xy,
    const Size& size,
    bool stops_hero,
    bool stops_enemies,
    bool stops_npcs,
    bool stops_blocks,
    bool stops_projectiles
):
  MapEntity(name, 0, layer, xy, size) {

  if (stops_hero) {
    entity_types_stopped.insert(EntityType::HERO);
  }
  if (stops_enemies) {
    entity_types_stopped.insert(EntityType::ENEMY);
  }
  if (stops_npcs) {
    entity_types_stopped.insert(EntityType::NPC);
  }
  if (stops_blocks) {
    entity_types_stopped.insert(EntityType::BLOCK);
  }
  if (stops_projectiles) {
    entity_types_stopped.insert(EntityType::CARRIED_ITEM);
    entity_types_stopped.insert(EntityType::ARROW);
    entity_types_stopped.insert(EntityType::HOOKSHOT);
    entity_types_stopped.insert(EntityType::BOOMERANG);
  }
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Wall::get_type() const {
  return EntityType::WALL;
}

/**
 * \brief Returns whether entities of this type can be drawn.
 * \return true if this type of entity can be drawn
 */
bool Wall::can_be_drawn() const {
  return false;
}

/**
 * \brief Returns whether this entity is an obstacle for another one
 * when it is enabled.
 * \param other another entity
 * \return true if this entity is an obstacle for the other one
 */
bool Wall::is_obstacle_for(MapEntity& other) {

  const auto it = entity_types_stopped.find(other.get_type());
  return it != entity_types_stopped.end();
}

}

