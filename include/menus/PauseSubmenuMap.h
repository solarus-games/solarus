#ifndef PAUSE_SUBMENU_MAP_H
#define PAUSE_SUBMENU_MAP_H

#include "Common.h"
#include "menus/PauseSubmenu.h"

/**
 * The Map submenu of the pause menu.
 */
class PauseSubmenuMap: public PauseSubmenu {

 private:

  static const SDL_Rect outside_world_minimap_size;

  // data
  Dungeon *dungeon;            /**< the dungeon whose map is displayed, or NULL if we are not in a dungeon */
  SDL_Rect link_position;      /**< position of Link on the minimap */

  // graphics
  Sprite *link_head_sprite;
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

 public:

  PauseSubmenuMap(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuMap(void);

  void key_pressed(const SDL_keysym &keysym);
  void update(void);
  void display(SDL_Surface *destination);
  void display_world_map(SDL_Surface *destination);
  void display_dungeon_map(SDL_Surface *destination);
};

#endif
