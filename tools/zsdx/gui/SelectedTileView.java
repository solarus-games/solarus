package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import zsdx.Tile;
import zsdx.Tileset;

/**
 * This component shows information about the current tile and lets the user edit it.
 */
public class SelectedTileView extends JPanel {

    // the tileset
    private Tileset tileset;

    // the components
    private ObstacleView obstacleView;
    private AnimationView animationView;
    private DefaultLayerView defaultLayerView;
    private JButton buttonDelete;

    /**
     * Constructor.
     */
    public SelectedTileView() {
	super(new GridBagLayout());

	GridBagConstraints constraints = new GridBagConstraints();
	constraints.insets = new Insets(5, 5, 5, 5); // margins
	constraints.anchor = GridBagConstraints.LINE_START; // alignment of the components

	// obstacle
	constraints.gridx = 0;
	constraints.gridy = 0;
	add(new JLabel("Obstacle"), constraints);
	obstacleView = new ObstacleView();

	// animation
	constraints.gridy = 1;
	add(new JLabel("Animation"), constraints);
	animationView = new AnimationView();

	// default layer
	constraints.gridy = 2;
	add(new JLabel("Default layer"), constraints);
	defaultLayerView = new DefaultLayerView();

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
		    tileset.removeTile();
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
	setCurrentTile(null);
    }

    /**
     * Changes the tile shown.
     * @param tile the new current tile (can be null)
     */
    public void setCurrentTile(Tile tile) {
	buttonDelete.setEnabled(tile != null);

	obstacleView.setCurrentTile(tile);
	animationView.setCurrentTile(tile);
	defaultLayerView.setCurrentTile(tile);
    }
}
