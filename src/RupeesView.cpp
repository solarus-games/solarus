#include "RupeesView.h"
#include "FileTools.h"
#include "CounterView.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Sound.h"

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
 * @param equipment the player's equipment
 * @param x x coordinate of the top-left corner of the rupees on the destination surface
 * @param y y coordinate of the top-left corner of the rupees on the destination surface
 */
RupeesView::RupeesView(Equipment *equipment, int x, int y):
  HudElement(x, y, 48, 12),
  equipment(equipment),
  rupee_counter(new CounterView(3, 16, 2)),
  next_rupee_update_date(SDL_GetTicks()) {

  img_rupee_icon = ResourceManager::load_image("hud/rupee_icon.png");

  nb_max_rupees_displayed = equipment->get_max_rupees();
  
  if (nb_max_rupees_displayed == 99) {
    icon_displayed = 0;
  }
  else if (nb_max_rupees_displayed == 199) {
    icon_displayed = 1;
  }
  else {
    icon_displayed = 2;
  }
  
  rupee_counter->set_value(equipment->get_rupees());
  
  rebuild();
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
  int nb_max_rupees = equipment->get_max_rupees();
  if (nb_max_rupees_displayed != nb_max_rupees) {
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
  Uint32 nb_current_rupees = equipment->get_rupees();
  unsigned int nb_current_rupees_displayed = rupee_counter->get_value();

  if (nb_current_rupees_displayed != nb_current_rupees && SDL_GetTicks() > next_rupee_update_date) {

    next_rupee_update_date = SDL_GetTicks() + 40;
    
    // increment or decrement the counter until the right value is reached
    if (nb_current_rupees < nb_current_rupees_displayed) {
      rupee_counter->decrease();
    }
    else {
      rupee_counter->increase();
    }

    // if we have just reached the right value, we play a specific sound
    if (rupee_counter->get_value() == nb_current_rupees) {
      ResourceManager::get_sound("rupee_counter_end")->play();
    }

    // while the counter is scrolling, play a sound (every 3 values)
    else if (nb_current_rupees_displayed % 3 == 0) {
      ResourceManager::get_sound("rupee_counter")->play();
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

  HudElement::rebuild();
  
  // max rupees (icon)
  SDL_BlitSurface(img_rupee_icon, &rupee_icon_positions[icon_displayed], surface_drawn, NULL);

  // current rupees (counter)
  rupee_counter->display(surface_drawn);
}
