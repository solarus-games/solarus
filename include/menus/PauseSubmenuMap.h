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
  Dungeon *dungeon;           /**< the dungeon whose map is displayed, or NULL if we are not in a dungeon */
  SDL_Rect link_position;     /**< position of Link on the minimap */

  // graphics
  SDL_Surface *world_map_img;
  Sprite *link_head_sprite;

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
