#ifndef ZSDX_SWORD_ICON_H
#define ZSDX_SWORD_ICON_H

#include "Common.h"
#include "hud/HudElement.h"
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
  KeysEffect::SwordKeyEffect sword_key_effect_displayed;
  int sword_number_displayed;
  bool is_enabled;
  SDL_Surface *img_sword_icon;

  // icon flipping
  bool is_flipping;
  Sprite *sprite_sword_icon;

 public:

  SwordIcon(KeysEffect *keys_effect, Equipment *equipment, int x, int y);
  ~SwordIcon(void);

  bool is_visible(void);
  void update(void);
  void rebuild(void);

};

#endif
