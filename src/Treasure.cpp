#include "Treasure.h"
#include "ResourceManager.h"
#include "Sound.h"

/**
 * Creates a new treasure.
 * @param content content of the treasure
 * @param amount for bombs, arrows, apples, pains au chocolat, croissants, hearts, small magic bottles,
 * big magic bottles, green rupees, blue rupees and red rupees: indicates the amount; if the amount is
 * greater than 1, a counter will be shown.
 * @param savegame_index index of the savegame variable indicating that Link has found this treasure
 * or -1 if this treasure is not saved
 */
Treasure::Treasure(TreasureContent content, int amount, int savegame_index):
  content(content), amount(amount), savegame_index(savegame_index) {

}

/**
 * Destructor.
 */
Treasure::~Treasure(void) {

}

/**
 * Give the treasure to the player: plays the treasure sound, makes Link
 * brandish the item and adds the item to Link's equipment.
 */
void Treasure::give_to_player(void) {

  play_treasure_sound();
  start_brandish_item();
  add_item_to_equipment();
  show_message();
  stop_brandish_item();
}

/**
 * Plays a treasure sound.
 */
void Treasure::play_treasure_sound(void) {
  
  // the treasure sound is the same for all items except the piece of heart and the heart container
  string sound_name;

  if (content != PIECE_OF_HEART && content != HEART_CONTAINER) {
    sound_name = "treasure";
  }
  else {
    sound_name = "piece_of_heart";
  }
  ResourceManager::get_sound(sound_name)->play();
}

/**
 * Makes Link brandish the item.
 */
void Treasure::start_brandish_item(void) {
  
}

/**
 * Makes Link stop brandish the item.
 */
void Treasure::stop_brandish_item(void) {
  
}

/**
 * Shows the message telling that the player
 * has found the item.
 */
void Treasure::show_message(void) {
  // particular cases:
  // pieces of heart, items with counters
}

/**
 * Adds the item to Link's eqipment.
 */
void Treasure::add_item_to_equipment(void) {
  
}
