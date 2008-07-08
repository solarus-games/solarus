#ifndef ZSDX_CARRIED_ITEM_H
#define ZSDX_CARRIED_ITEM_H

#include "Common.h"
#include "MapEntity.h"

/**
 * Represents an item (pot, bush...) carried by Link. He can throw it.
 * The item is attached to Link, not to the map.
 */
class CarriedItem: public MapEntity {

 private:
  
  Link *link;
  bool is_lifting;

 public:

  CarriedItem(Link *link, TransportableItem *transportable_item);
  ~CarriedItem(void);

  void set_stopped(void);
  void set_walking(void);
  void update(void);
};

#endif
