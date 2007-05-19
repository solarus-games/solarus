package tileset_editor;

import java.io.*;

/**
 * Represents a tile for the tileset editor.
 */
public class Tile implements Serializable {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 1L;

    // Constants to identify the obstacles

    public static final int NO_OSTACLE = 0;
    public static final int OBSTACLE = 1;
    public static final int OBSTACLE_UP_RIGHT = 2;
    public static final int OBSTACLE_UP_LEFT = 3;
    public static final int OBSTACLE_DOWN_LEFT = 4;
    public static final int OBSTACLE_DOWN_RIGHT = 5;

    // Constants to identify the animation type

    public static final int NO_ANIMATION = 0;
    public static final int ANIMATION_SEQUENCE_012 = 1;
    public static final int ANIMATION_SEQUENCE_0121 = 2;

    public static final int ANIMATION_HORIZONTAL_SEPARATION = 0;
    public static final int ANIMATION_VERTICAL_SEPARATION = 1;

    /**
     * Coordinates and dimensions of the tile.
     */
    private Rectangle positionInTileset;

    /**
     * Type of obstacle.
     */
    private int obstacle;

    /**
     * Animation sequence.
     */
    private int animationSequence;

    /**
     * Type of separation of the 3 animation frames in the tileset.
     */
    private int animationSeparation;

    /**
     * Constructor.
     */
    public Tile(Rectangle positionInTileset, int obstacle,
		int animationSequence, int animationSeparation) {
	this.positionInTileset = positionInTileset;
	this.obstacle = obstacle;
	this.animationSequence = animationSequence;
	this.animationSeparation = animationSeparation;
    }

}
