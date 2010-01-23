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

/**
 * A couple (key, value).
 * It can be used as a combo box item:
 * The key is the id of the item, and the value is the name displayed
 * in the combo box.
 */
public class KeyValue {

    private String key;
    private String value;

    /**
     * Constructor.
     * @param key the key
     * @param value the value
     */
    public KeyValue(String key, String value) {

	if (key == null) {
	    throw new NullPointerException("The key is null");
	}

	this.key = key;
	this.value = value;
    }

    /**
     * Constructor.
     * @param key the key
     * @param value the value
     */
    public KeyValue(int key, String value) {
	this(Integer.toString(key), value);
    }
    
    /**
     * Returns the key.
     * @return the key
     */
    public String getKey() {
	return key;
    }

    /**
     * Returns the value.
     * @return the value
     */
    public String getValue() {
	return value;
    }
    
    /**
     * Returns a string representation of the object.
     * @return the value
     */
    public String toString() {
	return value;
    }

    /**
     * Compares two key-value objects.
     * @param o another key-value object
     * @return true if the two objects have the same key (the value is ignored)
     */
    public boolean equals(Object o) {
	KeyValue keyValue = (KeyValue) o;
	return keyValue != null && key.equals(keyValue.getKey());
    }
}
