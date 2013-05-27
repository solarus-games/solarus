/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
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
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;
import org.solarus.editor.entities.NPC.Subtype;

/**
 * A component to edit an NPC.
 */
public class EditNPCComponent extends EditEntityComponent {

    // specific fields of this entity
    private JCheckBox withSpriteField;
    private ResourceChooser spriteField;
    private RadioField behaviorField;
    private JTextField dialogField;
    private ItemChooser itemField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditNPCComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // has a sprite?
        withSpriteField = new JCheckBox("Display a sprite");
        addField("Visibility", withSpriteField);

        // sprite name
        spriteField = new ResourceChooser(ResourceType.SPRITE, true);
        addField("Sprite name", spriteField);

        // behavior
        behaviorField = new RadioField("Show a message", "Call the map script", "Call an item script");
        addField("Action", behaviorField);

        // dialog
        dialogField = new JTextField(15);
        addField("Dialog id to show", dialogField);

        // item
        itemField = new ItemChooser(false);
        addField("Item script to call", itemField);

        // listeners
        withSpriteField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                spriteField.setEnabled(withSpriteField.isSelected());
            }
        });

        behaviorField.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ev) {
                dialogField.setEnabled(behaviorField.getSelectedIndex() == 0);
                itemField.setEnabled(behaviorField.getSelectedIndex() == 2);
            }
        });

        subtypeField.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ev) {

                Subtype subtype = (Subtype) subtypeField.getValue();
                withSpriteField.setEnabled(subtype != Subtype.USUAL_NPC);
            }
        });
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        NPC npc = (NPC) entity;
        String sprite = npc.getStringProperty("sprite");
        String behavior = npc.getStringProperty("behavior");

        boolean hasSprite = sprite != null;

        withSpriteField.setSelected(hasSprite);
        spriteField.setSelectedId(hasSprite ? sprite : "");

        spriteField.setEnabled(hasSprite);

        if (behavior.equals("map")) {
          behaviorField.setSelectedIndex(1);
          dialogField.setEnabled(false);
          itemField.setEnabled(false);
        }
        else if (behavior.substring(0, 5).equals("item#")) {
          behaviorField.setSelectedIndex(2);
          dialogField.setEnabled(false);
          itemField.setEnabled(true);
          itemField.setSelectedId(behavior.substring(5));
        }
        else if (behavior.substring(0, 7).equals("dialog#")) {
          behaviorField.setSelectedIndex(0);
          dialogField.setEnabled(true);
          itemField.setEnabled(false);
          dialogField.setText(behavior.substring(7));
        }
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

        String sprite = spriteField.getSelectedId();
        if (!withSpriteField.isSelected()) {
            sprite = null;
        }

        String behavior = "map";
        if (dialogField.isEnabled()) {
          behavior = "dialog#" + dialogField.getText();
        }
        else if (itemField.isEnabled()) {
          behavior = "item#" + itemField.getSelectedId();
        }

        return new ActionEditEntitySpecific(entity, sprite, behavior);
    }
}

