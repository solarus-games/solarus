package zsdx.gui.edit_entities;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit a chest.
 */
public class EditChestComponent extends EditEntityComponent {

    // specific fields of a chest
    private RadioField sizeField;
    private TreasureChooser contentField;
    private NumberChooser amountField;
    private JCheckBox saveField;
    private NumberChooser savegameIndexField;
    
    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditChestComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// big or small chest
	sizeField = new RadioField("Small", "Big");
	addField("Chest type", sizeField);

	// treasure content
	contentField = new TreasureChooser();
	addField("Treasure", contentField);

	// treasure amount
	amountField = new NumberChooser(1, 1, 999);
	addField("Amount", amountField);

	// treasure saving option
	saveField = new JCheckBox("Save the chest state");
	saveField.setSelected(true);
	addField("Savegame", saveField);

	// treasure savegame index
	savegameIndexField = new NumberChooser(0, 0, 32767);
	addField("Savegame variable", savegameIndexField);

	// enable or disable the amount field depending on the treasure content
	contentField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		
		boolean treasurePresent = (contentField.getContent() != TreasureContent.NOTHING);
		amountField.setEnabled(treasurePresent && contentField.getContent().hasAmount());
		savegameIndexField.setEnabled(treasurePresent);
	    }
	});

	// enable or disable the savegame index field depending on the check box
	saveField.addChangeListener(new ChangeListener() {
	    public void stateChanged(ChangeEvent ev) {
		savegameIndexField.setEnabled(saveField.isSelected());
	    }
	});
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Chest chest = (Chest) entity;

	sizeField.setSelectedIndex(chest.isBigChest() ? 1 : 0);
	contentField.setContent(chest.getContent());
	amountField.setNumber(chest.getAmount());
	saveField.setSelected(chest.getSavegameIndex() != -1);
	savegameIndexField.setNumber(chest.getSavegameIndex());
	savegameIndexField.setEnabled(chest.getSavegameIndex() != -1);
    }
    
    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException if the destination map or the entrance name are left blank
     */
    protected ActionEditEntity getAction() throws ZSDXException {
	
	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to a chest
	Chest chest = (Chest) entity;

	TreasureContent content = contentField.getContent();
	boolean bigChest = (sizeField.getSelectedIndex() != 0);
	int amount = amountField.getNumber();
	int savegameIndex = saveField.isSelected() ? savegameIndexField.getNumber() : -1;

	action.setSpecificAction(new ActionEditChest(map, chest, bigChest, content, amount, savegameIndex));

	return action;
    }
}
