/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
import org.solarus.editor.entities.Switch.Subtype;

/**
 * A component to edit a switch.
 */
public class EditSwitchComponent extends EditEntityComponent {

    // Specific fields of a switch.
    private JCheckBox withSpriteField;
    private ResourceChooser spriteField;
    private JCheckBox withSoundField;
    private ResourceChooser soundField;
    private JCheckBox needsBlockField;
    private JCheckBox inactivateWhenLeavingField;

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

        // has a sprite?
        withSpriteField = new JCheckBox("Display a sprite");
        addField("Visibility", withSpriteField);

        // sprite name
        spriteField = new ResourceChooser(ResourceType.SPRITE, true);
        addField("Sprite name", spriteField);

        // has a sound?
        withSoundField = new JCheckBox("Play a sound");
        addField("Sound", withSoundField);

        // sound name
        soundField = new ResourceChooser(ResourceType.SOUND, true);
        addField("Sound id", soundField);

        // needs block
        needsBlockField = new JCheckBox("Requires a block or a statue to be activated");
        addField("Activation", needsBlockField);

        // inactivate when leaving
        inactivateWhenLeavingField = new JCheckBox("Inactivate the switch when leaving");
        addField("Stay on switch", inactivateWhenLeavingField);

        // Listeners.
        withSpriteField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                spriteField.setEnabled(withSpriteField.isSelected());
            }
        });

        withSoundField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                soundField.setEnabled(withSoundField.isSelected());
            }
        });

        // Disable the 'needs block' and 'inactivate when leaving' fields
        // when the subtype is other than walkable.
        subtypeField.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ev) {
                boolean walkable = subtypeField.getValue() == Subtype.WALKABLE;
                if (walkable) {
                    needsBlockField.setEnabled(true);
                    inactivateWhenLeavingField.setEnabled(true);
                }
                else {
                    needsBlockField.setSelected(false);
                    inactivateWhenLeavingField.setEnabled(false);
                    needsBlockField.setEnabled(false);
                    inactivateWhenLeavingField.setEnabled(false);
                }
            }
        });
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        Switch sw = (Switch) entity;

        String sprite = sw.getStringProperty("sprite");
        boolean hasSprite = sprite != null;
        withSpriteField.setSelected(hasSprite);
        spriteField.setSelectedId(hasSprite ? sprite : "");
        spriteField.setEnabled(hasSprite);

        String sound = sw.getStringProperty("sound");
        boolean hasSound = sound != null;
        withSoundField.setSelected(hasSound);
        soundField.setSelectedId(hasSound ? sound : "");
        soundField.setEnabled(hasSound);

        needsBlockField.setSelected(sw.getBooleanProperty("needs_block"));
        inactivateWhenLeavingField.setSelected(sw.getBooleanProperty("inactivate_when_leaving"));
        boolean walkable = entity.getSubtype() == Subtype.WALKABLE;
        if (walkable) {
            needsBlockField.setEnabled(true);
            inactivateWhenLeavingField.setEnabled(true);
        }
        else {
            needsBlockField.setSelected(false);
            inactivateWhenLeavingField.setEnabled(false);
            needsBlockField.setEnabled(false);
            inactivateWhenLeavingField.setEnabled(false);
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

        String sound = soundField.getSelectedId();
        if (!withSoundField.isSelected()) {
            sound = null;
        }

        String needsBlock = needsBlockField.isSelected() ? "1" : "0";
        String inactivateWhenLeaving = inactivateWhenLeavingField.isSelected() ? "1" : "0";

        return new ActionEditEntitySpecific(
                entity,
                sprite,
                sound,
                needsBlock,
                inactivateWhenLeaving
        );
    }
}

