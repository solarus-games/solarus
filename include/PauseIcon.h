#ifndef ZSDX_PAUSE_ICON_H
#define ZSDX_PAUSE_ICON_H

#include "Common.h"
#include "HudElement.h"

/**
 * This class displays the pause icon.
 */
class PauseIcon: public HudElement {

 private:

  KeysEffect *keys_effect;

  AnimatedSprite *pause_icon_sprite;

 public:

  PauseIcon(KeysEffect *keys_effect, int x, int y);
  ~PauseIcon(void);
  
  void update(void);
  void rebuild(void);

};

#endif
