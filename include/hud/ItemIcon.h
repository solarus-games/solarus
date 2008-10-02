#ifndef ZSDX_ITEM_ICON_H
#define ZSDX_ITEM_ICON_H

#include "Common.h"
#include "hud/HudElement.h"
#include "InventoryItem.h"

/**
 * This class displays the icon representing
 * the item currently assigned to X or V.
 */
class ItemIcon: public HudElement {

 private:

  Savegame *savegame;          /**< the savegame, used to know what item is currently assigned to this slot */

  int slot;                    /**< the slot of this icon (0 for the X icon, 1 for the Y icon) */

  SDL_Surface *background_img; /**< background image of the icon (i.e. the empty icon) */
  SDL_Surface *items_img;      /**< image containing all item with all variants */

  InventoryItem::ItemId item_displayed;  /**< the item currently displayed on the icon */
  int item_variant_displayed;            /**< the item variant currently displayed */
  Counter *counter;                      /**< the counter associated to this item (if any) */
  int counter_value_displayed;           /**< the counter value currently displayed 
					  * (-1 if there is no counter) */

  bool is_enabled;                       /**< true if the icon is enabled */

  static const string background_file_names[]; /**< file name of the background image for each slot */

 public:

  ItemIcon(int slot, Savegame *savegame, int x, int y);
  ~ItemIcon(void);

  void update(void);
  void rebuild(void);
};

#endif
