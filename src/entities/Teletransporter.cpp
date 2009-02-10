#include "entities/Teletransporter.h"
#include "ZSDX.h"
#include "Game.h"
#include "Sprite.h"
#include "Map.h"
#include "entities/Hero.h"

/**
 * Constructor.
 * @param name name of the teletransporter
 * @param layer layer of the teletransporter
 * @param x x position of the teletransporter's rectangle
 * @param y y position of the teletransporter's rectangle
 * @param width width of the teletransporter's rectangle
 * @param height height of the teletransporter's rectangle
 * @param transition_style style of transition between the two maps
 * @param destination_map_id id of the destination map
 * @param destination_point_name location on the destination map, or "_same" to keep the hero's coordinates,
 * or "_side" to place the heroon the appropriate side of the map
 */
Teletransporter::Teletransporter(string name, MapEntity::Layer layer, int x, int y, int width, int height,
				 Subtype subtype, Transition::Style transition_style,
				 MapId destination_map_id, string destination_point_name):
  Detector(COLLISION_ORIGIN_POINT | COLLISION_FACING_POINT, name, layer, x, y, width, height),
  subtype(subtype), transition_style(transition_style),
  destination_map_id(destination_map_id), destination_point_name(destination_point_name) {
  
  if (subtype == YELLOW) {
    create_sprite("entities/teletransporter");
    get_sprite()->set_current_animation("yellow");
  }
  else {
    // TODO
  }
}

/**
 * Destructor.
 */
Teletransporter::~Teletransporter(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType Teletransporter::get_type() {
  return TELETRANSPORTER;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Teletransporter::is_obstacle_for(MapEntity *other) {

  EntityType type = other->get_type();

  if (type == ENEMY || type == NPC) {
    return true;
  }

  if (type == HERO) {
    return ((Hero*) other)->is_teletransporter_obstacle(this);
  }

  return false;
}

/**
 * This function is called by the engine when an entity overlaps the teletransporter.
 * This is a redefinition of Detector::collision().
 * The map is not notified anymore: here we just make the hero leave the map.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Teletransporter::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  entity_overlapping->collision_with_teletransporter(this, collision_mode);
}

/**
 * Makes the teletransporter move the hero to the destination.
 * @param hero the hero
 */
void Teletransporter::transport_hero(Hero *hero) {

  string name = destination_point_name;

  if (destination_point_name == "_side") {

    // special desination point: side of the map
    // we determine the appropriate side based on the teletransporter location 

    int x = get_x();
    int y = get_y();

    if (get_width() == 16 && x == 0) {
      name += '0';
    }
    else if (get_width() == 16 && x == map->get_width() - 16) {
      name += '2';
    }
    else if (get_height() == 16 && y == 0) {
      name += '3';
    }
    else if (get_height() == 16 && y == map->get_height() - 16) {
      name += '1';
    }
    else {
      DIE("Bad position of teletransporter '" << get_name() << "'"); 
      /*
      // dangerous because of diagonal movements
      int direction = (hero->get_movement_direction() / 90 + 2) % 4; 
      name += ('0' + direction);
      */
    }
  }

  zsdx->game->set_current_map(destination_map_id, name, transition_style);
}
