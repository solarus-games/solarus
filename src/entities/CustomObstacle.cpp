#include "entities/CustomObstacle.h"

/**
 * Constructor.
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
CustomObstacle::CustomObstacle(std::string name, MapEntity::Layer layer, int x, int y, int width, int height,
			       bool stops_hero, bool stops_enemies, bool stops_npcs, bool stops_blocks):
  MapEntity(name, 0, layer, x, y, width, height) {

  entity_types_stopped[HERO] = stops_hero;
  entity_types_stopped[ENEMY] = stops_enemies;
  entity_types_stopped[INTERACTIVE_ENTITY] = stops_npcs;
  entity_types_stopped[BLOCK] = stops_blocks;
}

/**
 * Destructor.
 */
CustomObstacle::~CustomObstacle(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType CustomObstacle::get_type(void) {
  return CUSTOM_OBSTACLE;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool CustomObstacle::is_obstacle_for(MapEntity *other) {
  return entity_types_stopped[other->get_type()];
}
