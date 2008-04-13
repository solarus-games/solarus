#ifndef ZSDX_MAGIC_BAR_H
#define ZSDX_MAGIC_BAR_H

#include "Common.h"
#include "HudElement.h"

/**
 * This class displays the magic bar on the screen.
 */
class MagicBar: public HudElement {
  
 private:

  // the data object observed
  Equipment *equipment;
  
  // static image representing the current level of the magic bar
  int current_magic_displayed;
  Uint32 next_magic_update_date;
  bool is_magic_decreasing;
  SDL_Surface *img_magic_bar;
  Sound *sound_magic_bar;

  // sprite representing the magic bar container
  int max_magic_displayed;
  AnimatedSprite *sprite_magic_bar_container;

 public:

  MagicBar(Equipment *equipment, int x, int y);
  ~MagicBar(void);

  void update(void);
  void rebuild(void);
  bool is_visible(void);  
};

#endif
