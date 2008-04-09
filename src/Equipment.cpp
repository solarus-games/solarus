/**
 * This module defines the class Equipment.
 */

#include "Equipment.h"
#include "ZSDX.h"
#include "GameResource.h"
#include "Game.h"
#include "Savegame.h"
#include "Link.h"

/**
 * Constructor.
 */
Equipment::Equipment(Savegame *savegame):
  savegame(savegame) {
}

/**
 * Destructor.
 */
Equipment::~Equipment(void) {

}

/**
 * This function should be called repeatedly.
 * It handles the 
 */
void Equipment::update(void) {

}

// tunic

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
    zsdx->game_resource->get_link()->initialize_sprites(); // reinitialize Link's sprites
  }
}

// sword

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
    zsdx->game_resource->get_link()->initialize_sprites(); // reinitialize Link's sprites
  }
}

// shield

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
    zsdx->game_resource->get_link()->initialize_sprites(); // reinitialize Link's sprites
  }
}

// rupees

/**
 * Returns the maximum number of rupees of Link.
 * @return Link's maximum number of rupees (99, 199 or 999)
 */
int Equipment::get_max_rupees(void) {
  return savegame->get_reserved_integer(SAVEGAME_MAX_RUPEES);
}

/**
 * Sets the maximum number of rupees of Link.
 * @param max_rupees Link's maximum number of rupees (99, 199 or 999)
 */
void Equipment::set_max_rupees(int max_rupees) {
  
  if (max_rupees != 99 && max_rupees != 199 && max_rupees != 999) {
    cerr << "Illegal maximum number of rupees: " << max_rupees << endl;
    exit(1);
  }

  savegame->set_reserved_integer(SAVEGAME_MAX_RUPEES, max_rupees);
}

/**
 * Returns Link's current number of rupees.
 * @return Link's current number of rupees
 */
int Equipment::get_rupees(void) {
  return savegame->get_reserved_integer(SAVEGAME_CURRENT_RUPEES);
}

/**
 * Sets Link's current number of rupees.
 * Exits with an error message if the given number of rupees is not valid.
 * @param rupees Link's new number of rupees
 */
void Equipment::set_rupees(int rupees) {

  if (rupees < 0 || rupees > get_max_rupees()) {
    cerr << "Illegal number of rupees: " << rupees << endl;
    exit(1);
  }
  
  savegame->set_reserved_integer(SAVEGAME_CURRENT_RUPEES, rupees);
}

/**
 * Adds some rupees to Link.
 * If the maximum number of rupees is achieved, no more rupees are added.
 * @param rupees_to_add number of rupees to add
 */
void Equipment::add_rupees(int rupees_to_add) {

  int max_rupees = get_max_rupees();
  int total = get_rupees() + rupees_to_add;

  set_rupees(MIN(total, max_rupees));
}

/**
 * Removes some rupees to Link.
 * If the number of rupees achieves zero, no more rupees are removed.
 * @param rupees_to_remove number of rupees to remove
 */
void Equipment::remove_rupees(int rupees_to_remove) {

  int total = get_rupees() - rupees_to_remove;
  
  set_rupees(MAX(total, 0));
}

// hearts

/**
 * Returns the maximum number of hearts of Link.
 * @return Link's maximum number of rupees
 */
int Equipment::get_max_hearts(void) {
  return savegame->get_reserved_integer(SAVEGAME_MAX_HEARTS);
}

/**
 * Sets the maximum number of hearts of Link.
 * The program exits with an error message if the given maximum
 * number of hearts is not valid.
 * @param max_hearts Link's maximum number of rupees
 */
void Equipment::set_max_hearts(int max_hearts) {

  if (max_hearts <= 0 || max_hearts > 20) {
    cerr << "Illegal maximum number of hearts: " << max_hearts << endl;
    exit(1);
  }

  savegame->set_reserved_integer(SAVEGAME_MAX_HEARTS, max_hearts);
}

/**
 * Adds a heart container to Link.
 */
