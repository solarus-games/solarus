package org.solarus.editor.gui;

import java.awt.*;

import javax.swing.*;

import org.solarus.editor.*;

/**
 * A component to choose the properties of a treasure, that is,
 * the item to give, its variant and the savegame variable. 
 */
public class TreasureChooser extends JPanel {

    private Treasure treasure;			/**< the treasure to edit*/
    
    // subcomponents
    private ItemChooser itemNameField;
    private NumberChooser variantField;
    private JCheckBox saveField;
    private NumberChooser savegameVariableField;
    
    private GridBagConstraints gridBagConstraints;

    /**
     * Creates an item chooser, specifying whether the special treasures
     * "None" and "Random" are included.
     * @param includeNone true to include an option "None"
     * @param includeRandom true to include an option "Random"
     */
    public TreasureChooser(boolean includeNone, boolean includeRandom) {
	
	super();
	
	// create a default treasure
	treasure = new Treasure("_none", 1, -1);

	// create the subcomponents
	itemNameField = new ItemChooser(includeNone, includeRandom);
	variantField = new NumberChooser(1, 1, Integer.MAX_VALUE);
	saveField = new JCheckBox("Save the treasure state");
	savegameVariableField = new NumberChooser(0, 0, 32767);

	// place the subcomponents
	gridBagConstraints = new GridBagConstraints();
	gridBagConstraints.insets = new Insets(5, 5, 5, 5); // margins
	gridBagConstraints.anchor = GridBagConstraints.LINE_START;
	gridBagConstraints.gridx = 0;
	gridBagConstraints.gridy = 0;
	
	add(itemNameField, gridBagConstraints);
	gridBagConstraints.gridy++;
	
	JPanel variantPanel = new JPanel();
	variantPanel.add(new JLabel("Variant of this item:"));
	variantPanel.add(variantField);
	add(variantField, gridBagConstraints);
	gridBagConstraints.gridy++;

	add(saveField, gridBagConstraints);
	gridBagConstraints.gridy++;

	JPanel savegameVariablePanel = new JPanel();
	savegameVariablePanel.add(new JLabel("Savegame variable:"));
	savegameVariablePanel.add(savegameVariableField);
	add(savegameVariablePanel, gridBagConstraints);
	gridBagConstraints.gridy++;

    }
    
    /**
     * Returns the treasure represented in this component.
     * @return the treasure
     */
    public Treasure getTreasure() {
	return treasure;
    }

    /**
     * Sets all properties of the treasure represented in this component.
     * @param ItemName name identifying the treasure to give (possibly "_none" or "_random")
     * @param variant variant of this item
     * @param savegameVariable savegame variable that stores the treasure's state,
     * or -1 to make the treasure unsaved
     */
    public void setTreasure(String ItemName, int variant, int savegameVariable) {

	treasure.setItemName(ItemName);
	treasure.setVariant(variant);
	treasure.setSavegameVariable(savegameVariable);
    }
}
