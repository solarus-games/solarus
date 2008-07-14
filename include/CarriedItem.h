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
  bool is_throwing;
  bool is_breaking;
  Sound *breaking_sound;
  Sprite *shadow_sprite;


 public:

  CarriedItem(Link *link, TransportableItem *transportable_item);
  ~CarriedItem(void);

  void set_stopped(void);
  void set_walking(void);
  void throw_item(Map *map, int direction);
  bool is_broken(void);
  
  void set_suspended(bool suspended);
  void update(void);
  void display_on_map(Map *map);
};

#endif
