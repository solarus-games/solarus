#include "hud/SmallKeysCounter.h"
#include "Counter.h"
#include "Equipment.h"
#include "ResourceManager.h"

/**
 * Constructor.
 * @param equipment the player's equipment
 * @param x x coordinate of the top-left corner of the key icon on the destination surface
 * @param y y coordinate of the top-left corner of the key icon on the destination surface
 */
SmallKeysCounter::SmallKeysCounter(Equipment *equipment, int x, int y):
  HudElement(x, y, 40, 16), equipment(equipment), counter(NULL) {

  img_icon = ResourceManager::load_image("hud/message_and_treasure_icons.png");
  counter = NULL;

  rebuild();
}

/**
 * Destructor.
 */
SmallKeysCounter::~SmallKeysCounter(void) {
  SDL_FreeSurface(img_icon);

  if (counter != NULL) {
    delete counter;
  }
}

/**
 * Updates the number of rupees displayed and the color of the icon.
 */
void SmallKeysCounter::update(void) {

  bool need_rebuild = false;

  // enable or disable the counter
  if (counter == NULL && equipment->are_small_keys_enabled()) {
    counter = new Counter(2, false, 10, 4);
    need_rebuild = true;
  }
  else if (counter != NULL && !equipment->are_small_keys_enabled()) {
    delete counter;
    counter = NULL;
    need_rebuild = true;
  }

  if (equipment->are_small_keys_enabled()) {

    // current small keys
    int nb_keys = equipment->get_small_keys();
    int nb_keys_displayed = counter->get_value();

    if (nb_keys_displayed != nb_keys) {
      counter->set_value(nb_keys);
      need_rebuild = true;
    }
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * Redraws the icon and the counter on the surface.
 */
void SmallKeysCounter::rebuild(void) {

  HudElement::rebuild();

  if (counter != NULL) { // the small keys are enabled

    // small key icon
    static SDL_Rect src_position = {36, 112, 8, 16};
    SDL_BlitSurface(img_icon, &src_position, surface_drawn, NULL);

    // current number of small keys
    counter->display(surface_drawn);
  }
}
