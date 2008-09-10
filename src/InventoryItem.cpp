#include "InventoryItem.h"
#include "Savegame.h"

/**
 * Properties of each item of the inventory.
 */
InventoryItem InventoryItem::items[28] = {

  InventoryItem(true, 0), // feather
  InventoryItem(true, Savegame::CURRENT_BOMBS), // bombs
  InventoryItem(true, Savegame::CURRENT_ARROWS), // bow and arrows
  InventoryItem(true, 0), // boomerang
  InventoryItem(true, 0), // lamp
  InventoryItem(true, 0), // hook shot
  InventoryItem(true, 0), // bottle 1

  InventoryItem(true, 0), // pegasus shoes
  InventoryItem(true, 0), // mystic mirror
  InventoryItem(true, 0), // cane of somaria
  InventoryItem(true, 0), // magic cape
  InventoryItem(false, 0), // iron glove + golden glove
  InventoryItem(false, Savegame::NB_FIRE_STONES), // fire stones
  InventoryItem(true, 0), // bottle 2

  InventoryItem(true, Savegame::CURRENT_APPLES),  // apples
  InventoryItem(true, Savegame::CURRENT_PAINS_AU_CHOCOLAT), // pains au chocolats
  InventoryItem(true, Savegame::CURRENT_CROISSANTS), // croissants
  InventoryItem(false, 0), // apple pie + golden bars + edelweiss + bone key 
  InventoryItem(false, 0), // flippers
  InventoryItem(false, 0), // red key
  InventoryItem(false, 0), // TODO item 20

  InventoryItem(false, 0), // clay key
  InventoryItem(false, 0), // rock key
  InventoryItem(false, 0), // iron key
  InventoryItem(false, 0), // stone key
  InventoryItem(false, 0), // wooden key
  InventoryItem(false, 0), // ice key
  InventoryItem(false, 0), // TODO item 27
};

/**
 * Constructor.
 * @param attributable true if this item can be assigned to icon X or V
 * @param counter_index index of the savegame variable indicating the
 * counter's value (0 for no counter)
 */
InventoryItem::InventoryItem(bool attributable, int counter_index):
  attributable(attributable), counter_index(counter_index) {

}

/**
 * Destructor.
 */
InventoryItem::~InventoryItem(void) {

}

/**
 * Returns an inventory item specified by its id.
 * @param id id of the inventory item to get
 */
InventoryItem * InventoryItem::get_item(ItemId id) {
  return &items[id];
}

/**
 * Returns whether the current item can be assigned to icon X or V.
 * @return true if the current item is attributable
 */
bool InventoryItem::is_attributable(void) {
  return attributable;
}

/**
 * Returns whether a counter is associated to this item.
 * This is equivalent to get_counter_index() != 0.
 */
bool InventoryItem::has_counter(void) {
  return counter_index != 0;
}

/**
 * If this item has a counter, returns the index of the savegame
 * variable indicating the counter's value. Otherwise, returns 0.
 * @return the index of the savegame variable indicating the counter's value
 */
int InventoryItem::get_counter_index(void) {
  return counter_index;
}
