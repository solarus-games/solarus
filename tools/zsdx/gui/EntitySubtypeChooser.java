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
import zsdx.entities.*;

/**
 * A combo box to select a subtype of an entity type.
 */
public class EntitySubtypeChooser extends JComboBox {

    /**
     * The enumeration displayed in this combo box.
     */
    private Class<? extends EntitySubtype> enumeration;

    /**
     * Creates a new entity enumeration chooser.
     * @param entityType the type of entity
     */
    public EntitySubtypeChooser(EntityType entityType) {
	super();

	this.enumeration = entityType.getSubtypeEnum();

	try {
	    String[] humanNames = (String[]) enumeration.getField("humanNames").get(null);
	    EntitySubtype[] values = (EntitySubtype[]) enumeration.getMethod("values").invoke(null, (Object[]) null);
	    for (int i = 0; i < values.length; i++) {
		addItem(new KeyValue(values[i].getId(), humanNames[i]));
	    }
	}
	catch (NoSuchFieldException ex) {
	    System.err.println("The field 'humanNames' is missing in enumeration " + enumeration.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("The method 'values' is missing in enumeration " + enumeration.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot access a member in enumeration " + enumeration.getName() + ": ex.getMessage()");
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
    public EntitySubtype getValue() {

	EntitySubtype value = null;
	try {
	    KeyValue item = (KeyValue) getSelectedItem();
	    int id = Integer.parseInt(item.getKey());
	    value = (EntitySubtype) enumeration.getMethod("get", int.class).invoke(null, id);
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("The method 'get' is missing in enumeration " + enumeration.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot access method 'get' in enumeration " + enumeration.getName() + ": ex.getMessage()");
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (InvocationTargetException ex) {
	    ex.getCause().printStackTrace();
	    System.exit(1);
	}
	return value;
    }

    /**
     * Sets the value selected.
     * @param value the value to make selected
     */
    public void setValue(EntitySubtype value) {
	KeyValue item = new KeyValue(value.getId(), null);
	setSelectedItem(item);
    }
}
