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
package zsdx.gui;

import zsdx.entities.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

/**
 * This component shows information about a tile pattern and lets the user edit it.
 */
public class TilePatternView extends JPanel implements Observer {

    /**
     * The tile pattern observed.
     */
    private TilePattern tilePattern;

    /**
     * The tileset.
     */
    private Tileset tileset;

    // the components
    private EnumerationIconChooser<Obstacle> obstacleView;
    private TilePatternAnimationView animationView;
    private EnumerationChooser<Layer> defaultLayerView;
    private JButton buttonDelete;

    /**
     * Constructor.
     */
    public TilePatternView() {
	super(new GridBagLayout());

	GridBagConstraints constraints = new GridBagConstraints();
	constraints.insets = new Insets(5, 5, 5, 5); // margins
	constraints.anchor = GridBagConstraints.LINE_START; // alignment of the components

	// obstacle
	constraints.gridx = 0;
	constraints.gridy = 0;
	add(new JLabel("Obstacle"), constraints);
	obstacleView = new EnumerationIconChooser<Obstacle>(Obstacle.class);

	obstacleView.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {

		    Obstacle obstacle = obstacleView.getValue();
		    if (tilePattern != null && obstacle != tilePattern.getObstacle()) {
			// the type of obstacle has changed
			try {
			    tilePattern.setObstacle(obstacle);
			}
			catch (TilesetException e) {
			    JOptionPane.showMessageDialog(null,
							  "Unable to set this type of obstacle: " + e.getMessage(),
							  "Error",
							  JOptionPane.ERROR_MESSAGE);
			    update(null, null);
			}
		    }
		}
	    });

	// animation
	constraints.gridy = 1;
	add(new JLabel("Animation"), constraints);
	animationView = new TilePatternAnimationView();

	// default layer
	constraints.gridy = 2;
	add(new JLabel("Default layer"), constraints);
	defaultLayerView = new EnumerationChooser<Layer>(Layer.class);

	defaultLayerView.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {

		    Layer defaultLayer = defaultLayerView.getValue();
		    if (tilePattern != null && defaultLayer != tilePattern.getDefaultLayer()) {
			// the default layer has changed
			tilePattern.setDefaultLayer(defaultLayer);
		    }
		}
	    });

	constraints.weightx = 1;
	constraints.gridx = 1;
	constraints.gridy = 0;
	add(obstacleView, constraints);

	constraints.gridy = 1;
	add(animationView, constraints);

	constraints.gridy = 2;
	add(defaultLayerView, constraints);

	// bouton delete
	buttonDelete = new JButton("Delete tile");
	buttonDelete.setEnabled(false);
	buttonDelete.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    tileset.removeTilePattern();
		}
	    });
	constraints.gridx = 0;
	constraints.gridy = 3;
	constraints.gridwidth = 2;
	add(buttonDelete, constraints);

	setTileset(null);
    }

    /**
     * Sets the tileset.
     * @param tileset the tileset
     */
    public void setTileset(Tileset tileset) {
	this.tileset = tileset;
	setCurrentTilePattern(null);
    }

    /**
     * This method is called when the tile pattern is changed.
     */
    public void update(Observable o, Object params) {

	if (tilePattern != null) {
	    buttonDelete.setEnabled(true);
	    obstacleView.setValue(tilePattern.getObstacle());
	    obstacleView.setEnabled(true);
	    defaultLayerView.setValue(tilePattern.getDefaultLayer());
	    defaultLayerView.setEnabled(true);
	}
	else {
	    buttonDelete.setEnabled(false);
	    obstacleView.setEnabled(false);
	    obstacleView.setValue(Obstacle.NONE);
	    defaultLayerView.setEnabled(false);
	}
	animationView.update(tilePattern);
    }

    /**
     * Changes the tile pattern shown.
     * @param tile the new current tile pattern (can be null)
     */
    public void setCurrentTilePattern(TilePattern tilePattern) {

	if (this.tilePattern != null) {
	    this.tilePattern.deleteObserver(this);
	}

	this.tilePattern = tilePattern;

	if (tilePattern != null) {
	    tilePattern.addObserver(this);
	}

	update(null, null);
    }
}
