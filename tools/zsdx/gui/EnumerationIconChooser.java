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

import javax.swing.*;
import java.lang.reflect.*;

/**
 * A combo box to select a value among the specified enumeration.
 * This items of the enumeration are represented with icons.
 * @param e the enumeration type returned and set by getValue() and setValue() respectively
 * (must be a subtype of Enum)
 */
public class EnumerationIconChooser<E extends Enum<E>> extends JComboBox {

    /**
     * The enumeration displayed in this combo box.
     */
    private Class<E> enumeration;

    /**
     * The icons displayed to represent the enumeration.
     */
    private ImageIcon[] icons;

    /**
     * Creates a new entity enumeration icon chooser.
     * @param enumeration the enumeration to display in this combo box
     */
    public EnumerationIconChooser(Class<E> enumeration) {
	super();

	this.enumeration = enumeration;

	try {
	    this.icons = (ImageIcon[]) enumeration.getMethod("getIcons").invoke(null);
	    for (int i = 0; i < icons.length; i++) {
		addItem(icons[i]);
	    }
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("The method 'getIcons()' is missing in enumeration " + enumeration.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot access method 'getIcons()' in enumeration " + enumeration.getName() + ": ex.getMessage()");
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (InvocationTargetException ex) {
	    ex.getCause().printStackTrace();
	    System.exit(1);
	}
    }

    /**
     * Returns the value currently selected.
     * @return the value currently selected
     */
    public E getValue() {
	ImageIcon icon = (ImageIcon) getSelectedItem();
	return E.valueOf(enumeration, icon.getDescription());
    }

    /**
     * Sets the value selected.
     * @param value the value to make selected
     */
    public void setValue(E value) {
	ImageIcon icon = icons[value.ordinal()];
	setSelectedItem(icon);
    }
}
