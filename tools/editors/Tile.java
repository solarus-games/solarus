package editors;

import java.awt.*;
import java.io.*;
import java.util.*;

/**
 * Represents a tile for the tileset editor.
 * The tile's dimensions must be multiples of 8 but this is not checked.
 */
public class Tile extends Observable {

    // Constants to identify the obstacles

    public static final int OBSTACLE_NONE = 0;
    public static final int OBSTACLE = 1;
    public static final int OBSTACLE_TOP_RIGHT = 2;
    public static final int OBSTACLE_TOP_LEFT = 3;
    public static final int OBSTACLE_BOTTOM_LEFT = 4;
    public static final int OBSTACLE_BOTTOM_RIGHT = 5;

    // Constants to identify the animation type

    public static final int ANIMATION_NONE = 0;
    public static final int ANIMATION_SEQUENCE_012 = 1;
    public static final int ANIMATION_SEQUENCE_0121 = 2;

    public static final int ANIMATION_SEPARATION_HORIZONTAL = 0;
    public static final int ANIMATION_SEPARATION_VERTICAL = 1;

    // Constants to identify the layer

    public static final int LAYER_LOW = 0;
    public static final int LAYER_INTERMEDIATE = 1;
    public static final int LAYER_HIGH = 2;
    public static final int LAYER_NB = 3;

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
     * Default layer of the tile.
     * It can be changed in the map editor once the tile is placed on a map.
     * So several instances of the same tile placed on a map can be on different layers.
     */
    private int defaultLayer;

    /**
     * Simple constructor with no animation.
     * @param positionInTileset position of the tile in the tileset
     * @param defaultLayer default layer of the the tile
     * @param obstacle type of obstacle
     * @throws TilesetException if the tile size is incorrect
     */
    public Tile(Rectangle positionInTileset, int defaultLayer, int obstacle) throws TilesetException {
	this(positionInTileset, defaultLayer, obstacle, ANIMATION_NONE, 0);
    }

    /**
     * Constructor.
     * @param positionInTileset position of the tile in the tileset
     * @param defaultLayer default layer of the the tile
     * @param obstacle type of obstacle
     * @param animationSequence type of animation: ANIMATION_NONE ANIMATION_SEQUENCE_012
     * or ANIMATION_SEQUENCE_0121
     * @param animationSeparation separation of the 3 animation frames in the tileset image:
     * ANIMATION_SEPARATION_HORIZONTAL or ANIMATION_SEPARATION_VERTICAL
     * @throws TilesetException if the tile size is incorrect
     */
    public Tile(Rectangle positionInTileset, int defaultLayer, int obstacle,
		int animationSequence, int animationSeparation) throws TilesetException {
	super();
	
	// check the width and the height
	if (positionInTileset.width <= 0 || positionInTileset.height <= 0 ||
	    positionInTileset.width % 8 != 0 || positionInTileset.height % 8 != 0) {
	    throw new TilesetException("The size of a tile must be positive and multiple of 8 pixels");
	}
	
	this.positionInTileset = positionInTileset;
	this.defaultLayer = defaultLayer;
	setObstacle(obstacle);
	setAnimationSequence(animationSequence);
	setAnimationSeparation(animationSeparation);
    }

    /**
     * Returns the tile's rectangle.
     * @return the tile's rectangle
     */
    public Rectangle getPositionInTileset() {
	return positionInTileset;
    }

    /**
     * Returns the tile's width.
     * @return the tile's width in pixels
     */
    public int getWidth() {
	return positionInTileset.width;
    }

    /**
     * Returns the tile's height.
     * @return the tile's height in pixels
     */
    public int getHeight() {
	return positionInTileset.height;
    }

    /**
     * Returns the tile's obstacle property.
     * @return Tile.OBSTACLE_NONE, Tile.OBSTACLE, Tile.OBSTACLE_TOP_RIGHT,
     * Tile.OBSTACLE_TOP_LEFT, Tile.OBSTACLE_BOTTOM_LEFT or Tile.OBSTACLE_BOTTOM_RIGHT
     */
    public int getObstacle() {
	return obstacle;
    }

