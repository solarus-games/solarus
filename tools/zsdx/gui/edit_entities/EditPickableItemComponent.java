package zsdx.gui.edit_entities;

import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.entities.PickableItem.Subtype;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;

/**
 * A component to edit a pickable item.
 */
public class EditPickableItemComponent extends EditEntityComponent {
    
    // specific fields of a pickable item
    private NumberChooser savegameVariableField; // enabled only for certain types of pickable items

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditPickableItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// savegame index
	savegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Savegame variable", savegameVariableField);

	// enable or disable the 'savegame variable' field depending on the pickable item type
	subtypeField.addActionListener(new ActionListenerEnableSavegameVariable());

	// remove 'Random' and 'None'
	subtypeField.removeItemAt(0);
	subtypeField.removeItemAt(0);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	PickableItem pickableItem = (PickableItem) entity;

	savegameVariableField.setNumber(pickableItem.getIntegerProperty("savegameVariable"));
	new ActionListenerEnableSavegameVariable().actionPerformed(null);
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {
	return new ActionEditEntitySpecific(entity, savegameVariableField.getNumber());
    }

    /**
     * A listener associated to the 'pickable item type' field,
     * to enable or disable the 'savegame index' field depending on the type.
     */
    private class ActionListenerEnableSavegameVariable implements ActionListener {

	public void actionPerformed(ActionEvent ev) {
	    Subtype subtype = (Subtype) subtypeField.getValue();
	    savegameVariableField.setEnabled(subtype.isSaved());
	}
    }
}
