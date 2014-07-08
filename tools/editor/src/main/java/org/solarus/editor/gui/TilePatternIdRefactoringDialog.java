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
import org.solarus.editor.entities.*;
import javax.swing.*;

/**
 * \brief A dialog that prompts the user to change the id of a tile pattern
 * in a tileset.
 *
 * The dialog is composed of a text field to input the new id and of a
 * checkbox to update maps refering to this tile pattern.
 */
public class TilePatternIdRefactoringDialog extends OkCancelDialog {

    /**
     * The tileset impacted by this modification.
     */
    private Tileset tileset;

    /**
     * The pattern id before the change.
     */
    private String oldPatternId;

    /**
     * The form to be filled by the user.
     */
    private TilePatternIdRefactoringComponent form;

    /**
     * Constructor.
     * @param tileset A tileset.
     * @param oldPatternId The tile pattern id to change in this tileset.
     */
    public TilePatternIdRefactoringDialog(Tileset tileset, String oldPatternId) {
        super("Rename tile pattern '" + oldPatternId + "'", false);

        this.tileset = tileset;
        this.oldPatternId = oldPatternId;

        form = new TilePatternIdRefactoringComponent(tileset, oldPatternId);
        setComponent(form);
    }

    /**
     * Performs the refactoring.
     */
    @Override
    protected void applyModifications() throws QuestEditorException {

        String newPatternId = form.getTilePatternId();
        if (newPatternId.equals(oldPatternId)) {
            // Nothing to do: quietly return.
            return;
        }

        tileset.changeTilePatternId(oldPatternId, newPatternId);

        // TODO update maps if requested and save the tileset
    }
}

