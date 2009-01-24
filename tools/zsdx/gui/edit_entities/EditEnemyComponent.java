package zsdx.gui.edit_entities;

import java.awt.event.*;

import javax.swing.JComboBox;

import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit an enemy.
 */
public class EditEnemyComponent extends EditEntityComponent {

    // specific fields
    private EnemyChooser subtypeField;
    private EnemyRankChooser rankField;
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
	subtypeField = new EnemyChooser();
	addField("Enemy type", subtypeField);

	// rank
	rankField = new EnemyRankChooser();
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

	subtypeField.setEnemySubtype(enemy.getSubtype());
	rankField.setRank(enemy.getRank());
	savegameVariableField.setNumber(enemy.getSavegameVariable());
	pickableItemSubtypeField.setPickableItemSubtype(enemy.getPickableItemSubtype());
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

	Enemy.Subtype subtype = subtypeField.getEnemySubtype();
	Enemy.Rank rank = rankField.getRank();
	int savegameVariable = savegameVariableField.getNumber();
	int pickableItemSubtype = pickableItemSubtypeField.getPickableItemType();
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

	    int subtype = pickableItemSubtypeField.getPickableItemType();

	    if (subtype <= PickableItem.PICKABLE_ITEM_ARROW_10) {
		pickableItemSavegameVariableField.setEnabled(false);
	    }
	    else {
		pickableItemSavegameVariableField.setEnabled(true);
	    }
	}
    }

    /**
     * A combo box to select the rank of an enemy.
     */
    public class EnemyRankChooser extends JComboBox {

	private final String[] names = {
	    "Normal", "Miniboss", "Boss"
	};

	/**
	 * Creates a new enemy rank chooser.
	 */
	public EnemyRankChooser() {
	    super();

	    for (int i = 0; i < names.length; i++) {
		addItem(new KeyValue(i, names[i]));
	    }
	}

	/**
	 * Returns the enemy rank currently selected.
	 * @return the enemy rank currently selected
	 */
	public Enemy.Rank getRank() {

	    KeyValue item = (KeyValue) getSelectedItem();
	    int index = Integer.parseInt(item.getKey());
	    return Enemy.Rank.values()[index];
	}

	/**
	 * Sets the enemy rank selected.
	 * @param rank the rank to make selected
	 */
	public void setRank(Enemy.Rank rank) {

	    KeyValue item = new KeyValue(rank.ordinal(), null);
	    setSelectedItem(item);
	}
    }
}
