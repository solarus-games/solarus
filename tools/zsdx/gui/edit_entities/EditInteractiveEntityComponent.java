package zsdx.gui.edit_entities;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;
import zsdx.entities.InteractiveEntity.Subtype;

/**
 * A component to edit an interactive entity.
 */
public class EditInteractiveEntityComponent extends EditEntityComponent {

    // specific fields of this entity
    private EnumerationChooser<Subtype> subtypeField;
    private JCheckBox withSpriteField;
    private JTextField spriteField;
    private RadioField withMessageField;
    private JTextField messageField;

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

	// subtype of interactive entity
	subtypeField = new EnumerationChooser<Subtype>(Subtype.class);
	addField("Interaction type", subtypeField);

	// has a sprite?
	withSpriteField = new JCheckBox("Display a sprite");
	addField("Visibility", withSpriteField);

	// sprite name
	spriteField = new JTextField(15);
	addField("Sprite name", spriteField);

	// show a message?
	withMessageField = new RadioField("Show a message", "Call the script");
	addField("Action", withMessageField);

	// message
	messageField = new JTextField(15);
	addField("Message id", messageField);

	// listeners
	withSpriteField.addChangeListener(new ChangeListener() {
	    public void stateChanged(ChangeEvent ev) {
		spriteField.setEnabled(withSpriteField.isSelected());
	    }
	});

	withMessageField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		messageField.setEnabled(withMessageField.getSelectedIndex() == 0);
	    }
	});

	subtypeField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {

		Subtype subtype = subtypeField.getValue();

		withSpriteField.setEnabled(true);
		spriteField.setEnabled(true);
		withMessageField.setEnabled(true);
		messageField.setEnabled(true);

		switch (subtype) {

		case NON_PLAYING_CHARACTER:
		    withSpriteField.setEnabled(false);
		    withSpriteField.setSelected(true);
		    break;

		case SIGN:
		    withSpriteField.setEnabled(false);
		    withSpriteField.setSelected(true);
		    spriteField.setText("entities/sign");
		    spriteField.setEnabled(false);
		    break;

		case WATER_FOR_BOTTLE:
		    withSpriteField.setSelected(false);
		    withSpriteField.setEnabled(false);
		    spriteField.setText("");
		    spriteField.setEnabled(false);
		    withMessageField.setSelectedIndex(1);
		    withMessageField.setEnabled(false);
		    messageField.setEnabled(false);
		    break;
		}
	    }
	});
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	InteractiveEntity interactiveEntity = (InteractiveEntity) entity;
	String sprite = interactiveEntity.getSprite();
	String message = interactiveEntity.getMessageId();

	subtypeField.setValue(interactiveEntity.getSubtype());
	withSpriteField.setSelected(sprite != null);
	spriteField.setText((sprite != null) ? sprite : "");
	spriteField.setEnabled(sprite != null);
	withMessageField.setSelectedIndex((message != null) ? 0 : 1);
	messageField.setText((message != null) ? message : "");
	messageField.setEnabled(message != null);
    }
    
    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException
     */
    protected ActionEditEntity getAction() throws ZSDXException {
	
	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to this kind of entity
	InteractiveEntity interactiveEntity = (InteractiveEntity) entity;

	Subtype subtype = subtypeField.getValue();
	String sprite = spriteField.getText();
	if (!withSpriteField.isSelected()) {
	    sprite = null;
	}

	String message = messageField.getText();
	if (withMessageField.getSelectedIndex() != 0) {
	    message = null;
	}

	action.setSpecificAction(new ActionEditInteractiveEntity(map, interactiveEntity,
		subtype, sprite, message));

	return action;
    }
}
