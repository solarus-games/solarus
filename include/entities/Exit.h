#ifndef ZSDX_EXIT_H
#define ZSDX_EXIT_H

#include "Common.h"
#include "Detector.h"
#include "Transition.h"

/**
 * A map exit is a detector placed on a map.
 * When Link walks on this detector, he leaves the map and enters another one.
 */
class Exit: public Detector {

 private:

  /**
   * Type of transition between the two maps.
   */
  Transition::Style transition_style;

  /**
   * Id of the destination map.
   */
  MapId destination_map_id;

  /**
   * Initial point of the destination map.
   */
  string entrance_name;

 public:

  Exit(string name, Layer layer, int x, int y, int width, int height,
       Transition::Style transition_style, MapId destination_map_id, string entrance_name);
  ~Exit(void);

  void collision(MapEntity *entity_overlapping);

};

#endif
