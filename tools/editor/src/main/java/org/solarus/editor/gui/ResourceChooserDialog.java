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
package org.solarus.editor.gui;

import org.solarus.editor.*;

/**
 * A dialog box with a combo box to choose a resource element.
 */
public class ResourceChooserDialog extends OkCancelDialog {

    /**
     * Id of the element chosen (or an empty string if the user cancelled).
     */
    private String id = "";

    /**
     * The component shown in the dialog box.
     */
    private ResourceChooser chooser;

    /**
     * Constructor.
     */
    public ResourceChooserDialog(ResourceType resourceType) {
//        super("Select a " + resourceNames[resourceType.getId()], false);
//
//        setBorderTitle("Please select a " + resourceNames[resourceType.getId()]);

        super("Select a " + resourceType.getName(), false);

        setBorderTitle("Please select a " + resourceType.getName());

            chooser = new ResourceChooser(resourceType, false);
            setComponent(chooser);

    }

    /**
     * Takes into account the modifications made by the user.
     */
    public void applyModifications() throws QuestEditorException {
        id = chooser.getSelectedId();
    }

    /**
     * Returns the id of the element that the user has just selected.
     * @return The id of the element, or an empty string if he canceled.
     */
    public String getSelectedId() {

        String selectedId = this.id;
        this.id = "";
        return selectedId;
    }
}
