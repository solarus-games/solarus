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

import javax.swing.JCheckBox;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a stream.
 */
public class EditStreamComponent extends EditEntityComponent {

    // specific fields
    private JCheckBox withSpriteField;
    private ResourceChooser spriteField;
    private NumberChooser speedField;
    private JCheckBox allowMovementField;
    private JCheckBox allowAttackField;
    private JCheckBox allowItemField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditStreamComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // Has a sprite?
        withSpriteField = new JCheckBox("Display a sprite");
        addField("Visibility", withSpriteField);

        // Sprite name.
        spriteField = new ResourceChooser(ResourceType.SPRITE, true);
        addField("Sprite name", spriteField);

        // Listeners.
        withSpriteField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                spriteField.setEnabled(withSpriteField.isSelected());
            }
        });

        // Speed.
        speedField = new NumberChooser(40, 1, Integer.MAX_VALUE);
        addField("Speed", speedField);

        // Allow movement.
        allowMovementField = new JCheckBox("Allow to move");
        addField("Movements", allowMovementField);

        // Allow attack.
        allowAttackField = new JCheckBox("Allow to use the sword");
        addField("Sword", allowAttackField);

        // Allow equipment items.
        allowItemField = new JCheckBox("Allow to use equipment items");
        addField("Items", allowItemField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        Stream stream = (Stream) entity;

        String sprite = stream.getStringProperty("sprite");
        boolean hasSprite = sprite != null;

        withSpriteField.setSelected(hasSprite);
        spriteField.setSelectedId(hasSprite ? sprite : "");
        spriteField.setEnabled(hasSprite);

        speedField.setNumber(stream.getIntegerProperty("speed"));
        allowMovementField.setSelected(stream.getBooleanProperty("allow_movement"));
        allowAttackField.setSelected(stream.getBooleanProperty("allow_attack"));
        allowItemField.setSelected(stream.getBooleanProperty("allow_item"));
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

        return new ActionEditEntitySpecific(
                entity,
                sprite,
                Integer.toString(speedField.getNumber()),
                (allowMovementField.isSelected() ? "1" : "0"),
                (allowAttackField.isSelected() ? "1" : "0"),
                (allowItemField.isSelected() ? "1" : "0")
        );
    }
}