void Equipment::add_heart_container(void) {
  set_max_hearts(get_max_hearts() + 1);
}

/**
 * Returns the current number of hearts of Link.
 * The value returned is actually the number of heart quarters,
 * so a value of 4 means that Link has 1 heart left.
 * @return Link's current number of hearts (in heart quarters)
 */
int Equipment::get_hearts(void) {
  return savegame->get_reserved_integer(SAVEGAME_CURRENT_HEARTS);
}

/**
 * Sets the current number of hearts of Link.
 * The given value is actually the number of heart quarters,
 * so a value of 4 means that Link has 1 heart left.
 * The program exits with an error message if the given number
 * of hearts is not valid.
 * @param hearts Link's new number of hearts (in heart quarters)
 */
void Equipment::set_hearts(int hearts) {

  if (hearts < 0 || hearts > 20 * 4) {
    cerr << "Illegal number of hearts: " << hearts << endl;
    exit(1);
  }

  savegame->set_reserved_integer(SAVEGAME_CURRENT_HEARTS, hearts);
}

/**
 * Gives some hearts to Link.
 * If the maximum number of hearts is achieved, no more hearts are added.
 * @param hearts_to_add number of hearts to add (in heart quarters)
 */
void Equipment::add_hearts(int hearts_to_add) {

  int max_hearts = get_max_hearts() * 4;
  int total = get_hearts() + hearts_to_add;

  set_hearts(MIN(total, max_hearts));
}

/**
 * Removes some hearts to Link.
 * If the number of hearts achieves zero, the game over sequence is started.
 * @param hearts_to_remove number of hearts to remove
 */
void Equipment::remove_hearts(int hearts_to_remove) {

  int total = get_hearts() - hearts_to_remove;
  
  set_hearts(MAX(total, 0));

  // TODO gameover
}

/**
 * Restores all the hearts.
 */
void Equipment::restore_all_hearts(void) {
  set_hearts(get_max_hearts());
}

/**
 * Returns the current number of pieces of heart of Link.
 * @returns Link's current number of pieces of heart, between 0 and 3
 */
int Equipment::get_nb_pieces_of_heart(void) {
  return savegame->get_reserved_integer(SAVEGAME_PIECES_OF_HEART);
}

/**
 * Returns whether Link has the specified piece of heart.
 * @param piece_of_heart_id index of the piece of heart, between 0
 * and the number of pieces of hearts in the game (which cannot
 * be greater than 80)
 */
bool Equipment::has_piece_of_heart(int piece_of_heart_id) {

  int index = SAVEGAME_FIRST_PIECE_OF_HEART + piece_of_heart_id;
  
  if (index < SAVEGAME_FIRST_PIECE_OF_HEART || index > SAVEGAME_LAST_PIECE_OF_HEART) {
    cerr << "Illegal piece of heart index: " << piece_of_heart_id << endl;
    exit(1);
  }

  return savegame->get_reserved_integer(index) != 0;
}

/**
 * Adds a piece of heart to Link.
 * @param piece_of_heart_id index of the piece of heart, between 1
 * and the number of pieces of hearts in the game - 1
 */
void Equipment::add_piece_of_heart(int piece_of_heart_id) {

  // note: the piece_of_heart sound is played by the collectable when it is picked

  if (has_piece_of_heart(piece_of_heart_id)) {
    cerr << "The player already has piece of heart #" << piece_of_heart_id << endl;
    exit(1);
  }

  savegame->set_reserved_integer(SAVEGAME_FIRST_PIECE_OF_HEART + piece_of_heart_id, 1);

  // check whether Link has a new heart

  int nb_pieces_of_heart = get_nb_pieces_of_heart() + 1;
  if (nb_pieces_of_heart < 4) {
    // no new heart
    savegame->set_reserved_integer(SAVEGAME_PIECES_OF_HEART, nb_pieces_of_heart);
  }
  else {
    // new heart container
    savegame->set_reserved_integer(SAVEGAME_PIECES_OF_HEART, 0);
    add_heart_container();
  }

  restore_all_hearts();
}
