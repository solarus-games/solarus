#ifndef ZSDX_TITLE_SCREEN_H
#define ZSDX_TITLE_SCREEN_H

#include "Common.h"

/**
 * This class shows the title screen of the game.
 */
class TitleScreen {

 private:

  bool phase_1_black_screen(void);
  bool phase_2_zs_presents(void);
  bool phase_3_title(void);

 public:

  TitleScreen(void);
  ~TitleScreen(void);

  bool show(void);
};

#endif
