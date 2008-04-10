#ifndef ZSDX_ACTION_ICON_H
#define ZSDX_ACTION_ICON_H

#include "Common.h"
#include "HudElement.h"

/**
 * This class displays the action icon.
 */
class ActionIcon: public HudElement {

 private:

  KeysEffect *keys_effect;

  AnimatedSprite *action_icon_sprite;

 public:

  ActionIcon(KeysEffect *keys_effect, int x, int y);
  ~ActionIcon(void);
  
  void update(void);
  void rebuild(void);

};

#endif
