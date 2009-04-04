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
#ifndef ZSDX_HUD_ELEMENT_H
#define ZSDX_HUD_ELEMENT_H

#include "Common.h"

/**
 * Abstract class for displaying an element of the HUD.
 * The HUD calls update() repeatedly. The update() function
 * should check whether the information displayed has changed,
 * and call rebuild() if necessary.
 * The rebuild() function build the surface of this HUD element.
 */
class HudElement {

 protected:

  /**
   * The surface on which the hud element is drawn when rebuild() is called.
   * When display() is called, this surface is blitted
   * on the destination surface (usually the screen).
   */
  SDL_Surface *surface_drawn;

  /**
   * Position of this hud element on the destination surface.
   */
  SDL_Rect destination_position;

  bool visible;
  
  HudElement(int x, int y, int width, int height);

 public:

  virtual ~HudElement(void);

  void set_position(int x, int y);
  void set_visible(bool visible);
  virtual bool is_visible(void);

  virtual void update(void) = 0;
  virtual void rebuild(void);
  void display(SDL_Surface *destination);
};

#endif
