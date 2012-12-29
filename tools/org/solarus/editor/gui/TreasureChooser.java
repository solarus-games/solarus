package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.solarus.editor.*;

/**
 * A component to choose the properties of a treasure, that is,
 * the item to give, its variant and the savegame variable.
 */
public class TreasureChooser extends JPanel {

    private Treasure treasure;                        /**< the treasure to edit */

    // subcomponents
    private ItemChooser itemNameField;
    private NumberChooser variantField;
    private JCheckBox saveField;
    private JTextField savegameVariableField;

    private GridBagConstraints gridBagConstraints;

    /**
     * Creates an item chooser, specifying whether the special treasures
     * "None" and "Random" are included.
     * @param includeNone true to include an option "None"
     */
    public TreasureChooser(boolean includeNone) {

        super(new GridBagLayout());

        // create a default, unspecified treasure
        treasure = new Treasure(null, 1, null);

        // create the subcomponents
        itemNameField = new ItemChooser(includeNone);
        variantField = new NumberChooser(1, 1, Integer.MAX_VALUE);
        saveField = new JCheckBox("Save the treasure state");
        savegameVariableField = new JTextField();

        // place the subcomponents
        setBorder(BorderFactory.createTitledBorder("Treasure"));
        gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.insets = new Insets(5, 5, 5, 5); // margins
        gridBagConstraints.anchor = GridBagConstraints.LINE_START;
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;

        add(itemNameField, gridBagConstraints);
        gridBagConstraints.gridy++;

        JPanel variantPanel = new JPanel();
        variantPanel.add(new JLabel("Variant of this item"));
        variantPanel.add(variantField);
        add(variantPanel, gridBagConstraints);
        gridBagConstraints.gridy++;

        add(saveField, gridBagConstraints);
        gridBagConstraints.gridy++;

        JPanel savegameVariablePanel = new JPanel();
        savegameVariablePanel.add(new JLabel("Savegame variable"));
        savegameVariablePanel.add(savegameVariableField);
        add(savegameVariablePanel, gridBagConstraints);
        gridBagConstraints.gridy++;

        // create listeners
        itemNameField.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent arg0) {

	        String itemName = itemNameField.getSelectedId();
		if (itemName.isEmpty()) {
		    treasure.setItemName(null);
		    variantField.setEnabled(false);
		    saveField.setEnabled(false);
		    savegameVariableField.setEnabled(false);
		}
		else {
		    treasure.setItemName(itemNameField.getSelectedId());
		    if (!variantField.isEnabled()) {
		        variantField.setEnabled(true);
			variantField.setValue(1);
		    }
		    saveField.setEnabled(true);
		    savegameVariableField.setEnabled(true);
		}
            }
        });

        variantField.addChangeListener(new ChangeListener() {

            public void stateChanged(ChangeEvent e) {
                treasure.setVariant(variantField.getNumber());
            }
        });

        saveField.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent e) {

                if (saveField.isSelected()) {
                    savegameVariableField.setEnabled(true);
                    treasure.setSavegameVariable(savegameVariableField.getText());
                }
                else {
                    savegameVariableField.setEnabled(false);
                    treasure.setSavegameVariable(null);
                }
            }
        });

        savegameVariableField.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent e) {
                treasure.setSavegameVariable(savegameVariableField.getText());
            }
        });
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
     * @param itemName Name identifying the treasure to give 
     * (possibly "_none" or "_random") or null for unspecified.
     * @param variant Variant of this item (null means unspecified).
     * @param savegameVariable Savegame variable that stores the treasure's state,
     * or -1 to make the treasure unsaved (null means unspecified).
     */
    public void setTreasure(String itemName, Integer variant, String savegameVariable) {

        treasure.setItemName(itemName);
        treasure.setVariant(variant);
        treasure.setSavegameVariable(savegameVariable);

        update();
    }

    /**
     * Updates this component according to the treasure.
     */
    private void update() {

        // Item name.
        if (treasure.getItemName() == null) {
            itemNameField.setSelectedId("");
        }
        else {
            itemNameField.setSelectedId(treasure.getItemName());
        }

        // Variant.
        if (treasure.getItemName() == null
                || treasure.getItemName() == Item.noneId) {
            variantField.setEnabled(false);
        }
        else {
            variantField.setEnabled(true);
            if (treasure.getVariant() == null) {
                variantField.setNumber(1);
            }
            else {
                variantField.setNumber(treasure.getVariant());
            }
        }

        // Savegame variable.
        if (treasure.getItemName() == null
                || treasure.getVariant() == null) {
            saveField.setEnabled(false);
            savegameVariableField.setEnabled(false);
        }
        else {
            saveField.setEnabled(true);
            if (treasure.getSavegameVariable() == null) {
                saveField.setSelected(false);
                savegameVariableField.setEnabled(false);
            }
            else {
                saveField.setSelected(true);
                savegameVariableField.setEnabled(true);
                savegameVariableField.setText(treasure.getSavegameVariable());
            }
        }
    }
}

