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

  SDL_Surface *img_floors;

 public:

  FloorView(Game *game, int x, int y);
  ~FloorView(void);

  void update(void);
  void rebuild(void);
};

#endif
