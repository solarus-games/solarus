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
import java.io.*;
import java.nio.charset.*;
import java.nio.file.*;
import java.util.*;
import java.util.regex.Pattern;

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
     * The main window of the quest editor.
     */
    private EditorWindow mainWindow;

    /**
     * Constructor.
     * @param tileset A tileset.
     * @param oldPatternId The tile pattern id to change in this tileset.
     */
    public TilePatternIdRefactoringDialog(
            EditorWindow mainWindow,
            Tileset tileset,
            String oldPatternId
            ) {
        super("Rename tile pattern '" + oldPatternId + "'", false);

        this.mainWindow = mainWindow;
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

        boolean updateMaps = form.isUpdateMapsChecked();
        if (updateMaps) {
            // Check that no map is open.
            Collection<AbstractEditorPanel> mapEditors = mainWindow.getOpenEditors(ResourceType.MAP);
            if (!mapEditors.isEmpty()) {
                throw new QuestEditorException("Please close all maps before updating tile pattern references.");
            }
        }

        tileset.changeTilePatternId(oldPatternId, newPatternId);

        if (updateMaps) {
            // First save the tileset.
            tileset.save();

            // Update all maps that use this tileset.
            Resource mapResource = Project.getResourceDatabase().getResource(ResourceType.MAP);
            String[] mapIds = mapResource.getIds();

            for (String mapId: mapIds) {
                updateMap(mapId);
            }
        }
    }

    /**
     * Updates references to the modified tile pattern in the specified map.
     * Does nothing if the map does not use this tileset.
     * @param mapId Id of the map to update.
     */
    private void updateMap(String mapId) throws QuestEditorException {

        // We don't create a Map object for performance reasons.
        // This would load the entire map with all its entities.
        // Instead, we just find and replace the appropriate text in the map
        // data file.

        try {
            File mapFile = Project.getMapFile(mapId);
            Path path = Paths.get(mapFile.getAbsolutePath());
            Charset charset = StandardCharsets.UTF_8;

            String content = new String(Files.readAllBytes(path), charset);
            String tilesetLine = "\n  tileset = \"" + tileset.getId() + "\",\n";
            if (!content.contains(tilesetLine)) {
                // This map uses another tileset: nothing to do.
                return;
            }

            String newPatternId = form.getTilePatternId();
            String patternRegex = "\n  pattern = \"?" + Pattern.quote(oldPatternId) + "\"?,\n";
            content = content.replaceAll(patternRegex, "\n  pattern = \"" + newPatternId + "\",\n");
            Files.write(path, content.getBytes(charset));
        }
        catch (IOException ex) {
            throw new QuestEditorException("Failed to update map '" + mapId + "': " + ex.getMessage());
        }
    }
}

