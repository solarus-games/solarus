#include "hud/RupeesCounter.h"
#include "Counter.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Sound.h"

/**
 * Position of the 3 rupee icons in the image.
 */
SDL_Rect RupeesCounter::rupee_icon_positions[3] = {
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
RupeesCounter::RupeesCounter(Equipment *equipment, int x, int y):
  HudElement(x, y, 48, 12),
  equipment(equipment),
  counter(new Counter(3, true, 16, 2)),
  next_rupee_update_date(SDL_GetTicks()) {

  img_rupee_icon = ResourceManager::load_image("hud/rupee_icon.png");

  nb_max_rupees_displayed = equipment->get_max_rupees();

  if (nb_max_rupees_displayed == 100) {
    icon_displayed = 0;
  }
  else if (nb_max_rupees_displayed == 300) {
    icon_displayed = 1;
  }
  else {
    icon_displayed = 2;
  }

  counter->set_value(equipment->get_rupees());

  rebuild();
}

/**
 * Destructor.
 */
RupeesCounter::~RupeesCounter(void) {
  SDL_FreeSurface(img_rupee_icon);
  delete counter;
}

/**
 * Updates the number of rupees displayed and the color of the icon.
 */
void RupeesCounter::update(void) {

  bool need_rebuild = false;

  // max rupees
  int nb_max_rupees = equipment->get_max_rupees();
  if (nb_max_rupees_displayed != nb_max_rupees) {
    nb_max_rupees_displayed = nb_max_rupees;

    if (nb_max_rupees_displayed == 100) {
      icon_displayed = 0;
    }
    else if (nb_max_rupees_displayed == 300) {
      icon_displayed = 1;
    }
    else {
      icon_displayed = 2;
    }

    need_rebuild = true;
  }

  // current rupees
  Uint32 nb_current_rupees = equipment->get_rupees();
  unsigned int nb_current_rupees_displayed = counter->get_value();

  if (nb_current_rupees_displayed != nb_current_rupees && SDL_GetTicks() > next_rupee_update_date) {

    next_rupee_update_date = SDL_GetTicks() + 40;
    
    // increment or decrement the counter until the right value is reached
    if (nb_current_rupees < nb_current_rupees_displayed) {
      counter->decrease();
    }
    else {
      counter->increase();
    }

    // if we have just reached the right value, we play a specific sound
    if (counter->get_value() == nb_current_rupees) {
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
void RupeesCounter::rebuild(void) {

  HudElement::rebuild();
  
  // max rupees (icon)
  SDL_BlitSurface(img_rupee_icon, &rupee_icon_positions[icon_displayed], surface_drawn, NULL);

  // current rupees (counter)
  counter->display(surface_drawn);
}
