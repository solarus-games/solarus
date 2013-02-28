/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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
import java.util.NoSuchElementException;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a door.
 */
public class EditDoorComponent extends EditEntityComponent {

    // specific fields of this kind of entity
    private ResourceChooser spriteField;
    private JCheckBox saveField;
    private JTextField savegameVariableField;
    private JLabel savegameVariableLabel;
    private EnumerationChooser<Door.OpeningMethod> openingMethodField;
    private JTextField requiredSavegameVariableField;
    private JLabel requiredSavegameVariableLabel;
    private JCheckBox requiredSavegameVariableConsumedField;
    private JLabel requiredSavegameVariableConsumedLabel;
    private ItemChooser requiredItemField;
    private JLabel requiredItemLabel;
    private JCheckBox requiredItemConsumedField;
    private JLabel requiredItemConsumedLabel;
    private JCheckBox showCannotOpenDialogField;
    private JTextField cannotOpenDialogField;
    private JLabel cannotOpenDialogLabel;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditDoorComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // sprite name
        spriteField = new ResourceChooser(ResourceType.SPRITE, true);
        addField("Sprite", spriteField);

        // saving option
        saveField = new JCheckBox("Save the door state");
        addField("Savegame", saveField);

        // savegame variable
        savegameVariableField = new JTextField(20);
        savegameVariableLabel = addField("Savegame variable", savegameVariableField);

