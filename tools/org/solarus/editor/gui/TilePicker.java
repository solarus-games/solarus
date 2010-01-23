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

import javax.swing.*;
import java.util.*;
import org.solarus.editor.Map;

/**
 * This component shows the tileset and allows the user to pick a tile
 * in order to put it into the map.
 */
public class TilePicker extends JScrollPane implements Observer {

    /**
     * The current map.
     */
    private Map map;

    /**
     * The component in the scroll pane.
     */
    private TilesetImageView tilesetImageView;

    /**
     * Constructor.
     */
    public TilePicker() {
	super();

	tilesetImageView = new TilesetImageView(false);
	setViewportView(tilesetImageView);
    }

    /**
     * Sets the observed map.
     * @param map the current map, or null if no map is loaded
     */
    public void setMap(Map map) {
	if (this.map != null) {
	    this.map.deleteObserver(this);
	}

	this.map = map;
	
	if (map != null) {
	    map.addObserver(this);
	}

	update(map, null);
    }
    
    /**
     * This function is called when the map changes.
     */
    public void update(Observable o, Object obj) {
	if (map != null) {
	    tilesetImageView.setTileset(map.getTileset());
	}
	else {
	    tilesetImageView.setTileset(null);
	}
    }
}
