/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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
