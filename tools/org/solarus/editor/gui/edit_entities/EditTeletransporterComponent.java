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
package org.solarus.editor.gui.edit_entities;

import java.awt.event.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a teletransporter.
 */
public class EditTeletransporterComponent extends EditEntityComponent {

    // specific fields of a teletransporter
    private EnumerationChooser<Transition> transitionField;
    private ResourceChooser mapField;
    private EntityChooser destinationPointField;

    private static final String samePointText = "<Same point>";
    private static final String sidePointText = "<Side of the map>";

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditTeletransporterComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// transition
	transitionField = new EnumerationChooser<Transition>(Transition.class);
	addField("Transition", transitionField);

	// destination map
	mapField = new ResourceChooser(ResourceType.MAP, true);
	addField("Destination map", mapField);

	// destination point
	destinationPointField = new EntityChooser(null, EntityType.DESTINATION_POINT,
		new String[] {"", samePointText, sidePointText});
	addField("Destination point", destinationPointField);

	// load the entrance list for the selected map
	mapField.addActionListener(new ActionListenerChangeDestinationMap());
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Teletransporter teletransporter = (Teletransporter) entity;

	transitionField.setValue(Transition.get(teletransporter.getIntegerProperty("transition")));
	mapField.setSelectedId(teletransporter.getProperty("destinationMapId"));
	
	String destinationPointName = teletransporter.getProperty("destinationPointName");
	if (destinationPointName.equals("_same")) {
	    destinationPointName = samePointText;
	}
	else if (destinationPointName.equals("_side")) {
	    destinationPointName = sidePointText;
	}
	destinationPointField.setSelectedName(destinationPointName);
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	String destinationMapId = mapField.getSelectedId();
	String destinationPointName = destinationPointField.getSelectedName();

	if (destinationPointName.equals(samePointText)) {
	    destinationPointName = "_same";
	}
	else if (destinationPointName.equals(sidePointText)) {
	    destinationPointName = "_side";
	}

	return new ActionEditEntitySpecific(entity, 
		Integer.toString(transitionField.getValue().getId()), 
		destinationMapId, destinationPointName);
    }

    /**
     * Action performed when the destination map field is changed.
     * The entrance field is reloaded to show the entrances of this map.
     */
    private class ActionListenerChangeDestinationMap implements ActionListener {

	public void actionPerformed(ActionEvent ev) {
	    
	    try {
		String mapId = mapField.getSelectedId();
		
		if (mapId.length() != 0) { // a map has just been selected
		    destinationPointField.setMap(new Map(mapId));
		}
		else {
		    destinationPointField.setMap(null);
		}
	    }
	    catch (ZSDXException ex) {
		GuiTools.warningDialog("The destination map is invalid: " + ex.getMessage());
	    }
	}
    }
}
