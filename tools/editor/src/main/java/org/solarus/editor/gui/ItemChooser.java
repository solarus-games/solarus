/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
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
     * Creates an item chooser, specifying whether the special item
     * "None" is included.
     * @param includeNone true to include an option "None"
     */
    public ItemChooser(boolean includeNone) {

        super(ResourceType.ITEM, false);

        KeyValue[] additionalOptions = new KeyValue[1];

        if (includeNone) {
            additionalOptions[0] = new KeyValue("", "(None)");
        }
	else {
            // Add an empty row anyway just to show that the user he has to select another item
            additionalOptions[0] = new KeyValue("", "");
        }

        setAdditionalOptions(additionalOptions);
    }
}

