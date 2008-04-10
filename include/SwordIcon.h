#ifndef ZSDX_SWORD_ICON_H
#define ZSDX_SWORD_ICON_H

#include "Common.h"
#include "HudElement.h"
#include "KeysEffect.h"

/**
 * This class displays the sword icon.
 */
class SwordIcon: public HudElement {

 private:

  // data objects to observe
  KeysEffect *keys_effect;
  Equipment *equipment;

  // static icon
  SwordKeyEffect sword_key_effect_displayed;
  int sword_number_displayed;
  SDL_Surface *img_sword_icon;

  // icon flipping
  bool is_flipping;
  AnimatedSprite *sprite_sword_icon;

 public:

  SwordIcon(KeysEffect *keys_effect, Equipment *equipment, int x, int y);
  ~SwordIcon(void);
  
  void update(void);
  void rebuild(void);

};

#endif
