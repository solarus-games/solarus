/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

import org.solarus.editor.*;

/**
 * A combo box component to select an item from the resource database.
 */
public class ItemChooser extends ResourceChooser {

    /**
     * Creates an item chooser, specifying whether the special items
     * "None" and "Random" are included.
     * @param includeNone true to include an option "None"
     * @param includeRandom true to include an option "Random"
     */
    public ItemChooser(boolean includeNone, boolean includeRandom) {

	super(ResourceType.ITEM, false);

	int size = (includeNone && includeRandom) ? 2 : 1;
	KeyValue[] additionalOptions = new KeyValue[size];
	int i = 0;

	if (includeNone) {
	    additionalOptions[i] = new KeyValue(Item.noneId, Item.noneName);
	    i++;
	}
	
	if (includeRandom) {
	    additionalOptions[i] = new KeyValue(Item.randomId, Item.randomName);
	    i++;
	}
	
	if (i == 0) {
	    // always add an empty row to show to the user that he has to select another item
	    additionalOptions[0] = new KeyValue("", "");
	}
	
	setAdditionalOptions(additionalOptions);
    }
}
