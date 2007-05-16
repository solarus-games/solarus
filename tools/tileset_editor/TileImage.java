package tileset_editor;

/**
 * Represents a tile image for the tileset editor.
 */
public class TileImage {

    // Constants to identify the obstacles

    public static final int NO_OSTACLE = 0;
    public static final int OBSTACLE = 1;
    public static final int OBSTACLE_UP_RIGHT = 2;
    public static final int OBSTACLE_UP_LEFT = 3;
    public static final int OBSTACLE_DOWN_LEFT = 4;
    public static final int OBSTACLE_DOWN_RIGHT = 5;

    // Constants to identify the animation type

    public static final int NO_ANIMATION = 0;
    public static final int ANIMATION_012 = 1;
    public static final int ANIMATION_0121 = 2;

    /**
     * Name of the tile.
     */
    private String name;

    /**
     * Type of obstacle.
     */
    private int obstacle;

    /**
     * Animation sequence.
     */
    private int animation;

    /**
     * Constructor.
     */
    public TileImage() {

    }

}
