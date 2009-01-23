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
  
  Map *map;
  Link *link;
  bool is_lifting;
  bool is_throwing;
  bool is_breaking;
  Sound *destruction_sound;
  Sprite *shadow_sprite;
  int throwing_direction;
  int damage_on_enemies;

 public:

  CarriedItem(Link *link, DestructibleItem *destructible_item);
  ~CarriedItem(void);

  EntityType get_type(void);

  int get_damage_on_enemies(void);

  void set_animation_stopped(void);
  void set_animation_walking(void);
  void throw_item(Map *map, int direction);
  bool is_broken(void);
  
  void set_suspended(bool suspended);
  void update(void);
  void display_on_map(void);

  virtual void collision_with_enemy(Enemy *enemy);
};

#endif
