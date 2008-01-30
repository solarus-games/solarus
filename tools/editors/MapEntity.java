package editors;

import java.awt.*;
import java.io.*;
import java.util.*;

/**
 * Represents an entity placed on the map with the map editor,
 * and how the entity is placed on the map: its position and its layer.
 */
public abstract class MapEntity extends Observable {

    /**
     * Position of the entity in the map.
     */
    protected Rectangle positionInMap;

    /**
     * Layer of the entity on the map.
     */
    protected int layer;

    /**
     * Color to display instead of the transparent pixels of the image.
     */
    protected static final Color bgColor = new Color(128, 128, 255);

    // Constants to identify the layer

    public static final int LAYER_LOW = 0;
    public static final int LAYER_INTERMEDIATE = 1;
    public static final int LAYER_HIGH = 2;
    public static final int LAYER_NB = 3;

    /**
     * Empty constructor.
     */
    protected MapEntity() {
	this(LAYER_LOW, 0, 0, 0, 0);
    }

    /**
     * Constructor.
     * @param layer layer of the entity
     * @param x x position of the entity on the map
     * @param y y position of the entity on the map
     * @param width width of the entity
     * @param height height of the entity
     */
    protected MapEntity(int layer, int x, int y, int width, int height) {
	this.layer = layer;
	this.positionInMap = new Rectangle(x, y, width, height);
    }

    /**
     * Returns the position of the entity on the map.
     * @return the position of the entity on the map
     */
    public Rectangle getPositionInMap() {
	return positionInMap;
    }

    /**
     * Changes the position of the entity on the map, by specifying its rectangle.
     * @param position a rectangle
     * @throws MapException if the rectangle's size is zero
     */
    public void setPositionInMap(Rectangle position) throws MapException {

	setPositionInMap(position.x, position.y);
	setSize(position.width, position.height);
    }
    
    /**
     * Changes the position of the entity on the map, by specifying two points.
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     * @throws MapException if the entity is not resizable, or the rectangle width
     * or its height is zero
     */
    public void setPositionInMap(int x1, int y1, int x2, int y2) throws MapException {

	int x = Math.min(x1, x2);
	int width = Math.abs(x2 - x1);
	
	int y = Math.min(y1, y2);
	int height = Math.abs(y2 - y1);

	setPositionInMap(x, y);
	setSize(width, height);

	// notify
	setChanged();
	notifyObservers();
    }
    
    /**
     * Changes the position of the entity on the map, by specifying a point.
     * The size of the entity is not changed.
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     */
    public void setPositionInMap(int x, int y) {

	positionInMap.x = x;
	positionInMap.y = y;
	
	// notify
	setChanged();
	notifyObservers();
    }

    /**
     * Changes the size of the entity on the map.
     * The location of the entity is not changed.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     * @throws MapException if the entity is not resizable, or the size specified is zero
     */
    public void setSize(int width, int height) throws MapException {

	if (!isResizable()) {
	    throw new MapException("This entity is not resizable");
	}

	if (width == 0 || height == 0) {
	    throw new MapException("The entity's size is zero"); 
	}

	positionInMap.width = width;
	positionInMap.height = height;

	// notify
	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether or not the entity is resizable.
     * By default, an entity is not resizable. The subclasses which represent
     * resizable entities should redefine this method and return true.
     * @return whether or not the entity is resizable
     */
    public boolean isResizable() {
	return false;
    }

    /**
     * Changes the position of the entity.
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public void move(int dx, int dy) {
	
	positionInMap.x += dx;
	positionInMap.y += dy;

	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether or not a point is in the entity's rectangle.
     * @param x x of the point
     * @param y y of the point
     * @return true if the point is in the entity's rectangle, false otherwise
     */
    public boolean containsPoint(int x, int y) {
	return positionInMap.contains(x, y);
    }

    /**
     * Returns the layer of the entity.
     * @return the layer of the entity: MapEntity.LAYER_LOW (most of the tiles),
     * MapEntity.LAYER_INTERMEDIATE or MapEntity.LAYER_HIGH.
     */
    public int getLayer() {
	return layer;
    }

    /**
     * Changes the layer of the tile.
     * @param layer the layer of the tile: MapEntity.LAYER_LOW (most of the tiles),
     * MapEntity.LAYER_INTERMEDIATE or MapEntity.LAYER_HIGH.
     */
    public void setLayer(int layer) {
	if (layer != this.layer) {
	    this.layer = layer;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the x coordinate of the entity on the map.
     * @return the x coordinate of the entity on the map
     */
    public int getX() {
	return positionInMap.x;
    }

    /**
     * Returns the y coordinate of the entity on the map.
     * @return the y coordinate of the entity on the map
     */
    public int getY() {
	return positionInMap.y;
    }

    /**
     * Draws the entity on the map view.
     * @param g graphic context
     * @param scale scale of the image (1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public abstract void paint(Graphics g, int scale, boolean showTransparency);
}
