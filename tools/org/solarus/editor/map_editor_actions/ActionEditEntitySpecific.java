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
package org.solarus.editor.map_editor_actions;

import java.util.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * Editing the specific properties of an entity. The subclasses of ActionEditEntity can define
 * the specific properties of the subclasses of MapEntity. 
 */
public class ActionEditEntitySpecific extends MapEditorAction {

    /**
     * The entity modified by this action.
     */
    private MapEntity entity;

    /**
     * The properties and their values.
     */
    private LinkedHashMap<String, String> propertiesBefore;
    private LinkedHashMap<String, String> propertiesAfter;

    /**
     * Constructor.
     */
    public ActionEditEntitySpecific(MapEntity entity, String ... properties) {
	this.entity = entity;
	propertiesBefore = new LinkedHashMap<String, String>(entity.getProperties());
	propertiesAfter = new LinkedHashMap<String, String>();

	int i = 0;
	for (String name: propertiesBefore.keySet()) {
	    propertiesAfter.put(name, properties[i++]);
	}
    }

    /**
     * Constructor.
     */
    public ActionEditEntitySpecific(MapEntity entity, int ... properties) {
	this.entity = entity;

	if (properties.length != entity.getProperties().size()) {
	    throw new IllegalArgumentException("Wrong number of properties specified (" +
		    properties.length + ": this entity has " + entity.getProperties().size() + "properties");
	}

	propertiesBefore = new LinkedHashMap<String, String>(entity.getProperties());
	propertiesAfter = new LinkedHashMap<String, String>();

	int i = 0;
	for (String name: propertiesBefore.keySet()) {
	    propertiesAfter.put(name, Integer.toString(properties[i++]));
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	entity.setProperties(propertiesAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	entity.setProperties(propertiesBefore);
    }
}
