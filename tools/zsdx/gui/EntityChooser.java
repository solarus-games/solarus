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
import java.util.List;
import zsdx.*;
import zsdx.entities.*;

/**
 * A combo box to select an entity of a certain type.
 * This component displays the name of the entities of this type in the map.
 */
public class EntityChooser extends JComboBox {

    /**
     * The map.
     */
    private Map map;

    /**
     * The kind of entity displayed in the combo box (doors, ennemies...).
     */
    private EntityType entityType;

    /**
     * A list of special options to add to the combo box.
     */
    private String[] additionalOptions;

    /**
     * Creates a new entity chooser.
     * @param map the map (can be null and specified later)
     * @param entityType the type of entity to show in the combo box
     * @param showEmptyOption true to add an empty option to the combo box
     */
    public EntityChooser(Map map, EntityType entityType, boolean showEmptyOption) {
	super();

	this.map = map;
	this.entityType = entityType;

	if (showEmptyOption) {
	    this.additionalOptions = new String[] {""};
	}
	else {
	    this.additionalOptions = new String[] {};    
	}

	buildList();
    }

    /**
     * Creates a new entity chooser.
     * @param map the map (can be null and specified later)
     * @param entityType the type of entity to show in the combo box
     * @param additionalOptions a list of special options to add to the combo box
     */
    public EntityChooser(Map map, EntityType entityType, String[] additionalOptions) {
	super();

	this.map = map;
	this.entityType = entityType;
	this.additionalOptions = additionalOptions; 

	buildList();
    }

    /**
     * Builds the combo box list.
     */
    private void buildList() {

	super.removeAllItems();

	for (int i = 0; i < additionalOptions.length; i++) {
	    addItem(additionalOptions[i]);
	}

	if (map != null) {
	    List<MapEntity> entities = map.getEntitiesOfType(entityType);
	    for (MapEntity entity: entities) {
		addItem(entity.getName());
	    }

	    // if there is only one entity, we make it selected
	    if (entities.size() == 1) {
		setSelectedIndex(additionalOptions.length);
	    }
	    setEnabled(true);
	}
	else {
	    setEnabled(false);
	}
    }

    /**
     * Returns the name of the selected entity.
     * @return the name of the selected entity, or an empty string if no entity is selected
     */
    public String getSelectedName() {
	return (String) getSelectedItem();
    }

    /**
     * Selects an entity in a combo box.
     * to select no entity
     */
    public void setSelectedName(String name) {
	setSelectedItem(name);
    }

    /**
     * Changes the map.
     * The list is rebuilt to show the entities on this map.
     * @param map the map
     */
    public void setMap(Map map) {
	this.map = map;
	buildList();
    }
}
