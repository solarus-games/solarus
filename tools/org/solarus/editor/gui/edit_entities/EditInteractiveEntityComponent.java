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
import javax.swing.*;
import javax.swing.event.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;
import org.solarus.editor.entities.InteractiveEntity.Subtype;

/**
 * A component to edit an interactive entity.
 */
public class EditInteractiveEntityComponent extends EditEntityComponent {

    // specific fields of this entity
    private JCheckBox withSpriteField;
    private ResourceChooser spriteField;
    private RadioField behaviorField;
    private JTextField messageField;
    private ItemChooser itemField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditInteractiveEntityComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// has a sprite?
	withSpriteField = new JCheckBox("Display a sprite");
	addField("Visibility", withSpriteField);

	// sprite name
	spriteField = new ResourceChooser(ResourceType.SPRITE, true);
	addField("Sprite name", spriteField);

	// behavior
	behaviorField = new RadioField("Show a message", "Call the map script", "Call an item script");
	addField("Action", behaviorField);

	// message
	messageField = new JTextField(15);
	addField("Message to show", messageField);

	// item
	itemField = new ItemChooser(false, false);
	addField("Item script to call", itemField);

	// listeners
	withSpriteField.addChangeListener(new ChangeListener() {
	    public void stateChanged(ChangeEvent ev) {
		spriteField.setEnabled(withSpriteField.isSelected());
	    }
	});

	behaviorField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		messageField.setEnabled(behaviorField.getSelectedIndex() == 0);
		itemField.setEnabled(behaviorField.getSelectedIndex() == 2);
	    }
	});

	subtypeField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {

		Subtype subtype = (Subtype) subtypeField.getValue();
		withSpriteField.setEnabled(subtype != Subtype.NON_PLAYING_CHARACTER);
	    }
	});
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	InteractiveEntity interactiveEntity = (InteractiveEntity) entity;
	String sprite = interactiveEntity.getProperty("sprite");
	String behavior = interactiveEntity.getProperty("behavior");
	EntitySubtype subtype = interactiveEntity.getSubtype();

	boolean hasSprite = (!sprite.equals("_none"));

	withSpriteField.setSelected(hasSprite);
	spriteField.setSelectedId(hasSprite ? sprite : "");

	spriteField.setEnabled(hasSprite);

	if (behavior.equals("map")) {
	  behaviorField.setSelectedIndex(1);
	  messageField.setEnabled(false);
	  itemField.setEnabled(false);
	}
	else if (behavior.substring(0, 5).equals("item#")) {
	  behaviorField.setSelectedIndex(2);
	  messageField.setEnabled(false);
	  itemField.setEnabled(true);
	  itemField.setSelectedId(behavior.substring(5));
	}
	else if (behavior.substring(0, 7).equals("dialog#")) {
	  behaviorField.setSelectedIndex(0);
	  messageField.setEnabled(true);
	  itemField.setEnabled(false);
	  messageField.setText(behavior.substring(7));
	}
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	String sprite = spriteField.getSelectedId();
	if (!withSpriteField.isSelected()) {
	    sprite = "_none";
	}

	String behavior = "map";
	if (messageField.isEnabled()) {
	  behavior = "dialog#" + messageField.getText();
	}
	else if (itemField.isEnabled()) {
	  behavior = "item#" + itemField.getSelectedId();
	}

	return new ActionEditEntitySpecific(entity, sprite, behavior);
    }
}
