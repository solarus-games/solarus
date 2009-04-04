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
package zsdx.gui;

import zsdx.entities.PickableItem.Subtype;

/**
 * A combo box to select a type of pickable item.
 */
public class PickableItemSubtypeChooser extends EnumerationChooser<Subtype> {
    
    /**
     * Creates a new pickable item type chooser.
     * @param allowRandomAndNone true to include "Random" and "None" in the combo box
     */
    public PickableItemSubtypeChooser(boolean allowRandomAndNone) {
	super(Subtype.class);

	// special values: random and none
	if (!allowRandomAndNone) {
	    removeItemAt(0);
	    removeItemAt(0);
	}
    }
}
