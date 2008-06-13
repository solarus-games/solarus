#ifndef ZSDX_ENTITY_DETECTOR_H
#define ZSDX_ENTITY_DETECTOR_H

#include "Common.h"
#include "MapEntity.h"

/**
 * An entity detector is an object placed somewhere on the map
 * to detect the presence of Link or other moving entities.
 * Examples of entity detectors are exits and switches.
 *
 * An entity is considered to be on a detector if its origin point
 * is inside the detector's rectangle.
 */
class EntityDetector: public MapEntity {

 protected:
  
  EntityDetector(string name, Layer layer, int x, int y, int width, int height);

  virtual void entity_overlaps(MapEntity *entity_overlapping);

 public:

  virtual ~EntityDetector(void);

  void check_entity_overlapping(MapEntity *entity);

};

#endif
