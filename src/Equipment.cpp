/**
 * This module defines the class Equipment.
 */

#include "Equipment.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "Link.h"

/**
 * Constructor.
 */
Equipment::Equipment(Link *link):
  link(link), savegame(zsdx->game->get_savegame()) {
}

/**
 * Destructor.
 */
Equipment::~Equipment(void) {

}

/**
 * Returns the number of Link's tunic.
 * @return Link's tunic number (0: green tunic,
 * 1: blue tunic, 2: red tunic)
 */
int Equipment::get_tunic_number(void) {
  return savegame->get_reserved_integer(SAVEGAME_LINK_TUNIC);
}

/**
 * Sets Link's tunic.
 * The savegame data is modified.
 * @param tunic_number Link's tunic number (0: green tunic,
 * 1: blue tunic, 2: red tunic)
 */
void Equipment::set_tunic_number(int tunic_number) {

  if (tunic_number != get_tunic_number()) {
    // the tunic has changed

    if (tunic_number < 0 || tunic_number > 2) {
      cerr << "Illegal tunic number: " << tunic_number << endl;
      exit(1);
    }

    savegame->set_reserved_integer(SAVEGAME_LINK_TUNIC, tunic_number);
    link->initialize_sprites(); // reinitialize Link's sprites
  }
}

/**
 * Returns whether Link has a sword.
 * @return true if Link has a sword, i.e. if get_sword_number() > 0
 */
bool Equipment::has_sword(void) {
  return get_sword_number() > 0;
}

/**
 * Returns the number of Link's sword.
 * @return Link's sword number (0: no sword,
 * 1 to 4: sword 1 to 4)
 */
int Equipment::get_sword_number(void) {
  return savegame->get_reserved_integer(SAVEGAME_LINK_SWORD);
}

/**
 * Sets Link's sword.
 * The savegame data is modified.
 * @param sword_number Link's sword number (0: no sword,
 * 1 to 4: sword 1 to 4)
 */
void Equipment::set_sword_number(int sword_number) {

  if (sword_number != get_sword_number()) {
    // the sword has changed

    if (sword_number < 0 || sword_number > 4) {
      cerr << "Illegal sword number: " << sword_number << endl;
      exit(1);
    }

    savegame->set_reserved_integer(SAVEGAME_LINK_SWORD, sword_number);
    link->initialize_sprites(); // reinitialize Link's sprites
  }
}

/**
 * Returns whether Link has a shield.
 * @return true if Link has a shield, i.e. if get_shield_number() > 0
 */
bool Equipment::has_shield(void) {
  return get_shield_number() > 0;
}

/**
 * Returns the number of Link's shield.
 * @return Link's shield number (0: no shield,
 * 1 to 3: shield 1 to 3)
 */
int Equipment::get_shield_number(void) {
  return savegame->get_reserved_integer(SAVEGAME_LINK_SHIELD);
}

/**
 * Sets Link's shield.
 * The savegame data is modified.
 * @param shield_number Link's shield number (0: no shield,
 * 1 to 3: shield 1 to 3)
 */
void Equipment::set_shield_number(int shield_number) {

  if (shield_number != get_shield_number()) {
    // the shield has changed

    if (shield_number < 0 || shield_number > 3) {
      cerr << "Illegal shield number: " << shield_number << endl;
      exit(1);
    }

    savegame->set_reserved_integer(SAVEGAME_LINK_SHIELD, shield_number);
    link->initialize_sprites();
  }
}
