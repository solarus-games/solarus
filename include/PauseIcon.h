#ifndef ZSDX_PAUSE_ICON_H
#define ZSDX_PAUSE_ICON_H

#include "Common.h"
#include "HudElement.h"
#include "KeysEffect.h"

/**
 * This class displays the pause icon.
 */
class PauseIcon: public HudElement {

 private:

  // data object to observe
  KeysEffect *keys_effect;

  // static icon
  PauseKeyEffect pause_key_effect_displayed;
  SDL_Surface *img_pause_icon;

  // icon flipping
  bool is_flipping;
  AnimatedSprite *sprite_pause_icon;

 public:

  PauseIcon(KeysEffect *keys_effect, int x, int y);
  ~PauseIcon(void);
  
  void update(void);
  void rebuild(void);

};

#endif
