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

import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * A component to edit a teletransporter.
 */
public class EditTeletransporterComponent extends EditEntityComponent {

    // specific fields of a teletransporter
    private JCheckBox withSpriteField;
    private ResourceChooser spriteField;
    private JCheckBox withSoundField;
    private ResourceChooser soundField;
    private EnumerationChooser<Transition> transitionField;
    private ResourceChooser mapField;
    private EntityChooser destinationField;

    private static final String samePointText = "<Same point>";
    private static final String sidePointText = "<Side of the map>";

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditTeletransporterComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // transition
        transitionField = new EnumerationChooser<Transition>(Transition.class);
        addField("Transition", transitionField);

        // destination map
        mapField = new ResourceChooser(ResourceType.MAP, true);
        addField("Destination map", mapField);

        // destination point
        destinationField = new EntityChooser(null, EntityType.DESTINATION,
                new String[] {"", samePointText, sidePointText});
        addField("Destination point", destinationField);

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

        // listeners
        mapField.addActionListener(new ActionListenerChangeDestinationMap());

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

    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        Teletransporter teletransporter = (Teletransporter) entity;

        String sprite = teletransporter.getProperty("sprite");
        boolean hasSprite = sprite != null;
        withSpriteField.setSelected(hasSprite);
        spriteField.setSelectedId(hasSprite ? sprite : "");
        spriteField.setEnabled(hasSprite);

        String sound = teletransporter.getProperty("sound");
        boolean hasSound = sound != null;
        withSoundField.setSelected(hasSound);
        soundField.setSelectedId(hasSound ? sound : "");
        soundField.setEnabled(hasSound);

        transitionField.setValue(Transition.get(teletransporter.getIntegerProperty("transition")));
        mapField.setSelectedId(teletransporter.getProperty("destination_map"));

        String destinationName = teletransporter.getProperty("destination");
        if (destinationName.equals("_same")) {
            destinationName = samePointText;
        }
        else if (destinationName.equals("_side")) {
            destinationName = sidePointText;
        }
        destinationField.setSelectedName(destinationName);
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

        String destinationMapId = mapField.getSelectedId();
        String destinationName = destinationField.getSelectedName();

        if (destinationName.equals(samePointText)) {
            destinationName = "_same";
        }
        else if (destinationName.equals(sidePointText)) {
            destinationName = "_side";
        }

        return new ActionEditEntitySpecific(entity,
                sprite, sound,
                Integer.toString(transitionField.getValue().getId()),
                destinationMapId, destinationName);
    }

    /**
     * Action performed when the destination map field is changed.
     * The entrance field is reloaded to show the entrances of this map.
     */
    private class ActionListenerChangeDestinationMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {

            try {
                String mapId = mapField.getSelectedId();

                if (mapId.length() != 0) { // a map has just been selected
                    destinationField.setMap(new Map(mapId));
                }
                else {
                    destinationField.setMap(null);
                }
            }
            catch (ZSDXException ex) {
                GuiTools.warningDialog("The destination map is invalid: " + ex.getMessage());
            }
        }
    }
}

