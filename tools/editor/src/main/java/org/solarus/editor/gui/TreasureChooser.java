package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.BadLocationException;

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
        treasure = new Treasure(null, null, null);

        // create the subcomponents
        itemNameField = new ItemChooser(includeNone);
        variantField = new NumberChooser(1, 1, Integer.MAX_VALUE);
        saveField = new JCheckBox("Save the treasure state");
        savegameVariableField = new JTextField(20);

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
                }
                else {
                    treasure.setItemName(itemNameField.getSelectedId());
                    if (!variantField.isEnabled()) {
                        variantField.setValue(1);
                    }
                    variantField.setEnabled(true);
                }
            }
        });

        variantField.addChangeListener(new ChangeListener() {

            public void stateChanged(ChangeEvent e) {
                if (variantField.isEnabled()) {
                    treasure.setVariant(variantField.getNumber());
                }
                else {
                    treasure.setVariant(null);
                }
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

        savegameVariableField.getDocument().addDocumentListener(new DocumentListener() {

            private void setTreasureSavegameVariable() {

                try {
                    int length = savegameVariableField.getDocument().getLength();
                    treasure.setSavegameVariable(savegameVariableField.getDocument().getText(0, length));
                }
                catch (BadLocationException ex) {
                    GuiTools.errorDialog(ex.getMessage());
                }
            }

            @Override
            public void changedUpdate(DocumentEvent e) {
                setTreasureSavegameVariable();
            }

            @Override
            public void insertUpdate(DocumentEvent e) {
                setTreasureSavegameVariable();
            }

            @Override
            public void removeUpdate(DocumentEvent e) {
                setTreasureSavegameVariable();
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
     * @param itemName Name identifying the treasure to give or null for no treasure.
     * @param variant Variant of this item (null means unspecified).
     * @param savegameVariable Savegame variable that stores the treasure's state,
     * or null to make the treasure unsaved (null means unspecified).
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

        if (treasure.getItemName() == null ||
                !treasure.isItemNameValid()) {
            itemNameField.setSelectedId("");
            variantField.setEnabled(false);
        }
        else {
            itemNameField.setSelectedId(treasure.getItemName());
            variantField.setEnabled(true);
            if (treasure.getVariant() == null) {
                variantField.setNumber(1);
            }
            else {
                Integer variant = treasure.getVariant();
                variantField.setNumber(variant);
            }
        }

        // Savegame variable.
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

