#ifndef ZSDX_TELETRANSPORTER_H
#define ZSDX_TELETRANSPORTER_H

#include "Common.h"
#include "Detector.h"
#include "Transition.h"

/**
 * A teletransporter is a detector placed on a map.
 * When the hero walks on this detector, he is placed on a destination point
 * on the same map or another one.
 */
class Teletransporter: public Detector {

 public:

  /**
   * Subtypes of teletransporters.
   */
  enum Subtype {
    INVISIBLE = 0, /**< an invisible detector, usually to move the hero to another map */
    YELLOW    = 1, /**< a classical teletransporter */
    BLUE      = 2, /**< a teletransporter with a different color and an immediate transition */
  };

 private:

  Subtype subtype;                      /**< subtype of teletransporter */
  Transition::Style transition_style;   /**< style of transition between the two maps */
  MapId destination_map_id;             /**< id of the destination map */
  std::string destination_point_name;        /**< destination point on that map, or "_same" to keep the hero's coordinates,
					 * or "_side" to place the hero on the appropriate side of the map */
  int destination_side;                 /**< when the destination point is "_side", indicates which side
					 * of the destination map this teletransporters leads to
					 * (this depends on the teletransporter position on the map */
  int transition_direction;             /**< when the destination point is "_side", indicates the direction 
					 * of the transition between the two maps (this is the opposite
					 * direction of destination_side) */

 public:

  Teletransporter(std::string name, Layer layer, int x, int y, int width, int height,
		  Subtype subtype, Transition::Style transition_style,
		  MapId destination_map_id, std::string destination_point_name);
  ~Teletransporter(void);

  EntityType get_type(void);
  void set_map(Map *map);

  bool is_obstacle_for(MapEntity *other);
  bool check_collision_custom(MapEntity *entity);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  void transport_hero(Hero *hero);
};

#endif
