#ifndef ZSDX_TELETRANSPORTER_H
#define ZSDX_TELETRANSPORTER_H

#include "Common.h"
#include "Detector.h"
#include "Transition.h"

/**
 * A teletransporter is a detector placed on a map.
 * When Link walks on this detector, he is placed on a destination point
 * on the same map or another one.
 */
class Teletransporter: public Detector {

 public:

  /**
   * Subtypes of teletransporters.
   */
  enum Subtype {
    INVISIBLE = 0, /**< an invisible detector, usually to move Link to another map */
    YELLOW    = 1, /**< a classical teletransporter */
    BLUE      = 2, /**< a teletransporter with a different color and an immediate transition */
  };

 private:

  Subtype subtype;                      /**< subtype of teletransporter */
  Transition::Style transition_style;   /**< style of transition between the two maps */
  MapId destination_map_id;             /**< id of the destination map */
  string destination_point_name;        /**< destination point on that map, or "_same" to keep Link's coordinates,
					 * or "_side" to place Link on the appropriate side of the map */

 public:

  Teletransporter(string name, Layer layer, int x, int y, int width, int height,
		  Subtype subtype, Transition::Style transition_style,
		  MapId destination_map_id, string destination_point_name);
  ~Teletransporter(void);

  EntityType get_type(void);

  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);

};

#endif
