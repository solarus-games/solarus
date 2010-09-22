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

    private boolean includeNone;
    private boolean includeRandom;

    /**
     * Creates an item chooser, specifying whether the special items
     * "None" and "Random" are included.
     * @param includeNone true to include an option "None"
     * @param includeRandom true to include an option "Random"
     */
    public ItemChooser(boolean includeNone, boolean includeRandom) {
	super(ResourceType.ITEM, false);
	this.includeNone = includeNone;
	this.includeRandom = includeRandom;
    }

    /**
     * Rebuils the list. Two special elements (no item and random item)
     * are added at the beginning of the list.
     */
    protected void buildList() {

	if (includeNone) {
	    addItem(new KeyValue(Item.noneId, Item.noneName));
	}
	if (includeRandom) {
	    addItem(new KeyValue(Item.randomId, Item.randomName));
	}

	super.buildList();
    }
}
