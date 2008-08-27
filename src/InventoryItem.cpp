#include "InventoryItem.h"
#include "Savegame.h"

/**
 * Properties of each item of the inventory.
 */
const InventoryItem InventoryItem::items[28] = {

  {true, 0}, // feather
  {true, SAVEGAME_CURRENT_BOMBS}, // bombs
  {true, SAVEGAME_CURRENT_ARROWS}, // bow and arrows
  {true, 0}, // boomerang
  {true, 0}, // lamp
  {true, 0}, // hook shot
  {true, 0}, // bottle 1

  {true, 0}, // pegasus shoes
  {true, 0}, // mystic mirror
  {true, 0}, // cane of somaria
  {true, 0}, // magic cape
  {false, 0}, // iron glove + golden glove
  {false, SAVEGAME_NB_FIRE_STONES}, // fire stones
  {true, 0}, // bottle 1

  {true, SAVEGAME_CURRENT_APPLES},  // apples
  {true, SAVEGAME_CURRENT_PAINS_AU_CHOCOLAT}, // pains au chocolats
  {true, SAVEGAME_CURRENT_CROISSANTS}, // croissants
  {false, 0}, // apple pie + golden bars + edelweiss + bone key 
  {false, 0}, // flippers
  {false, 0}, // red key
  {false, 0}, // TODO item 20

  {false, 0}, // clay key
  {false, 0}, // rock key
  {false, 0}, // iron key
  {false, 0}, // stone key
  {false, 0}, // wooden key
  {false, 0}, // ice key
  {false, 0}, // TODO item 27
};