        // enable or disable the 'savegame variable' field depending on the check box
        saveField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                savegameVariableField.setEnabled(saveField.isSelected());
                savegameVariableLabel.setEnabled(saveField.isSelected());
            }
        });

        // opening method, condition and condition consumed
        openingMethodField = new EnumerationChooser<Door.OpeningMethod>(Door.OpeningMethod.class);
        addField("Opening method", openingMethodField);

        requiredSavegameVariableField = new JTextField(20);
        requiredSavegameVariableLabel = addField("Needs savegame variable to open", requiredSavegameVariableField);
        requiredSavegameVariableConsumedField = new JCheckBox("Reset/decrement this value when opening");
        requiredSavegameVariableConsumedLabel = addField("", requiredSavegameVariableConsumedField);

        requiredItemField = new ItemChooser(false);
        requiredItemLabel = addField("Needs equipment item to open", requiredItemField);

        requiredItemConsumedField = new JCheckBox("Remove/decrement this item when opening");
        requiredItemConsumedLabel = addField("", requiredItemConsumedField);

        openingMethodField.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ev) {

                requiredSavegameVariableField.setEnabled(false);
                requiredSavegameVariableLabel.setEnabled(false);
                requiredSavegameVariableConsumedField.setEnabled(false);
                requiredSavegameVariableConsumedLabel.setEnabled(false);
                requiredItemField.setEnabled(false);
                requiredItemLabel.setEnabled(false);
                requiredItemConsumedField.setEnabled(false);
                requiredItemConsumedLabel.setEnabled(false);
                Door.OpeningMethod openingMethod = openingMethodField.getValue();
                if (openingMethod == Door.OpeningMethod.BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
                    requiredSavegameVariableField.setEnabled(true);
                    requiredSavegameVariableLabel.setEnabled(true);
                    requiredSavegameVariableConsumedField.setEnabled(true);
                    requiredSavegameVariableConsumedLabel.setEnabled(true);
                }
                else if (openingMethod == Door.OpeningMethod.BY_INTERACTION_IF_ITEM) {
                    requiredItemField.setEnabled(true);
                    requiredItemLabel.setEnabled(true);
                    requiredItemConsumedField.setEnabled(true);
                    requiredItemConsumedLabel.setEnabled(true);
                }
            }
        });

        // show dialog option
        showCannotOpenDialogField = new JCheckBox("Show a dialog if the hero fails to open");
        addField("Action if not allowed", showCannotOpenDialogField);

        // dialog if cannot open
        cannotOpenDialogField = new JTextField(15);
        cannotOpenDialogLabel = addField("Dialog id to show", cannotOpenDialogField);

        // enable or disable the 'dialog to show' field depending on the check box
        showCannotOpenDialogField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                cannotOpenDialogLabel.setEnabled(showCannotOpenDialogField.isSelected());
                cannotOpenDialogField.setEnabled(showCannotOpenDialogField.isSelected());
            }
        });
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        Door door = (Door) entity;
        String sprite = door.getProperty("sprite");
        String savegameVariable = door.getProperty("savegame_variable");
        Door.OpeningMethod openingMethod = Door.OpeningMethod.get(door.getProperty("opening_method"));
        String openingCondition = door.getProperty("opening_condition");
        boolean openingConditionConsumed = door.getBooleanProperty("opening_condition_consumed");
        String cannotOpenDialog = door.getProperty("cannot_open_dialog_id");

        try {
          spriteField.setSelectedId(sprite);
        }
        catch (NoSuchElementException ex) {
          spriteField.setSelectedId("");
        }

        if (savegameVariable != null) {
          savegameVariableField.setText(savegameVariable);
          savegameVariableField.setEnabled(true);
          savegameVariableLabel.setEnabled(true);
          saveField.setSelected(true);
        }
        else {
          savegameVariableField.setEnabled(false);
          savegameVariableLabel.setEnabled(false);
          saveField.setSelected(false);
        }

        openingMethodField.setValue(openingMethod);

        requiredSavegameVariableField.setEnabled(false);
        requiredSavegameVariableLabel.setEnabled(false);
        requiredSavegameVariableConsumedField.setEnabled(false);
        requiredSavegameVariableConsumedLabel.setEnabled(false);
        requiredItemField.setEnabled(false);
        requiredItemLabel.setEnabled(false);
        requiredItemConsumedField.setEnabled(false);
        requiredItemConsumedLabel.setEnabled(false);
        if (openingMethod == Door.OpeningMethod.BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
            requiredSavegameVariableField.setText(openingCondition);
            requiredSavegameVariableField.setEnabled(true);
            requiredSavegameVariableLabel.setEnabled(true);
            requiredSavegameVariableConsumedField.setSelected(openingConditionConsumed);
            requiredSavegameVariableConsumedField.setEnabled(true);
            requiredSavegameVariableConsumedLabel.setEnabled(true);
        }
        else if (openingMethod == Door.OpeningMethod.BY_INTERACTION_IF_ITEM) {
            requiredItemField.setSelectedId(openingCondition);
            requiredItemField.setEnabled(true);
            requiredItemLabel.setEnabled(true);
            requiredItemConsumedField.setSelected(openingConditionConsumed);
            requiredItemConsumedField.setEnabled(true);
            requiredItemConsumedLabel.setEnabled(true);
        }

        if (cannotOpenDialog != null) {
          cannotOpenDialogField.setText(cannotOpenDialog);
          cannotOpenDialogField.setEnabled(true);
          cannotOpenDialogLabel.setEnabled(true);
          showCannotOpenDialogField.setSelected(true);
        }
        else {
          cannotOpenDialogField.setEnabled(false);
          cannotOpenDialogLabel.setEnabled(false);
          showCannotOpenDialogField.setSelected(false);
        }
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

        String sprite = spriteField.getSelectedId();
        String savegameVariable = savegameVariableField.isEnabled() ?
                savegameVariableField.getText() : null;
        Door.OpeningMethod openingMethod = openingMethodField.getValue();
        String openingCondition = null;
        boolean openingConditionConsumed = false;
        if (openingMethod == Door.OpeningMethod.BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
            openingCondition = requiredSavegameVariableField.getText();
            openingConditionConsumed = requiredSavegameVariableConsumedField.isSelected();
        }
        else if (openingMethod == Door.OpeningMethod.BY_INTERACTION_IF_ITEM) {
            openingCondition = requiredItemField.getSelectedId();
            openingConditionConsumed = requiredItemConsumedField.isSelected();
        }
        String cannotOpenDialog = showCannotOpenDialogField.isEnabled() ?
                cannotOpenDialogField.getText() : null;

        return new ActionEditEntitySpecific(entity,
                sprite,
                savegameVariable,
                openingMethod.getId(),
                openingCondition,
                openingConditionConsumed ? "1" : "0",
                cannotOpenDialog);
    }
}
