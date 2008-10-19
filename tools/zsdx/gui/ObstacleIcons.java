package zsdx.gui;

import javax.swing.*;
import zsdx.entities.*;

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
	String path = "zsdx/images/";

	obstacleIcons = new ImageIcon[6];
	obstacleIcons[MapEntity.OBSTACLE_NONE] = new ImageIcon(path + "obstacle_none.png");
	obstacleIcons[MapEntity.OBSTACLE] = new ImageIcon(path + "obstacle.png");
	obstacleIcons[MapEntity.OBSTACLE_TOP_RIGHT] = new ImageIcon(path + "obstacle_top_right.png");
	obstacleIcons[MapEntity.OBSTACLE_TOP_LEFT] = new ImageIcon(path + "obstacle_top_left.png");
	obstacleIcons[MapEntity.OBSTACLE_BOTTOM_LEFT] = new ImageIcon(path + "obstacle_bottom_left.png");
	obstacleIcons[MapEntity.OBSTACLE_BOTTOM_RIGHT] = new ImageIcon(path + "obstacle_bottom_right.png");
    }

    /**
     * This class shouldn't be instancied.
     */
    private ObstacleIcons() {

    }
    
    /**
     * Returns the icon representing a type of obstacle.
     * @param obstacle type of obstacle: MapEntity.OBSTACLE_NONE, MapEntity.OBSTACLE, MapEntity.OBSTACLE_TOP_RIGHT,
     * MapEntity.OBSTACLE_TOP_LEFT, MapEntity.OBSTACLE_BOTTOM_LEFT, MapEntity.OBSTACLE_BOTTOM_RIGHT
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
     * @param obstacle type of obstacle: MapEntity.OBSTACLE_NONE, MapEntity.OBSTACLE, MapEntity.OBSTACLE_TOP_RIGHT,
     * MapEntity.OBSTACLE_TOP_LEFT, MapEntity.OBSTACLE_BOTTOM_LEFT, MapEntity.OBSTACLE_BOTTOM_RIGHT
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
