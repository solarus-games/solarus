#ifndef ZSDX_FLOOR_VIEW_H
#define ZSDX_FLOOR_VIEW_H

#include "Common.h"
#include "HudElement.h"

/**
 * Displays the floor of the current map.
 */
class FloorView: public HudElement {

 private:

  Game *game;
  Map *current_map;
  SDL_Surface *img_floors;

  bool is_floor_displayed;
  Uint32 hide_floor_date;

 public:

  FloorView(Game *game, int x, int y);
  ~FloorView(void);

  void update(void);
  void rebuild(void);
};

#endif
