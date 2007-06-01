package tileset_editor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;

/**
 * Graphical component to select the obstacle properties of a tile:
 * - no obstacle
 * - obstacle
 * - obstacle top right
 * - obstacle top left
 * - obstacle bottom left
 * - obstacle bottom right
 */
public class ObstacleView extends JComboBox implements Observer, ActionListener {

    /**
     * The tile observed.
     */
    private Tile tile;

    /**
     * Icons displayed in the combo box
     */
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
    public ObstacleView() {
	super(obstacleIcons);
	addActionListener(this);
	setCurrentTile(null);
    }

    /**
     * Sets the current tile observed.
     */
    public void setCurrentTile(Tile tile) {
	if (this.tile != null) {
	    this.tile.deleteObserver(this);
	}

	this.tile = tile;
	
	if (tile != null) {
	    tile.addObserver(this);
	    setEnabled(true);
	    update(tile, null);
	}
	else {
	    setEnabled(false);
	}
    }

    /**
     * This method is called when the tile is changed.
     * The selection is then updated.
     */
    public void update(Observable o, Object params) {
	setSelectedIndex(tile.getObstacle());
    }
    
    /**
     * This method is called when the selection the two combo box is changed.
     * The tile is then updated.
     */
    public void actionPerformed(ActionEvent e) {
	tile.setObstacle(getSelectedIndex());
    }
}
