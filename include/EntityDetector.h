#ifndef ZSDX_ENTITY_DETECTOR_H
#define ZSDX_ENTITY_DETECTOR_H

#include "Common.h"
#include "MapEntity.h"

/**
 * Collisions modes between an entity and the detector.
 */
enum CollisionMode {
  COLLISION_WITH_ENTITY_ORIGIN,       /**< collision if the entity's origin point is inside the detector's rectangle */
  COLLISION_WITH_ENTITY_RECTANGLE,    /**< collision if the entity's rectangle overlaps the detector's rectangle */
  COLLISION_WITH_ENTITY_FACING_POINT, /**< collision if the entity's facing point overlaps the detector's rectangle */
};

/**
 * An entity detector is an object placed somewhere on the map
 * to detect the presence of Link or other moving entities.
 * Examples of entity detectors are exits and switches.
 *
 * For now, there are two possible collision modes:
 * - The origin point of the entity is inside the detector's rectangle.
 * This means the entity really has to be on the detector. Example: a switch, an exit.
 * - The entity's rectangle overlaps the detector's rectangle.
 * This means the entity just has to touch the detector. Example:
 * a pickable item, an enemy.
 *
 * Other collision modes will be implemented to handle detectors with obstacles
 * (such as pots, chests...) and pixel precise collisions.
 */
class EntityDetector: public MapEntity {

 private:

  const CollisionMode mode;

  bool layer_ignored;
  
  bool check_collision_origin(MapEntity *entity);
  bool check_collision_rectangle(MapEntity *entity);
  bool check_collision_facing_point(MapEntity *entity);
  
 protected:

  EntityDetector(CollisionMode collision_mode, string name, Layer layer,
		 int x, int y, int width, int height);

  void set_layer_ignored(bool layer_ignored);

  virtual void entity_collision(MapEntity *entity_overlapping);

 public:

  virtual ~EntityDetector(void);

  virtual void check_entity_collision(MapEntity *entity);
  virtual void action_key_pressed(Map *map);

};

#endif
