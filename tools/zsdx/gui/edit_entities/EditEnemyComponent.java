package zsdx.gui.edit_entities;

import java.awt.event.*;

import zsdx.*;
import zsdx.entities.*;
import zsdx.entities.Enemy.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit an enemy.
 */
public class EditEnemyComponent extends EditEntityComponent {

    // specific fields
    private EnumerationChooser<Subtype> subtypeField;
    private EnumerationChooser<Enemy.Rank> rankField;
    private NumberChooser savegameVariableField;
    private PickableItemSubtypeChooser pickableItemSubtypeField;
    private NumberChooser pickableItemSavegameVariableField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditEnemyComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// enemy subtype
	subtypeField = new EnumerationChooser<Subtype>(Subtype.class);
	addField("Enemy type", subtypeField);

	// rank
	rankField = new EnumerationChooser<Rank>(Rank.class);
	addField("Rank", rankField);

	// savegame variable
	savegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Enemy savegame variable", savegameVariableField);

	// pickable item type
	pickableItemSubtypeField = new PickableItemSubtypeChooser(true);
	addField("Pickable item", pickableItemSubtypeField);

	// pickable item savegame variable
	pickableItemSavegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Pickable item savegame variable", pickableItemSavegameVariableField);

	// enable or disable the 'savegame variable' field depending on the pickable item type
	pickableItemSubtypeField.addActionListener(new ActionListenerEnableSavegameVariable());
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Enemy enemy = (Enemy) entity;

	subtypeField.setValue((Subtype) enemy.getSubtype());
	rankField.setValue(enemy.getRank());
	savegameVariableField.setNumber(enemy.getSavegameVariable());
	pickableItemSubtypeField.setValue(enemy.getPickableItemSubtype());
	pickableItemSavegameVariableField.setNumber(enemy.getPickableItemSavegameVariable());
	new ActionListenerEnableSavegameVariable().actionPerformed(null);
    }

    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException if the fields are incorrectly filled
     */
    protected ActionEditEntity getAction() throws ZSDXException {

	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the specific properties
	Enemy enemy = (Enemy) entity;

	Subtype subtype = subtypeField.getValue();
	Rank rank = rankField.getValue();
	int savegameVariable = savegameVariableField.getNumber();
	PickableItem.Subtype pickableItemSubtype = pickableItemSubtypeField.getValue();
	int pickableItemSavegameVariable = pickableItemSavegameVariableField.getNumber();

	action.setSpecificAction(new ActionEditEnemy(map, enemy,
		subtype, rank, savegameVariable, pickableItemSubtype, pickableItemSavegameVariable));

	return action;
    }

    /**
     * A listener associated to the 'pickable item type' field,
     * to enable or disable the 'savegame variable' field depending on the type.
     */
    private class ActionListenerEnableSavegameVariable implements ActionListener {

	public void actionPerformed(ActionEvent ev) {
	    PickableItem.Subtype pickableItemSubtype = pickableItemSubtypeField.getValue();
	    pickableItemSavegameVariableField.setEnabled(pickableItemSubtype.isSaved());
	}
    }
}
