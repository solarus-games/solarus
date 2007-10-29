/**
 * This module defines the class EntityDetector.
 */

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
  
  /**
   * Constructor.
   * @param layer layer of the detector
   * @param x x position of the detector's rectangle
   * @param y y position of the detector's rectangle
   * @param width width of the detector's rectangle
   * @param height height of the detector's rectangle
   */
  EntityDetector(Layer layer, int x, int y, int width, int height);

 public:

  /**
   * Destructor.
   */
  virtual ~EntityDetector(void) { };

  /**
   * Displays the entity on the map.
   * This function does nothing because by default, an entity detector is invisible.
   * @param map the map
   */
  virtual void display_on_map(Map *map);

  /**
   * This function is called by the engine when an entity overlaps the detector.
   * @param entity_overlapping the entity overlapping the detector
   */
  virtual void entity_overlaps(MapEntity *entity_overlapping);

};

#endif
