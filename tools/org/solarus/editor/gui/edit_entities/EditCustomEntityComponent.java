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

import javax.swing.JCheckBox;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a custom entity.
 */
public class EditCustomEntityComponent extends EditEntityComponent {

    // specific fields
    private JCheckBox withModelField;
    private ResourceChooser modelField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditCustomEntityComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // Has a model?
        withModelField = new JCheckBox("Call a custom entity script");
        addField("Use a model", withModelField);

        // Model.
        modelField = new ResourceChooser(ResourceType.ENTITY, true);
        addField("Entity script to call", modelField);

        // Listeners.
        withModelField.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent ev) {
                modelField.setEnabled(withModelField.isSelected());
            }
        });

    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        CustomEntity customEntity = (CustomEntity) entity;

        String model = customEntity.getStringProperty("model");
        boolean hasModel = model != null && Project.getResource(ResourceType.ENTITY).exists(model);

        withModelField.setSelected(hasModel);
        modelField.setSelectedId(hasModel ? model : "");

        modelField.setEnabled(hasModel);
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

        String model = modelField.getSelectedId();
        if (!withModelField.isSelected()) {
            model = null;
        }

        return new ActionEditEntitySpecific(entity, model);
    }
}

