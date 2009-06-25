#ifndef ZSDX_BOOMERANG_H
#define ZSDX_BOOMERANG_H

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * A boomerang thrown on the map.
 */
class Boomerang: public MapEntity {

 public:  

  Boomerang(Hero *hero);
  ~Boomerang(void);

  EntityType get_type(void);

  bool can_be_obstacle(void);
  bool can_detect_entities(void);
  bool can_be_displayed(void);
  bool is_displayed_in_y_order(void);
};


#endif
 
