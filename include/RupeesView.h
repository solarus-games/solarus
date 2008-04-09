#ifndef ZSDX_RUPEES_VIEW_H
#define ZSDX_RUPEES_VIEW_H

#include "Common.h"
#include "HudElement.h"

/**
 * This class displays the rupee counter and its icon.
 */
class RupeesView: public HudElement {

 private:

  Equipment *equipment;
  
  // rectangles of the rupee icons in the image
  static SDL_Rect rupee_icon_positions[3];

  SDL_Surface *img_rupee_icon;

  int nb_max_rupees_displayed;
  int icon_displayed;
  CounterView *rupee_counter;
 
  Uint32 next_rupee_update_date;
  
 public:

  RupeesView(Equipment *equipment, int x, int y);
  ~RupeesView(void);
  
  void update(void);
  void rebuild(void);
};

#endif
