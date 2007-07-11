package editors;

import javax.swing.*;

/**
 * This class provides icons representing the different kinds of obstacles.
 */
public class ObstacleIcons {

    /**
     * Icons representing each type of obstacle for a tile.
     */
    private static final ImageIcon[] obstacleIcons;

    /**
     * Strings representing each type of obstacle for a tile.
     */
    private static final String[] obstacleNames = {
	"No obstacle",
	"Obstacle",
	"Top Right",
	"Top Left",
	"Bottom Left",
	"Bottom Right"
    };

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
     * This class shouldn't be instancied.
     */
    private ObstacleIcons() {

    }
    
    /**
     * Returns the icon representing a type of obstacle.
     * @param obstacle type of obstacle: Tile.OBSTACLE_NONE, Tile.OBSTACLE, Tile.OBSTACLE_TOP_RIGHT,
     * Tile.OBSTACLE_TOP_LEFT, Tile.OBSTACLE_BOTTOM_LEFT, Tile.OBSTACLE_BOTTOM_RIGHT
     */
    public static ImageIcon getIcon(int obstacle) {
	return obstacleIcons[obstacle];
    }

    /**
     * Returns all the icons.
     * @return icon of each type of obstacle
     */
    public static ImageIcon[] getIcons() {
	return obstacleIcons;
    }

    /**
     * Returns the name of a type of obstacle.
     * @param obstacle type of obstacle: Tile.OBSTACLE_NONE, Tile.OBSTACLE, Tile.OBSTACLE_TOP_RIGHT,
     * Tile.OBSTACLE_TOP_LEFT, Tile.OBSTACLE_BOTTOM_LEFT, Tile.OBSTACLE_BOTTOM_RIGHT
     */
    public static String getName(int obstacle) {
	return obstacleNames[obstacle];
    }

    /**
     * Returns all the obstacle names.
     * @return name of each type of obstacle
     */
    public static String[] getNames() {
	return obstacleNames;
    }
}
