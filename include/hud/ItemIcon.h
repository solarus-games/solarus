/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_ITEM_ICON_H
#define SOLARUS_ITEM_ICON_H

#include "Common.h"
#include "hud/HudElement.h"

/**
 * @brief Displays the item currently assigned to one of both inventory item keys.
 *
 * Two keys (by default X and V) control the inventory items and
 * the player can choose what inventory items are assigned to those keys.
 * This class displays a slot with an icon of the inventory item currently assigned to it.
 */
class ItemIcon: public HudElement {

  private:

    int slot;							/**< the slot of this icon (0 for the X icon, 1 for the Y icon) */

    Surface *background_img;					/**< background image of the icon (i.e. the empty icon) */
    Sprite *item_sprite;					/**< sprite of item currently displayed */

    std::string item_displayed;					/**< the item currently displayed on the icon (or an empty string) */
    int item_variant_displayed;					/**< the item variant currently displayed */
    Counter *counter;						/**< the counter associated to this item (if any) */
    int counter_value_displayed;				/**< the counter value currently displayed 
								 * (-1 if there is no counter) */
    int counter_maximum_displayed;				/**< the counter maximum value currently considered 
								 * (-1 if there is no counter) */

    static const std::string background_file_names[];		/**< file name of the background image for each slot */

  public:

    ItemIcon(Game &game, int slot, int x, int y);
    ~ItemIcon();

    void update();
    void rebuild();
};

#endif

