/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
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
package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;
import org.solarus.editor.entities.*;

/**
 * This component shows a list of tile patterns and allows the user to select them.
 * The list is displayed as a JList with an icon for each tile.
 * The id of the tile pattern in the tileset is different from it index in the list items,
 * because in a tileset, when a tile pattern is removed, the other tiles keep their ids.
 */
public class TilePatternsView extends JPanel {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * Component with the properties of the tileset.
     */
    private TilesetPropertiesView tilesetPropertiesView;

    /**
     * The tile patterns list.
     */
    private JList tilePatternsList;

    /**
     * The tile list pattern model.
     */
    private TilePatternsListModel tilePatternsListModel;

    /**
     * The icon associated to each tile.
     */
    private ArrayList<TilePatternIcon> tilePatternIcons;

    /**
     * View of the selected tile.
     */
    private TilePatternView tilePatternView;

    /**
     * Constructor.
     */
    public TilePatternsView() {
	super();

	tilePatternIcons = new ArrayList<TilePatternIcon>();

	setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
	setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

	// tileset properties
	// the tile pattern list
	// view of the current tile pattern

	// tileset properties
	tilesetPropertiesView = new TilesetPropertiesView();
	tilesetPropertiesView.setMaximumSize(new Dimension(Integer.MAX_VALUE, 120));
	tilesetPropertiesView.setAlignmentX(Component.LEFT_ALIGNMENT);

	// list
	tilePatternsListModel = new TilePatternsListModel();
	tilePatternsList = new JList(tilePatternsListModel);
	tilePatternsList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	tilePatternsList.setLayoutOrientation(JList.HORIZONTAL_WRAP);
	tilePatternsList.setVisibleRowCount(-1); // make the rows as wide as possible
 	tilePatternsList.getSelectionModel().addListSelectionListener(new TilePatternListSelectionListener());
	tilePatternsList.setCellRenderer(new TilePatternListRenderer());

	tilePatternsList.addKeyListener(new KeyAdapter() {
		public void keyPressed(KeyEvent keyEvent) {
		    if (keyEvent.getKeyCode() == KeyEvent.VK_DELETE) {
			if (tileset != null && tileset.getSelectedTilePattern() != null) {
			    tileset.removeTilePattern();
			}
		    }
		}
	    });

	JScrollPane listScroller = new JScrollPane(tilePatternsList);
 	listScroller.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));
	listScroller.setAlignmentX(Component.LEFT_ALIGNMENT);

	// tile view
	tilePatternView = new TilePatternView();
 	tilePatternView.setMaximumSize(new Dimension(Integer.MAX_VALUE, 300));
	tilePatternView.setAlignmentX(Component.LEFT_ALIGNMENT);

	add(tilesetPropertiesView);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(listScroller);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(tilePatternView);
    }

    /**
     * Sets the tileset observed.
     * @param tileset the current tileset, or null if there is no tileset 
     */
    public void setTileset(Tileset tileset) {
	
	if (tileset != this.tileset) {

	    if (this.tileset != null) {
		this.tileset.deleteObserver(tilePatternsListModel);
	    }

	    this.tileset = tileset;
	    tilesetPropertiesView.setTileset(tileset);
	    tilePatternView.setTileset(tileset);

	    if (tileset != null) {
		tileset.addObserver(tilePatternsListModel);
	    
		loadIcons();
		tilePatternsListModel.update(tileset, null);
	    }
	    else {
		repaint();
	    }
	}
    }

    /**
     * Loads the icons for the tile patterns list.
     */
    private void loadIcons() {

	tilePatternIcons.clear();

	for (TilePattern tilePattern: tileset.getTilePatterns()) {
	    tilePatternIcons.add(new TilePatternIcon(tilePattern, tileset));
	}
    }

    /**
     * List selection listener associated to the tile patterns list.
     * When the user selects a tile pattern, it becomes selected also in the tileset.
     */
    private class TilePatternListSelectionListener implements ListSelectionListener {

	/**
	 * This function is called when the selection is changed.
	 */
	public void valueChanged(ListSelectionEvent e) {

	    // get the rank of the tile pattern just selected in the list (0 to nbTilePatterns)
	    int selectedTilePatternRank = tilePatternsList.getSelectedIndex();

	    // select this tile pattern in the tileset
	    if (selectedTilePatternRank != -1) {
		tileset.setSelectedTilePatternId(tileset.rankToId(selectedTilePatternRank));
	    }
	    else {
		tileset.unselectTilePattern();
	    }
	}
    }

    /**
     * List model for the tile patterns list.
     */
    private class TilePatternsListModel extends AbstractListModel implements Observer {

	/**
	 * Returns the number of tile patterns.
	 */
	public int getSize() {
	    int size = 0;

	    if (tileset != null) {
		size = tileset.getNbTilePatterns();
	    }

	    return size;
	}
	
	/**
	 * Returns the a component with a 16*16 image of the specified tile pattern. 
	 */
	public Object getElementAt(int rank) {

	    return tileset.getTilePattern(tileset.rankToId(rank));
	}

	/**
	 * This function is called when the tileset changes.
	 * @param o the tileset
	 * @param params information about what has changed:
	 *   - a TilePattern: indicates that this tile pattern has just been created
	 *   - an Integer: indicates that the tile pattern with this id has just been removed
	 *   - null: other cases
	 */
	public void update(Observable o, Object params) {

	    // reload the icons if a tile was added or removed
	    if (params instanceof Integer || params instanceof TilePattern) {
		loadIcons();
	    }

	    // update the enabled state of the buttons
	    int tilesetSelectedPatternId = tileset.getSelectedTilePatternId();
	    int listSelectedRank = tilePatternsList.getSelectedIndex();

	    if (tileset.getSelectedTilePattern() != null) {
		// an existing tile pattern is selected
		// make this tile pattern selected in the list
		int listRank = tileset.idToRank(tilesetSelectedPatternId);
		if (listRank != listSelectedRank) {
		    tilePatternsList.setSelectedIndex(listRank);
		    tilePatternsList.ensureIndexIsVisible(listRank);
		}
	    }
	    else if (tileset.getSelectedTilePattern() == null) {
		// no tile pattern is selected anymore
		tilePatternsList.removeSelectionInterval(listSelectedRank, listSelectedRank);
	    }
	    // redraw the table
	    fireContentsChanged(this, 0, tileset.getNbTilePatterns() - 1);

	    // notify the tile pattern view
	    tilePatternView.setCurrentTilePattern(tileset.getSelectedTilePattern());
	}
    }

    /**
     * This class defines how the list elements are displayed.
     */
    private class TilePatternListRenderer implements ListCellRenderer {

	/**
	 * Returns a component representing a tile.
	 */
	public Component getListCellRendererComponent(JList list, Object value, int rank,
						      boolean isSelected, boolean cellHasFocus) {
	    if (rank >= tilePatternIcons.size()) {
		// the icon doesn't exist yet
		loadIcons();
	    }

	    return tilePatternIcons.get(rank);
	}
    }
}
