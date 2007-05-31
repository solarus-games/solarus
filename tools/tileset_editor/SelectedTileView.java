package tileset_editor;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.io.*;

/**
 * This component shows information about the current tile and lets the user edit it.
 * TODO: make a class ObstacleView (like AnimationView)
 */
public class SelectedTileView extends JPanel implements Observer {

    /**
     * The tile shown.
     */
    private Tile tile;

    // the components

    private JComboBox obstacleView;
    private AnimationView animationView;

    private static ImageIcon[] obstacleIcons;

    // load the icons
    static {
	String path = "tileset_editor/images/";

	obstacleIcons = new ImageIcon[6];
	obstacleIcons[Tile.OBSTACLE_NONE] = new ImageIcon(path + "obstacle_none.png");
	obstacleIcons[Tile.OBSTACLE] = new ImageIcon(path + "obstacle.png");
	obstacleIcons[Tile.OBSTACLE_TOP_RIGHT] = new ImageIcon(path + "obstacle_top_right.png");
	obstacleIcons[Tile.OBSTACLE_TOP_LEFT] = new ImageIcon(path + "obstacle_top_left.png");
	obstacleIcons[Tile.OBSTACLE_BOTTOM_LEFT] = new ImageIcon(path + "obstacle_bottom_left.png");
	obstacleIcons[Tile.OBSTACLE_BOTTOM_RIGHT] = new ImageIcon(path + "obstacle_bottom_right.png");
    }
    

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
	obstacleView = new JComboBox(obstacleIcons);

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
	if (this.tile != null) {
	    this.tile.deleteObserver(this);
	}

	this.tile = tile;
	
	if (tile != null) {
	    tile.addObserver(this);
	    obstacleView.setEnabled(true);
	    update(tile, null);
	}
	else {
	    obstacleView.setEnabled(false);
	}

	animationView.setCurrentTile(tile);
    }

    /**
     * This function is called when the tile changes.
     */
    public void update(Observable o, Object params) {
	obstacleView.setSelectedIndex(tile.getObstacle());
    }
}
