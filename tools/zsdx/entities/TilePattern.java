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
package zsdx.entities;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import zsdx.*;

/**
 * Represents a tile pattern.
 * The pattern's dimensions must be multiples of 8 but this is not checked.
 */
public class TilePattern extends Observable {

    // Constants to identify the animation type

    public static final int ANIMATION_NONE = 0;
    public static final int ANIMATION_SEQUENCE_012 = 1;
    public static final int ANIMATION_SEQUENCE_0121 = 2;

    public static final int ANIMATION_SEPARATION_HORIZONTAL = 0;
    public static final int ANIMATION_SEPARATION_VERTICAL = 1;

    /**
     * Coordinates and dimensions of the tile pattern.
     */
    private Rectangle positionInTileset;

    /**
     * Subimages representing this tile pattern in the tileset for each zoom mode.
     */
    private BufferedImage[] images;

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
     * Default layer of the tiles having this pattern.
     * It can be changed in the map editor once the tile is placed on a map.
     * Thus, several tiles with the same pattern can be on different layers of the map.
     */
    private int defaultLayer;

    /**
     * Simple constructor with no animation.
     * @param positionInTileset position of the tile pattern in the tileset
     * @param defaultLayer default layer of the tile pattern
     * @param obstacle type of obstacle
     * @throws TilesetException if the tile size is incorrect
     */
    public TilePattern(Rectangle positionInTileset, int defaultLayer, int obstacle) throws TilesetException {
	this(positionInTileset, defaultLayer, obstacle, ANIMATION_NONE, 0);
    }

    /**
     * Constructor.
     * @param positionInTileset position of the tile pattern in the tileset
     * @param defaultLayer default layer of the tiles created with this pattern
     * @param obstacle type of obstacle
     * @param animationSequence type of animation: ANIMATION_NONE, ANIMATION_SEQUENCE_012
     * or ANIMATION_SEQUENCE_0121
     * @param animationSeparation separation of the 3 animation frames in the tileset image:
     * ANIMATION_SEPARATION_HORIZONTAL or ANIMATION_SEPARATION_VERTICAL
     * @throws TilesetException if the pattern size is incorrect
     */
    public TilePattern(Rectangle positionInTileset, int defaultLayer, int obstacle,
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

	this.images = new BufferedImage[4];
    }

