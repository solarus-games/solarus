#ifndef ZSDX_ENTITY_DETECTOR_H
#define ZSDX_ENTITY_DETECTOR_H

#include "Common.h"
#include "MapEntity.h"

/**
 * An entity detector is an object placed somewhere on the map
 * to detect the presence of Link or other moving entities.
 * Examples of entity detectors are exits and switches.
 */
class EntityDetector: public MapEntity {

 protected:
  
  EntityDetector(string name, Layer layer, int x, int y, int width, int height);

 public:

  virtual ~EntityDetector(void);

  virtual void display_on_map(Map *map);
  virtual void entity_overlaps(MapEntity *entity_overlapping);
};

#endif
