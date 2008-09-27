#ifndef ZSDX_SMALL_KEYS_COUNTER_H
#define ZSDX_SMALL_KEYS_COUNTER_H

#include "Common.h"
#include "HudElement.h"

/**
 * This class displays the small key counter and its icon
 * when the small keys are enabled in the current map.
 */
class SmallKeysCounter: public HudElement {

 private:

  Game *game;
  Equipment *equipment;
  
  SDL_Surface *img_icon;
  Counter *counter;
  
 public:

  SmallKeysCounter(Equipment *equipment, int x, int y);
  ~SmallKeysCounter(void);
  
  void update(void);
  void rebuild(void);
};

#endif
