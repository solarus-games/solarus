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

import java.util.NoSuchElementException;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a destructible item.
 */
public class EditDestructibleComponent extends EditEntityComponent {

    // Specific fields of a this kind of entity.
    private TreasureChooser treasureField;
    private ResourceChooser spriteField;
    private JCheckBox withDestructionSoundField;
    private JLabel destructionSoundLabel;
    private ResourceChooser destructionSoundField;
    private JCheckBox withWeightField;
    private JLabel weightLabel;
    private NumberChooser weightField;
    private JCheckBox canBeCutField;
    private JCheckBox canExplodeField;
    private JCheckBox canRegenerateField;
    private JCheckBox withDamageOnEnemiesField;
    private JLabel damageOnEnemiesLabel;
    private NumberChooser damageOnEnemiesField;
    private JCheckBox withGroundField;
    private JLabel groundLabel;
    private EnumerationChooser<Ground> groundField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditDestructibleComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // Pickable treasure contained.
        treasureField = new TreasureChooser(true);
        addField("Treasure", treasureField);

        // Sprite.
        spriteField = new ResourceChooser(ResourceType.SPRITE, true);
        addField("Sprite", spriteField);

        // Destruction sound.
        withDestructionSoundField = new JCheckBox("Play a sound when destroyed");
        addField("Sound when destroyed", withDestructionSoundField);
        destructionSoundField = new ResourceChooser(ResourceType.SOUND, true);
        destructionSoundLabel = addField("", destructionSoundField);
        // Enable or disable the destruction sound field depending on the check box.
        withDestructionSoundField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                destructionSoundLabel.setEnabled(withDestructionSoundField.isSelected());
                destructionSoundField.setEnabled(withDestructionSoundField.isSelected());
            }
        });

        // Lifting the object.
        withWeightField = new JCheckBox("Can be lifted");
        addField("Weight", withWeightField);
        weightField = new NumberChooser(0, 0, Integer.MAX_VALUE);
        weightLabel = addField("Lift ability required", weightField);
        // Enable or disable the weight field depending on the check box.
        withWeightField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                weightLabel.setEnabled(withWeightField.isSelected());
                weightField.setEnabled(withWeightField.isSelected());
            }
        });

        // Cutting the object.
        canBeCutField = new JCheckBox("Can be cut");
        addField("Cutting the object", canBeCutField);

        // Exploding.
        canExplodeField = new JCheckBox("Can explode");
        addField("Exploding", canExplodeField);

        // Regenerating.
        canRegenerateField = new JCheckBox("Can regenerate");
        addField("Regeneration", canRegenerateField);

        // Hurting enemies when thrown.
        withDamageOnEnemiesField = new JCheckBox("Hurt enemies when thrown");
        addField("Damage on enemies", withDamageOnEnemiesField);
        damageOnEnemiesField = new NumberChooser(1, 1, Integer.MAX_VALUE);
        damageOnEnemiesLabel = addField("Life points to remove", damageOnEnemiesField);
        // Enable or disable the damage field depending on the check box.
        withDamageOnEnemiesField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                damageOnEnemiesLabel.setEnabled(withDamageOnEnemiesField.isSelected());
                damageOnEnemiesField.setEnabled(withDamageOnEnemiesField.isSelected());
            }
        });

        // Modified ground.
        withGroundField = new JCheckBox("Set a special ground");
        addField("", withGroundField);
        groundField = new EnumerationChooser<Ground>(Ground.class);
        groundLabel = addField("Ground to define", groundField);
        // Enable or disable the ground field depending on the check box.
        withGroundField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                groundLabel.setEnabled(withGroundField.isSelected());
                groundField.setEnabled(withGroundField.isSelected());
            }
        });
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {

        super.update(); // update the common fields

        // Get the properties.
        Destructible destructible = (Destructible) entity;
        String sprite = destructible.getStringProperty("sprite");
        String destructionSound = destructible.getStringProperty("destruction_sound");
        int weight = destructible.getIntegerProperty("weight");
        boolean canBeBut = destructible.getBooleanProperty("can_be_cut");
        boolean canExplode = destructible.getBooleanProperty("can_explode");
        boolean canRegenerate = destructible.getBooleanProperty("can_regenerate");
        int damageOnEnemies = destructible.getIntegerProperty("damage_on_enemies");
        Ground ground;
        try {
            ground = Ground.getByName(destructible.getStringProperty("ground"));
        }
        catch (QuestEditorException ex) {
            ground = Ground.WALL;
        }

        // Treasure.
        treasureField.setTreasure(
                destructible.getStringProperty("treasure_name"),
                destructible.getIntegerProperty("treasure_variant"),
                destructible.getStringProperty("treasure_savegame_variable"));

        // Sprite.
        try {
            spriteField.setSelectedId(sprite);
        }
        catch (NoSuchElementException ex) {
            spriteField.setSelectedId("");
        }

        // Destruction sound.
        try {
            if (destructionSound == null) {
                // No destruction sound.
                withDestructionSoundField.setSelected(false);
                destructionSoundLabel.setEnabled(false);
                destructionSoundField.setEnabled(false);
            }
            else {
                withDestructionSoundField.setSelected(true);
                destructionSoundLabel.setEnabled(true);
                destructionSoundField.setEnabled(true);
                destructionSoundField.setSelectedId(destructionSound);
            }
        }
        catch (NoSuchElementException ex) {
            destructionSoundField.setSelectedId("");
        }

        // Weight.
        if (weight == -1) {
            // Cannot be lifted.
            withWeightField.setSelected(false);
            weightLabel.setEnabled(false);
            weightField.setEnabled(false);
        }
        else {
            // Can be lifted.
            withWeightField.setSelected(true);
            weightLabel.setEnabled(true);
            weightField.setEnabled(true);
            weightField.setNumber(weight);
        }

        // Cutting the object.
        canBeCutField.setSelected(canBeBut);

        // Exploding.
        canExplodeField.setSelected(canExplode);

        // Regenerating.
        canRegenerateField.setSelected(canRegenerate);

        // Damage on enemies.
        if (damageOnEnemies == 0) {
            // Cannot hurt enemies.
            withDamageOnEnemiesField.setSelected(false);
            damageOnEnemiesLabel.setEnabled(false);
            damageOnEnemiesField.setEnabled(false);
        }
        else {
            // Can hurt enemies.
            withDamageOnEnemiesField.setSelected(true);
            damageOnEnemiesLabel.setEnabled(true);
            damageOnEnemiesField.setEnabled(true);
            damageOnEnemiesField.setNumber(damageOnEnemies);
        }

        // Modified ground.
        if (ground == Ground.WALL) {
            // No special ground.
            withGroundField.setSelected(false);
            groundLabel.setEnabled(false);
            groundField.setEnabled(false);
        }
        else {
            // Define a special ground. 
            withGroundField.setSelected(true);
            groundLabel.setEnabled(true);
            groundField.setEnabled(true);
            groundField.setValue(ground);
        }
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

        String treasureName = treasureField.getTreasure().getItemName();
        Integer treasureVariant = treasureField.getTreasure().getVariant();
        String treasureSavegameVariable = treasureField.getTreasure().getSavegameVariable();
        String sprite = spriteField.getSelectedId();
        String destructionSound = null;
        if (withDestructionSoundField.isSelected()) {
            destructionSound = destructionSoundField.getSelectedId();
        }
        int weight = -1;
        if (withWeightField.isSelected()) {
            weight = weightField.getNumber();
        }
        boolean canBeCut = canBeCutField.isSelected();
        boolean canExplode = canExplodeField.isSelected();
        boolean canRegenerate = canRegenerateField.isSelected();
        int damageOnEnemies = 0;
        if (withDamageOnEnemiesField.isSelected()) {
            damageOnEnemies = damageOnEnemiesField.getNumber();
        }
        Ground ground = Ground.WALL;
        if (withGroundField.isSelected()) {
            ground = groundField.getValue();
        }

        return new ActionEditEntitySpecific(entity,
                treasureName,
                treasureVariant == null ? null : Integer.toString(treasureVariant),
                treasureSavegameVariable,
                sprite,
                destructionSound,
                Integer.toString(weight),
                canBeCut ? "1" : "0",
                canExplode ? "1" : "0",
                canRegenerate ? "1" : "0",
                Integer.toString(damageOnEnemies),
                ground.getName()
                );
    }
}

