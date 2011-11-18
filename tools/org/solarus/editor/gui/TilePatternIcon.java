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
import javax.swing.*;
import java.util.*;
import org.solarus.editor.entities.*;

/**
 * This component shows a tile pattern's icon.
 */
public class TilePatternIcon extends JComponent implements Observer {

    private static final Color bgColorSelected = new Color(128, 128, 255);
    private static final Color bgColorUnselected = Color.WHITE;

    /**
     * The tile pattern.
     */
    private TilePattern tilePattern;

    /**
     * The tileset, needed to get its image.
     */
    private Tileset tileset;

    /**
     * Coordinates of the tile pattern as displayed by the icon.
     */
    private int dx1, dx2, dy1, dy2;

    /**
     * Coordinates of the tile pattern's icon in the tileset double image.
     */
    private int sx1, sx2, sy1, sy2;

    /**
     * Constructor.
     */
    public TilePatternIcon(TilePattern tilePattern, Tileset tileset) {
	this.tilePattern = tilePattern;
	this.tileset = tileset;
	tilePattern.addObserver(this);
	tileset.addObserver(this);

	update(tilePattern, null);
	update(tileset, null);
    }

    /**
     * Returns the preferred size: 48*48.
     * @return 48*48
     */
    public Dimension getPreferredSize() {
	return new Dimension(48, 48);
    }

    /**
     * Draws the tile pattern's image.
     * @param g graphic context
     */
    public void paint(Graphics g) {
	Image tilesetImage = tileset.getDoubleImage();

	if (tilesetImage != null) {
	    Color bgColor = (tileset.getSelectedTilePattern() == tilePattern) ? bgColorSelected : bgColorUnselected;
	    g.setColor(bgColor);
	    g.fillRect(0, 0, 47, 47);
	    g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, null);
	}
    }

    /**
     * This function is called when the tile is changed.
     */
    public void update(Observable o, Object params) {

	if (o instanceof TilePattern) { // the tile pattern has changed

	    Rectangle positionInTileset = tilePattern.getPositionInTileset();
	    int width = positionInTileset.width;
	    int height = positionInTileset.height;

	    // if the tile pattern is animated, let's display only the first frame
	    if (tilePattern.isMultiFrame()) {
		if (tilePattern.getAnimationSeparation() == TilePattern.AnimationSeparation.HORIZONTAL) {
		    width = width / 3;
		}
		else {
		    height = height / 3;
		}
	    }

	    // coordinates of the tile pattern in the tileset
	    sx1 = positionInTileset.x * 2;
	    sx2 = sx1 + width * 2;
	    sy1 = positionInTileset.y * 2;
	    sy2 = sy1 + height * 2;

	    // coordinates of the tile pattern in the icon

	    // check the tile pattern dimensions (we need to draw the tile 
	    // into a 16*16 square)
	    if (width > 16 || height > 16) {

		// the image will be rescaled to fit in the 16*16 square
		// but we need to keep the same ratio height-width
		double ratio = ((double) height) / width;
		if (height > width) {
		    height = 16;
		    width = (int) (height / ratio);
		}
		else {
		    width = 16;
		    height = (int) (width * ratio);
		}
	    }

	    // now the dimensions fit in the 32*32 square
	    dx1 = 24 - width;
	    dx2 = 23 + width;
	    dy1 = 24 - height;
	    dy2 = 23 + height;
	}

	repaint();
    }
}
