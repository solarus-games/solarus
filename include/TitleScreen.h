#ifndef ZSDX_TITLE_SCREEN_H
#define ZSDX_TITLE_SCREEN_H

#include "Common.h"

/**
 * This class shows the title screen of the game.
 */
class TitleScreen {

 private:

  void phase_1_black_screen(void);
  void phase_2_zs_presents(void);
  void phase_3_title(void);

 public:

  TitleScreen(void);
  ~TitleScreen(void);

  void show(void);
};

#endif
