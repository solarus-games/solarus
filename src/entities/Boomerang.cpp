#include "entities/Boomerang.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "movements/CollisionMovement.h"
#include "movements/PlayerMovement.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Map.h"

/**
 * Creates a boomerang.
 * @param hero the hero
 */
Boomerang::Boomerang(Hero *hero):
  MapEntity() {

  // initialize the entity
  set_layer(hero->get_layer());
  create_sprite("entities/boomerang");
  set_origin(8, 8);
  set_coordinates(hero->get_coordinates());
  set_rectangle_from_sprite();
  set_map(hero->get_map());

  // determine the boomerang direction
  int boomerang_direction = hero->get_animation_direction() * 90;
  // TODO take into account diagonal directions (but we cannot use PlayerMovement since the movement is disabled)
  
  CollisionMovement *movement = new CollisionMovement();
  movement->set_speed(24);
  movement->set_direction(boomerang_direction);
  set_movement(movement);

  next_sound_date = SDL_GetTicks();
}

/**
 * Destructor.
 */
Boomerang::~Boomerang(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Boomerang::get_type() {
  return BOOMERANG;
}

/**
 * Returns whether entities of this type can be obstacles for other entities.
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 * @return true if this type of entity can be obstacle for other entities
 */
bool Boomerang::can_be_obstacle(void) {
  return false; 
}

/**
 * Returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * collision() will be called when a collision is detected.
 * @return true if this type of entity can detect other entities
 */
bool Boomerang::can_detect_entities(void) {
  return false;
}

/**
 * Returns whether entities of this type can be displayed.
 * If enabled, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 * @return true if this type of entity can be displayed
 */
bool Boomerang::can_be_displayed(void) {
  return true; 
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Boomerang::is_displayed_in_y_order(void) {
  return true;
}

/**
 * Updates the boomerang.
 */
void Boomerang::update(void) {

  MapEntity::update();

  Uint32 now = SDL_GetTicks();
  if (now >= next_sound_date) {
    ResourceManager::get_sound("boomerang")->play();
    next_sound_date = now + 150;
  }

  if (get_movement()->is_stopped()) {
    // collision with an obstacle
    get_map()->get_entities()->remove_entity(this);
  }
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Boomerang::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return false;
}

/**
 * Returns whether a water tile is currently considered as an obstacle for this entity.
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool Boomerang::is_water_obstacle(void) {
  return false;
}

/**
 * Returns whether a hole is currently considered as an obstacle for this entity.
 * @return true if the holes are currently an obstacle for this entity
 */
bool Boomerang::is_hole_obstacle(void) {
  return false;
}

