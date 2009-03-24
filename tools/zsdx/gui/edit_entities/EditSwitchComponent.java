package zsdx.gui.edit_entities;

import javax.swing.*;
import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.entities.Switch.Subtype;

/**
 * A component to edit a switch.
 */
public class EditSwitchComponent extends EditEntityComponent {

    // specific fields of a switch
    private JCheckBox needsBlockField;
    private JCheckBox disableWhenLeavingField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditSwitchComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// needs block
	needsBlockField = new JCheckBox("Requires a block or a statue to be enabled");
	addField("Activation", needsBlockField);

	// disable when leaving
	disableWhenLeavingField = new JCheckBox("Disable the switch when leaving");
	addField("Stay on switch", disableWhenLeavingField);

	// disable the 'needs block' field when the subtype is invisible
	subtypeField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		needsBlockField.setEnabled(subtypeField.getValue() != Subtype.INVISIBLE);
	    }
	});
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Switch sw = (Switch) entity;

	needsBlockField.setSelected(sw.getBooleanProperty("needsBlock"));
	disableWhenLeavingField.setSelected(sw.getBooleanProperty("disableWhenLeaving"));
	needsBlockField.setEnabled(entity.getSubtype() != Subtype.INVISIBLE);
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	int needsBlock = needsBlockField.isSelected() ? 1 : 0;
	int disableWhenLeaving = disableWhenLeavingField.isSelected() ? 1 : 0;

	return new ActionEditEntitySpecific(entity, needsBlock, disableWhenLeaving);
    }
}
