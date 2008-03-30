/**
 * This module defines the class RupeesView.
 */

#include "RupeesView.h"
#include "FileTools.h"
#include "CounterView.h"
#include "Savegame.h"
#include "ZSDX.h"
#include "GameResource.h"
#include "Sound.h"
#include "Color.h"

/**
 * Position of the 3 rupee icons in the image.
 */
SDL_Rect RupeesView::rupee_icon_positions[3] = {
  {0, 0, 12, 12},
  {12, 0, 12, 12},
  {24, 0, 12, 12}
};

/**
 * Constructor.
 * @param x x coordinate of the top-left corner of the rupees on the destination surface
 * @param y y coordinate of the top-left corner of the rupees on the destination surface
 */
RupeesView::RupeesView(Savegame *savegame, int x, int y):
  HudElement(savegame, x, y, 48, 12),
  rupee_counter(new CounterView(3, 16, 2)),
  next_rupee_update_date(SDL_GetTicks()) {

  img_rupee_icon = FileTools::open_image("hud/rupee_icon.png");

  if (!savegame->is_empty()) {
    
    nb_max_rupees_displayed = savegame->get_reserved_integer(SAVEGAME_MAX_RUPEES);

    if (nb_max_rupees_displayed == 99) {
      icon_displayed = 0;
    }
    else if (nb_max_rupees_displayed == 199) {
      icon_displayed = 1;
    }
    else {
      icon_displayed = 2;
    }

    rupee_counter->set_value(savegame->get_reserved_integer(SAVEGAME_CURRENT_RUPEES));
    
    rebuild();
  }
}

/**
 * Destructor.
 */
RupeesView::~RupeesView(void) {
  SDL_FreeSurface(img_rupee_icon);
  delete rupee_counter;
}

/**
 * Updates the number of rupees displayed and the color of the icon.
 */
void RupeesView::update(void) {

  bool need_rebuild = false;

  // max rupees
  int nb_max_rupees = savegame->get_reserved_integer(SAVEGAME_MAX_RUPEES);
  if (nb_max_rupees != nb_max_rupees_displayed) {
    nb_max_rupees_displayed = nb_max_rupees;

    if (nb_max_rupees_displayed == 99) {
      icon_displayed = 0;
    }
    else if (nb_max_rupees_displayed == 199) {
      icon_displayed = 1;
    }
    else {
      icon_displayed = 2;
    }

    need_rebuild = true;
  }

  // current rupees
  int nb_current_rupees = savegame->get_reserved_integer(SAVEGAME_CURRENT_RUPEES);
  int nb_current_rupees_displayed = rupee_counter->get_value();

  if (nb_current_rupees != nb_current_rupees_displayed && SDL_GetTicks() > next_rupee_update_date) {

    next_rupee_update_date = SDL_GetTicks() + 40;

    if (nb_current_rupees < nb_current_rupees_displayed) {
      rupee_counter->decrement();
    }
    else {
      rupee_counter->increment();
    }

    if (nb_current_rupees_displayed % 3 == 0) {
      zsdx->game_resource->get_sound("rupee_counter")->play();
    }

    need_rebuild = true;
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * Redraws the icon and the counter on the surface.
 */
void RupeesView::rebuild(void) {

  SDL_FillRect(surface_drawn, NULL, COLOR_BLACK);
  
  // max rupees (icon)
  SDL_BlitSurface(img_rupee_icon, &rupee_icon_positions[icon_displayed], surface_drawn, NULL);

  // current rupees (counter)
  rupee_counter->display(surface_drawn);
}
