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

import javax.swing.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a wall.
 */
public class EditWallComponent extends EditEntityComponent {

    // specific fields of a wall
    private JCheckBox stopsHeroField;
    private JCheckBox stopsEnemiesField;
    private JCheckBox stopsNPCsField;
    private JCheckBox stopsBlocksField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditWallComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // hero
        stopsHeroField = new JCheckBox("Obstacle for the hero");
        addField("Hero", stopsHeroField);

        // enemies
        stopsEnemiesField = new JCheckBox("Obstacle for enemies");
        addField("Enemies", stopsEnemiesField);

        // non-playing characters
        stopsNPCsField = new JCheckBox("Obstacle for non-playing characters");
        addField("NPCs", stopsNPCsField);

        // blocks
        stopsBlocksField = new JCheckBox("Obstacle for blocks and statues");
        addField("Blocks", stopsBlocksField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        Wall wall = (Wall) entity;
        stopsHeroField.setSelected(wall.getBooleanProperty("stops_hero"));
        stopsEnemiesField.setSelected(wall.getBooleanProperty("stops_enemies"));
        stopsNPCsField.setSelected(wall.getBooleanProperty("stops_npcs"));
        stopsBlocksField.setSelected(wall.getBooleanProperty("stops_blocks"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

        return new ActionEditEntitySpecific(entity,
                stopsHeroField.isSelected() ? 1 : 0, stopsEnemiesField.isSelected() ? 1 : 0,
                stopsNPCsField.isSelected() ? 1 : 0, stopsBlocksField.isSelected() ? 1 : 0);
    }
}