    /**
     * Changes the tile's obstacle property.
     * @param obstacle Tile.OBSTACLE_NONE, Tile.OBSTACLE, Tile.OBSTACLE_TOP_RIGHT,
     * Tile.OBSTACLE_TOP_LEFT, Tile.OBSTACLE_BOTTOM_LEFT or Tile.OBSTACLE_BOTTOM_RIGHT
     * @throws TilesetException if the obstacle specified is diagonal and the tile is not square
     */
    public void setObstacle(int obstacle) throws TilesetException {

	// diagonal obstacle: check that the tile is square
	if (obstacle != OBSTACLE_NONE && obstacle != OBSTACLE
	    && positionInTileset.width != positionInTileset.height) {
	    throw new TilesetException("Cannot make a diagonal obstacle on a non-square tile");
	}

	this.obstacle = obstacle;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the default layer of the tile.
     * @return the default layer of the tile: Tile.LAYER_LOW (most of the tiles),
     * Tile.LAYER_INTERMEDIATE or Tile.LAYER_HIGH.
     */
    public int getDefaultLayer() {
	return defaultLayer;
    }

    /**
     * Changes the default layer of the tile.
     * @param defaultLayer the default layer of the tile: Tile.LAYER_LOW (most of the tiles),
     * Tile.LAYER_INTERMEDIATE or Tile.LAYER_HIGH.
     */
    public void setDefaultLayer(int defaultLayer) {
	if (defaultLayer != this.defaultLayer) {
	    this.defaultLayer = defaultLayer;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the tile's animation sequence.
     * @return the tile's animation sequence: Tile.NO_ANIMATION,
     * Tile.ANIMATION_012 or Tile.ANIMATION_0121
     */
    public int getAnimationSequence() {
	return animationSequence;
    }

    /**
     * Sets the tile's animation sequence.
     * If the specified animation makes the tile animated, a valid separation is chosen
     * between Tile.ANIMATION_SEPARATION_HORIZONTAL or Tile.ANIMATION_SEPARATION_VERTICAL.
     * You can change it with setAnimationSeparation.
     * If both directions are invalid, an TilesetException is thrown.
     * @param animationSequence the tile's animation sequence: Tile.NO_ANIMATION,
     * Tile.ANIMATION_012 or Tile.ANIMATION_0121
     * @throws TilesetException if the tile cannot be divided in 3 frames
     */
    public void setAnimationSequence(int animationSequence) throws TilesetException {

	if (animationSequence != ANIMATION_NONE) {

	    // try to set the animation separation
	    int width = positionInTileset.width;
	    int height = positionInTileset.height;

	    // try to divide the tile in the biggest direction
	    if (width >= height) {
		try {
		    trySetAnimationSeparation(ANIMATION_SEPARATION_HORIZONTAL);
		}
		catch (TilesetException e) {
		    trySetAnimationSeparation(ANIMATION_SEPARATION_VERTICAL);
		    // an exception is thrown if this doesn't work either
		}
	    }
	    else {
		try {
		    trySetAnimationSeparation(ANIMATION_SEPARATION_VERTICAL);
		}
		catch (TilesetException e) {
		    trySetAnimationSeparation(ANIMATION_SEPARATION_HORIZONTAL);
		}
	    }
	}

	this.animationSequence = animationSequence;

	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether or not the tile is animated.
     * @return true if the tile is animated, false otherwise
     */
    public boolean isAnimated() {
	return animationSequence != ANIMATION_NONE;
    }

    /**
     * Returns the type of separation of the 3 animation frames if the tile is animated.
     * @return the type of separation of the 3 animation frames:
     * Tile.ANIMATION_HORIZONTAL_SEPARATION or Tile.ANIMATION_VERTICAL_SEPARATION
     */
    public int getAnimationSeparation() {
	return animationSeparation;
    }

    /**
     * Sets the type of separation of the 3 animation frames if the tile is animated.
     * Nothing is done if the tile is not animated.
     * @param animationSeparation the type of separation of the 3 animation frames:
     * Tile.ANIMATION_HORIZONTAL_SEPARATION or Tile.ANIMATION_VERTICAL_SEPARATION
     * @throws TilesetException thrown if the separation is not valid, i.e.
     * if the size of the 3 frames formed are not multiple of 8.
     */
    public void setAnimationSeparation(int animationSeparation) throws TilesetException {
	if (!isAnimated()) {
	    return;
	}

	trySetAnimationSeparation(animationSeparation);

	setChanged();
	notifyObservers();
    }

    /**
     * Sets the type of separation of the 3 animation frames.
     * @param animationSeparation the type of separation of the 3 animation frames:
     * Tile.ANIMATION_HORIZONTAL_SEPARATION or Tile.ANIMATION_VERTICAL_SEPARATION
     * @throws TilesetException thrown if the separation is not valid, i.e.
     * if the size of the 3 frames formed are not multiple of 8.
     */
    private void trySetAnimationSeparation(int animationSeparation) throws TilesetException {
	
	// check whether the tile can be separated in 3 frames
	if (animationSeparation == ANIMATION_SEPARATION_HORIZONTAL
	    && positionInTileset.width % 24 != 0) {
	    throw new TilesetException("Cannot divide the tile in 3 frames : the size of each frame must be a multiple of 8 pixels");
	}
	else if (animationSeparation == ANIMATION_SEPARATION_VERTICAL
		 && positionInTileset.height % 24 != 0) {
	    throw new TilesetException("Cannot divide the tile in 3 frames : the size of each frame must be a multiple of 8 pixels");
	}

	this.animationSeparation = animationSeparation;
    }

    /**
     * Draws the tile on a component.
     * @param g graphic context
     * @param tileset the tileset
     * @param x x coordinate of where the tile has to be painted
     * @param y y coordinate of where the tile has to be painted
     * @param scale scale of the image (1: unchanged, 2: zoom of 200%)
     */
    public void paint(Graphics g, Tileset tileset, int x, int y, int scale) {
	
	Image tilesetImage = (scale == 2) ? tileset.getDoubleImage() : tileset.getImage();

	int width = positionInTileset.width * scale;
	int height = positionInTileset.height * scale;
	
	int sx1 = positionInTileset.x * scale;
	int sx2 = sx1 + width;
	int sy1 = positionInTileset.y * scale;
	int sy2 = sy1 + height;
	
	int dx1 = x * scale;
	int dx2 = dx1 + width;
	int dy1 = y * scale;
	int dy2 = dy1 + height;
			
	g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
    }

    /**
     * Compares this tile to another one.
     * @return true if it the tiles have exactly the same properties
     */
    public boolean equals(Object other) {

	Tile tile = (Tile) other;

	return obstacle == tile.obstacle
	    && animationSequence == tile.animationSequence
	    && animationSeparation == tile.animationSeparation
	    && positionInTileset.equals(tile.positionInTileset);
    }
}
