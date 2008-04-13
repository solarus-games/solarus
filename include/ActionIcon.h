#ifndef ZSDX_ACTION_ICON_H
#define ZSDX_ACTION_ICON_H

#include "Common.h"
#include "HudElement.h"
#include "KeysEffect.h"

/**
 * This class displays the action icon.
 */
class ActionIcon: public HudElement {

 private:

  // data object to observe
  KeysEffect *keys_effect;

  // static icon
  ActionKeyEffect action_key_effect_displayed;
  bool is_enabled;
  SDL_Surface *img_action_icon;

  // icon flipping
  bool is_flipping;
  AnimatedSprite *sprite_action_icon;

  bool is_visible(void);

 public:

  ActionIcon(KeysEffect *keys_effect, int x, int y);
  ~ActionIcon(void);
  
  void update(void);
  void rebuild(void);

};

#endif