    /**
     * Creates a tile pattern from a string.
     * @param description a string representing the tile pattern, as returned by toString()
     * @throws ZSDXException if there is a syntax error in the string
     */
    public TilePattern(String description) throws ZSDXException {

	try {
	    StringTokenizer tokenizer = new StringTokenizer(description);

	    int tilePatternType = Integer.parseInt(tokenizer.nextToken());
	    this.obstacle = Integer.parseInt(tokenizer.nextToken());

	    if (obstacle < 0) {
		obstacle += 8;
	    }

	    this.defaultLayer = Integer.parseInt(tokenizer.nextToken());
	    this.images = new BufferedImage[4];

	    if (tilePatternType == 0) {

		// simple tile pattern: "0 obstacle defaultLayer x y width height"
		int x = Integer.parseInt(tokenizer.nextToken());
		int y = Integer.parseInt(tokenizer.nextToken());
		int width = Integer.parseInt(tokenizer.nextToken());
		int height = Integer.parseInt(tokenizer.nextToken());

		this.positionInTileset = new Rectangle(x, y, width, height);
		this.animationSequence = ANIMATION_NONE;
	    }
	    else if (tilePatternType == 1) {

		// animated tile pattern: "1 obstacle defaultLayer animationSequence width height x1 y1 x2 y2 x3 y3"
		this.animationSequence = Integer.parseInt(tokenizer.nextToken());

		int width = Integer.parseInt(tokenizer.nextToken());
		int height = Integer.parseInt(tokenizer.nextToken());
		int x1 = Integer.parseInt(tokenizer.nextToken());
		int y1 = Integer.parseInt(tokenizer.nextToken());
		int x2 = Integer.parseInt(tokenizer.nextToken());

		this.positionInTileset = new Rectangle(x1, y1, width, height);

		if (x1 == x2) {
		    this.animationSeparation = ANIMATION_SEPARATION_VERTICAL;
		    positionInTileset.height *= 3;
		}
		else {
		    this.animationSeparation = ANIMATION_SEPARATION_HORIZONTAL;
		    positionInTileset.width *= 3;
		}
	    }
	    else {
		throw new ZSDXException("Unknown tile type '" + tilePatternType + "'");
	    }
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Returns a string describing this tile.
     * @return a string representation of the tile
     */
    public String toString() {

	StringBuffer description = new StringBuffer();

	int obstacleSaved = (obstacle < MapEntity.OBSTACLE_SHALLOW_WATER) ? obstacle : (obstacle - 8);

	if (animationSequence == ANIMATION_NONE) {
	    // simple tile pattern: "0 obstacle defaultLayer x y width height"

	    description.append('0');
	    description.append('\t');
	    description.append(obstacleSaved);
	    description.append('\t');
	    description.append(defaultLayer);
	    description.append('\t');
	    description.append(positionInTileset.x);
	    description.append('\t');
	    description.append(positionInTileset.y);
	    description.append('\t');
	    description.append(positionInTileset.width);
	    description.append('\t');
	    description.append(positionInTileset.height);
	}
	else {
	    // animated tile pattern: "1 obstacle defaultLayer animationSequence width height x1 y1 x2 y2 x3 y3"

	    description.append('1');
	    description.append('\t');
	    description.append(obstacleSaved);
	    description.append('\t');
	    description.append(defaultLayer);
	    description.append('\t');
	    description.append(animationSequence);
	    description.append('\t');

	    int width, height, x, y, dx, dy;
	    if (animationSeparation == ANIMATION_SEPARATION_HORIZONTAL) {
		width = positionInTileset.width / 3;
		height = positionInTileset.height;
		dx = width;
		dy = 0;
	    }
	    else {
		width = positionInTileset.width;
		height = positionInTileset.height / 3;
		dx = 0;
		dy = height;
	    }

	    x = positionInTileset.x;
	    y = positionInTileset.y;

	    description.append(width);
	    description.append('\t');
	    description.append(height);
	    description.append('\t');
	    description.append(x);
	    description.append('\t');
	    description.append(y);
	    description.append('\t');
	    description.append(x + dx);
	    description.append('\t');
	    description.append(y + dy);
	    description.append('\t');
	    description.append(x + 2 * dx);
	    description.append('\t');
	    description.append(y + 2 * dy);
	}

	return description.toString();
    }

    /**
     * Returns the rectangle of the tile pattern.
     * If the tile pattern is animated, a rectangle containing the 3 frames
     * is returned.
     * @return the pattern's rectangle
     */
    public Rectangle getPositionInTileset() {
	return positionInTileset;
    }

    /**
     * Returns the x coordinate of the tile pattern in the tileset image.
     * @return the x coordinate of the tile pattern in the tileset image
     */
    public int getX() {
	return positionInTileset.x;
    }

    /**
     * Returns the y coordinate of the tile pattern in the tileset image.
     * @return the y coordinate of the tile pattern in the tileset image
     */
    public int getY() {
	return positionInTileset.y;
    }

    /**
     * Returns the tile pattern's width.
     * If the tile pattern is animated, the value returned is the width
     * of one frame.
     * @return the tile pattern's width in pixels
     */
    public int getWidth() {

	int width = positionInTileset.width;
	
	if (isAnimated() && animationSeparation == ANIMATION_SEPARATION_HORIZONTAL) {
	    width = width / 3;
	}

	return width;
    }

    /**
     * Returns the tile pattern's height.
     * If the tile pattern is animated, the value returned is the height
     * of one frame.
     * @return the tile pattern's height in pixels
     */
    public int getHeight() {

	int height = positionInTileset.height;
	
	if (isAnimated() && animationSeparation == ANIMATION_SEPARATION_VERTICAL) {
	    height = height / 3;
	}

	return height;
    }

    /**
     * Returns the tile pattern's size.
     * @return the tile pattern's size in pixels
     */
    public Dimension getSize() {
	return new Dimension(getWidth(), getHeight());
    }
    
    /**
     * Returns the tile pattern's obstacle property.
     * @return the obstacle property
     */
    public int getObstacle() {
	return obstacle;
    }

    /**
     * Changes the tile pattern's obstacle property.
     * @param obstacle the obstacle propery
     * @throws TilesetException if the obstacle specified is diagonal
     * and the tile pattern is not square
     */
    public void setObstacle(int obstacle) throws TilesetException {

	// diagonal obstacle: check that the tile is square
	if (obstacle >= MapEntity.OBSTACLE_TOP_RIGHT && obstacle <= MapEntity.OBSTACLE_BOTTOM_RIGHT
	    && getWidth() != getHeight()) {
	    System.out.println("width = " + getWidth() + ", height = " + getHeight());
	    throw new TilesetException("Cannot make a diagonal obstacle on a non-square tile pattern");
	}

	this.obstacle = obstacle;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the default layer of the tiles having this pattern.
     * @return the default layer of the tile pattern:
     * TilePattern.LAYER_LOW (most of the tiles),
     * TilePattern.LAYER_INTERMEDIATE or TilePattern.LAYER_HIGH.
     */
    public int getDefaultLayer() {
	return defaultLayer;
    }

    /**
     * Changes the default layer the tiles having this pattern.
     * @param defaultLayer the default layer of the tile pattern: TilePattern.LAYER_LOW
     * (most of the tiles), TilePattern.LAYER_INTERMEDIATE or TilePattern.LAYER_HIGH.
     */
    public void setDefaultLayer(int defaultLayer) {
	if (defaultLayer != this.defaultLayer) {
	    this.defaultLayer = defaultLayer;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the tile pattern's animation sequence.
     * @return the tile pattern's animation sequence: TilePattern.NO_ANIMATION,
     * TilePattern.ANIMATION_012 or TilePattern.ANIMATION_0121
     */
    public int getAnimationSequence() {
	return animationSequence;
    }

    /**
     * Sets the tile pattern's animation sequence.
     * If the specified animation makes the tile animated, a valid separation is chosen
     * between TilePattern.ANIMATION_SEPARATION_HORIZONTAL and TilePattern.ANIMATION_SEPARATION_VERTICAL.
     * You can change it with setAnimationSeparation().
     * If both directions are invalid, an TilesetException is thrown.
     * @param animationSequence the tile's animation sequence: TilePattern.NO_ANIMATION,
     * TilePattern.ANIMATION_012 or TilePattern.ANIMATION_0121
     * @throws TilesetException if the tile cannot be divided in 3 frames
     */
    public void setAnimationSequence(int animationSequence) throws TilesetException {

	if (animationSequence != ANIMATION_NONE) {

	    // try to set the animation separation
	    int width = positionInTileset.width;
	    int height = positionInTileset.height;

	    // try to divide the tile pattern in the biggest direction
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
     * Returns whether or not the tile pattern is animated.
     * @return true if the tile pattern is animated, false otherwise
     */
    public boolean isAnimated() {
	return animationSequence != ANIMATION_NONE;
    }

    /**
     * Returns the type of separation of the 3 animation frames if the tile pattern is animated.
     * @return the type of separation of the 3 animation frames:
     * TilePattern.ANIMATION_HORIZONTAL_SEPARATION or TilePattern.ANIMATION_VERTICAL_SEPARATION
     */
    public int getAnimationSeparation() {
	return animationSeparation;
    }

    /**
     * Sets the type of separation of the 3 animation frames if the tile pattern is animated.
     * Nothing is done if the tile pattern is not animated.
     * @param animationSeparation the type of separation of the 3 animation frames:
     * TilePattern.ANIMATION_HORIZONTAL_SEPARATION and TilePattern.ANIMATION_VERTICAL_SEPARATION
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
	
	// check that the tile pattern can be separated in 3 frames
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
     * Compares this tile pattern to another one.
     * @return true if it the tile patterns have exactly the same properties
     */
    public boolean equals(Object other) {

	if (!(other instanceof TilePattern)) {
	    return false;
	}

	TilePattern tilePattern = (TilePattern) other;

	return obstacle == tilePattern.obstacle
	    && animationSequence == tilePattern.animationSequence
	    && animationSeparation == tilePattern.animationSeparation
	    && positionInTileset.equals(tilePattern.positionInTileset);
    }

    /**
     * Returns the image representing this tile pattern in its tileset with a zoom mode.
     * @param zoom the zoom
     * @param tileset the tileset
     */
    public BufferedImage getTileImage(Tileset tileset, double zoom) {

	int index;
	if (zoom == 0.25) {
	    index = 0;
	}
	else if (zoom == 0.5) {
	    index = 1;
	}
	else if (zoom == 1.0) {
	    index = 2;
	}
	else {
	    index = 3;
	}

	if (images[index] == null) {
	    int x = (int) Math.round(getX() * zoom);
	    int y = (int) Math.round(getY() * zoom);
	    int width = (int) Math.round(getWidth() * zoom);
	    int height = (int) Math.round(getHeight() * zoom);
	    images[index] = tileset.getScaledImage(index).getSubimage(x, y, width, height);
	}

	return images[index];
    }

    /**
     * Draws the tile pattern on a component.
     * @param g graphic context
     * @param tileset the tileset
     * @param x x coordinate of where the tile pattern has to be painted
     * @param y y coordinate of where the tile pattern has to be painted
     * @param zoom scale of the image (1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, Tileset tileset, int x, int y, double zoom, boolean showTransparency) {

	BufferedImage patternImage = getTileImage(tileset, zoom);
	int dx = (int) Math.round(x * zoom);
	int dy = (int) Math.round(y * zoom);

/*
	Color bgColor = showTransparency ? null : MapEntity.bgColor;
	g.drawImage(getTileImage(tileset, zoom), dx, dy, bgColor, this);
*/

        if (!showTransparency) {
	    g.setColor(MapEntity.bgColor);
	    g.fillRect(dx, dy, patternImage.getWidth(), patternImage.getHeight());
	}
	g.drawImage(patternImage, dx, dy, null);
    }
}
