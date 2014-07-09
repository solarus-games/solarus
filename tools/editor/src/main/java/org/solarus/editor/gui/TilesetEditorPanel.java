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

import java.awt.*;
import javax.swing.*;

import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * Component to edit a tileset.
 */
public class TilesetEditorPanel extends AbstractEditorPanel {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * The list of tile patterns.
     */
    private TilePatternsView tilePatternsView;

    /**
     * The tileset image.
     */
    private TilesetImageView tilesetImageView;

    /**
     * Creates a tileset editor.
     * @param mainWindow The main window of the quest editor.
     * @param tilesetId Id of the tileset to open.
     * @throws QuestEditorException If the tileset could not be loaded.
     */
    public TilesetEditorPanel(EditorWindow mainWindow, String tilesetId)
            throws QuestEditorException {
        super(mainWindow, getEditorId(tilesetId));

        setLayout(new BorderLayout());

        // tile patterns list and tileset image

        // tile patterns list
        tilePatternsView = new TilePatternsView(mainWindow);
        tilePatternsView.setAlignmentY(Component.TOP_ALIGNMENT);
        tilePatternsView.setPreferredSize(new Dimension(200, Integer.MAX_VALUE));
        tilePatternsView.setMinimumSize(new Dimension(0, 0));

        // tileset image
        tilesetImageView = new TilesetImageView(true);
        JScrollPane tilesetImageScroller = new ViewScroller(tilesetImageView, tilesetImageView);
        tilesetImageScroller.setAlignmentY(Component.TOP_ALIGNMENT);

        JSplitPane tilesetPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, tilePatternsView, tilesetImageScroller);
        tilesetPanel.setContinuousLayout(true);
        tilesetPanel.setDividerLocation(400);
        // we must put our main panel in another panel
        // otherwise the background color of the window is bad
        add(tilesetPanel);

        tileset = new Tileset(tilesetId);

        // Notify the children views.
        tilePatternsView.setTileset(tileset);
        tilesetImageView.setTileset(tileset);
    }

    /**
     * Returns the id of any tileset editor that edits the specified tileset.
     * @param tilesetId Id of a tileset.
     * @return Id of an editor that edits this tileset.
     */
    public static String getEditorId(String tilesetId) {
        return Project.getTilesetFile(tilesetId).getAbsolutePath();
    }

    /**
     * This function is called when the user wants to close the current tileset.
     * If the tileset is not saved, we propose to save it.
     * @return false if the user canceled.
     */
    @Override
    public boolean checkCurrentFileSaved() {
        boolean result = true;

        if (tileset != null && !tileset.isSaved()) {
            int answer = JOptionPane.showConfirmDialog(this,
                    "Tileset '" + tileset.getName() + "' has been modified. Do you want to save it?",
                    "Save the modifications",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                save();
            } else if (answer == JOptionPane.CANCEL_OPTION) {
                result = false;
            }
        }

        return result;
    }

    /**
     * Give the resource opened in the editor
     * @return the tileset
     */
    public Tileset getTileset() {
        return tileset;
    }

    /**
     * Returns the type of resource represented by this editor.
     * @return The type of resource.
     */
    @Override
    public ResourceType getResourceType() {
        return ResourceType.TILESET;
    }

    /**
     * Returns the id of the resource element open in this editor.
     * @return The id of the tileset.
     */
    @Override
    public String getResourceId() {
        return tileset.getId();
    }

    /**
     * Give the name of the resource opened in the editor
     * @return the name of the tileset
     */
    @Override
    public String getTitle() {
        return tileset == null ? "" : "Tileset " + tileset.getName();
    }

    /**
     * Saves the current resource.
     */
    @Override
    public void save() {

        try {
            tileset.save();
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Could not save the tileset: " + ex.getMessage());
        }
    }

    /**
     * Closes this editor without confirmation.
     */
    @Override
    public void close() {
        tileset.deleteObservers();

        // Notify the children views.
        tilePatternsView.setTileset(null);
        tilesetImageView.setTileset(null);
    }
}
