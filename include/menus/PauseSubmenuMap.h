#ifndef PAUSE_SUBMENU_MAP_H
#define PAUSE_SUBMENU_MAP_H

#include "Common.h"
#include "menus/PauseSubmenu.h"
#include "Controls.h"

/**
 * The Map submenu of the pause menu.
 */
class PauseSubmenuMap: public PauseSubmenu {

 private:

  static const SDL_Rect outside_world_minimap_size;

  // data
  Equipment *equipment;        /**< general equipment */
  DungeonEquipment *dungeon_equipment;        /**< dungeon equipment */
  Dungeon *dungeon;            /**< the dungeon whose map is displayed, or NULL if we are not in a dungeon */
  SDL_Rect hero_position;      /**< position of the hero on the minimap */

  // graphics
  Sprite *hero_head_sprite;
  Sprite *up_arrow_sprite;
  Sprite *down_arrow_sprite;

  // outside world map
  SDL_Surface *world_map_img;
  int world_minimap_visible_y; /**< when the world map is displayed: y position of the minimap visible rectangle */
  int moving_visible_y;        /**< 0, -1 or 1 depending on the arrow pressed */
  Uint32 next_moving_visible_y_date;

  // dungeon map
  SDL_Surface *dungeon_map_background;
  SDL_Surface *dungeon_map_icons;
  Counter *small_keys_counter;

  SDL_Surface *dungeon_floors_img;
  int hero_floor, boss_floor, highest_floor, lowest_floor, nb_floors;
  int nb_floors_displayed, highest_floor_displayed, selected_floor;

  SDL_Surface *dungeon_map_img;
  Sprite *hero_point_sprite;

  void load_dungeon_map_image(void);

 public:

  PauseSubmenuMap(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuMap(void);

  void key_pressed(Controls::GameKey key);
  void update(void);
  void display(SDL_Surface *destination);
  void display_world_map(SDL_Surface *destination);
  void display_dungeon_map(SDL_Surface *destination);
  void display_dungeon_items(SDL_Surface *destination);
  void display_dungeon_floors(SDL_Surface *destination);
};

#endif
