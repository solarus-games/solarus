package tileset_editor;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.io.*;

/**
 * This component shows information about the current tile and lets the user edit it.
 * TODO: make a class ObstacleView (like AnimationView)
 */
public class SelectedTileView extends JPanel {

    // the components

    private ObstacleView obstacleView;
    private AnimationView animationView;

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
	add(new JLabel("Obstacle:"), constraints);
	obstacleView = new ObstacleView();

	// animation
	constraints.gridy = 1;
	add(new JLabel("Animation:"), constraints);
	animationView = new AnimationView();

	constraints.weightx = 1;
	constraints.gridx = 1;
	constraints.gridy = 0;
	add(obstacleView, constraints);

	constraints.gridy = 1;
	add(animationView, constraints);

	setCurrentTile(null);
    }

    /**
     * Changes the tile shown.
     * @param tile the new current tile (can be null)
     */
    public void setCurrentTile(Tile tile) {
	obstacleView.setCurrentTile(tile);
	animationView.setCurrentTile(tile);
    }
}
