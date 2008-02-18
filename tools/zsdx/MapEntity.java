package zsdx;

import java.awt.*;
import java.util.*;

/**
 * Represents an entity placed on the map with the map editor,
 * and how the entity is placed on the map: its position and its layer.
 */
public abstract class MapEntity extends Observable {

    /**
     * The map.
     */
    protected Map map;
    
    /**
     * Position of the entity in the map.
     */
    protected Rectangle positionInMap;

    /**
     * Origin point of the entity (relative to the top-left corner of its rectangle).
     */
    protected Point hotSpot;

    /**
     * Layer of the entity on the map.
     */
    protected int layer;

    /**
     * Direction of the entity (0 to 3).
     * Not used by all kinds of entities.
     */
    protected int direction;

    /**
     * String identifying the entity.
     * Not used by all kinds of entities.
     */
    protected String name;

    /**
     * Color to display instead of the transparent pixels of the image.
     */
    protected static final Color bgColor = new Color(255, 0, 255);

    // Constants to identify the layer

    public static final int LAYER_LOW = 0;
    public static final int LAYER_INTERMEDIATE = 1;
    public static final int LAYER_HIGH = 2;
    public static final int LAYER_NB = 3;

    // Constants to identify the obstacles

    public static final int OBSTACLE_NONE = 0;
    public static final int OBSTACLE = 1;
    public static final int OBSTACLE_TOP_RIGHT = 2;
    public static final int OBSTACLE_TOP_LEFT = 3;
    public static final int OBSTACLE_BOTTOM_LEFT = 4;
    public static final int OBSTACLE_BOTTOM_RIGHT = 5;

    // types of entities
    public static final int ENTITY_TILE = 0;
    public static final int ENTITY_ENTRANCE = 1;
    public static final int ENTITY_EXIT = 2;

    // names of the entity types
    public static final String[] entityTypeNames = {
	"tile", "entrance", "exit"
    };

    // concrete subclasses of MapEntity
    public static final Class<?>[] entityClasses = {
	TileOnMap.class,          // ENTITY_TILE
	MapEntrance.class,        // ENTITY_ENTRANCE
	MapExit.class             // ENTITY_EXIT
    };

    /**
     * Empty constructor.
     */
    protected MapEntity() {
	this(null, LAYER_LOW, 0, 0, 0, 0, false);
    }
    
    /**
     * Creates an entity.
     * If the entity is identifiable, a default name
     * is given to the entity (this name is computed such that it is
     * different from the other entities of the same kind on the map).
     * @param map the map
     * @param layer layer of the entity
     * @param x x position of the entity on the map
     * @param y y position of the entity on the map
     * @param width width of the entity
     * @param height height of the entity
     * @param computeDefaultName true to give a default name to the entity
     * (this name is computed such that it is different from the other
     * entities of the same kind on the map) or false if the entity is
     * not identifiable or if you want to set its name later 
     */
    protected MapEntity(Map map, int layer, int x, int y,
	    int width, int height, boolean computeDefaultName) {
	this.map = map;
	this.layer = layer;
	this.positionInMap = new Rectangle(x, y, width, height);
	this.hotSpot = new Point();

	if (hasName() && computeDefaultName) {
	    computeDefaultName();
	}
    }

    /**
     * Creates a map entity from a string description as returned by toString().
     * @param description a string describing the entity, as returned by toString()
     * @param map the map (needed for some types of entities)
     * @throws ZSDXException if the string is incorrect
     */
    public static MapEntity createFromString(String description, Map map) throws ZSDXException {

	StringTokenizer tokenizer = new StringTokenizer(description);

	int entityType = Integer.parseInt(tokenizer.nextToken());
	int layer = Integer.parseInt(tokenizer.nextToken());
	int x = Integer.parseInt(tokenizer.nextToken());
	int y = Integer.parseInt(tokenizer.nextToken());

	MapEntity entity;

	switch (entityType) {
	    
	case ENTITY_TILE:
	    entity = new TileOnMap(tokenizer, map);
	    break;
	    
	case ENTITY_ENTRANCE:
	    entity = new MapEntrance(tokenizer, map);
	    break;

	case ENTITY_EXIT:
	    entity = new MapExit(tokenizer, map);
	    break;

	default:
	    throw new ZSDXException("Unknown entity type '" + entityType + "'");
	}

	entity.setLayer(layer);
	entity.setPositionInMap(x, y);

	if (entity.hasName()) {
	    entity.setName(tokenizer.nextToken());
	}

	if (entity.hasDirection()) {
	    entity.setDirection(Integer.parseInt(tokenizer.nextToken()));
	}

	return entity;
    }

    /**
     * Sets the hotspot of the entity.
     * @param x x coordinate
     * @param y y coordinate
     */
    protected void setHotSpot(int x, int y) {
	
	hotSpot.x = x;
	hotSpot.y = y;
    }

    /**
     * Returns the entity's obstacle property (default is OBSTACLE_NONE).
     * @return OBSTACLE_NONE, OBSTACLE, OBSTACLE_TOP_RIGHT,
     * OBSTACLE_TOP_LEFT, OBSTACLE_BOTTOM_LEFT or OBSTACLE_BOTTOM_RIGHT
     */
    public int getObstacle() {
	return OBSTACLE_NONE;
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

	positionInMap.x = position.x;
	positionInMap.y = position.y;
	
	setSize(position.width, position.height);
    }
    
    /**
     * Changes the position of the entity on the map, by specifying two points
     * (the top-left corner and the bottom-right corner).
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

	positionInMap.x = x;
	positionInMap.y = y;
	
	setSize(width, height);
    }
    
    /**
     * Changes the position of the entity on the map, by specifying the coordinates of its hotspot.
     * The size of the entity is not changed.
     * @param x x coordinate of the hotspot
     * @param y y coordinate of the hotspot
     */
    public void setPositionInMap(int x, int y) {

	positionInMap.x = x - hotSpot.x;
	positionInMap.y = y - hotSpot.y;
	
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
     * Returns whether the entity has a direction property.
     * The subclasses which represent entities with a direction
     * should return true.
     * @return true if the entity has a direction, false otherwise
     */
    public boolean hasDirection() {
	return getNbDirections() > 1;
    }

    /**
     * Returns the number of possible directions of the entity.
     * The entity should have a direction (i.e. hasDirection() returns true).
     * @return the number of possible directions of the entity (or 0
     * if the entity has not a direction property)
     */
    public abstract int getNbDirections();

    /**
     * Returns the direction of the entity.
     * The entity should have a direction (i.e. hasDirection() returns true).
     * @return the entity's direction
     */
    public int getDirection() {
	return direction;
    }

    /**
     * Changes the direction of the entity.
     * The entity must have a direction (i.e. hasDirection() returns true).
     * By default, an entity has no direction and this method throws an exception.
     * @param direction the entity's direction
     * @throws UnsupportedOperationException if the entity has no direction
     * @throws IllegalArgumentException if the direction is invalid
     */
    public void setDirection(int direction) throws UnsupportedOperationException, IllegalArgumentException {

	if (!hasDirection()) {
	    throw new UnsupportedOperationException("This entity has no direction");
	}

	if (direction < 0 || direction >= getNbDirections()) {
	    throw new IllegalArgumentException("The direction '" + direction
		    	+ "' is incorrect: the number of possible directions is " + getNbDirections());
	}

	this.direction = direction;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether the entity has a name.
     * The subclasses which represent identifiable entities should return true.
     * @return true if the entity has a name, false otherwise
     */
    public abstract boolean hasName();

    /**
     * Returns the name of the entity.
     * The name of an entity permits to identify it.
     * @return the entity's name, or null if this entity is not identifiable
     */
    public String getName() {
	return name;
    }

    /**
     * Changes the name of the entity.
     * The entity must be identifiable (i.e. hasName() returns true).
     * By default, an entity is not identifiable and this method throws an exception.
     * @param name the new entity's name
     * @throws UnsupportedOperationException if the entity is not identifiable
     * @throws MapException if this name is already used by another entity of the same type
     */
    public void setName(String name) throws UnsupportedOperationException, MapException {

	if (!hasName()) {
	    throw new UnsupportedOperationException("This entity is not identifiable");
	}
	
	MapEntity other = map.getEntityWithName(getType(), name);
	if (other != null && other != this) {
	    throw new MapException("This name is already used by another entity of this map");
	}

	this.name = name;
	setChanged();
	notifyObservers();
    }

    /**
     * Sets a default name to the entity.
     * @param map the map
     */
    private void computeDefaultName() {
	
	int entityType = getType();	
	String prefix = entityTypeNames[entityType] + ' ';
	int i = 1;
	
	// compute an unused name
	while (map.getEntityWithName(entityType, prefix + i) != null) {
	    i++;	    
	}
	
	try {
	    setName(entityTypeNames[entityType] + i);
	}
	catch (MapException ex) {
	    // should not happen if the while above works
	    System.err.println("Unexcepted error: " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Returns the minimum width of the entity (for a resizable entity).
     * When the entity is resized, its new width must be a multiple of this minimum size.
     * @return the minimum width of the entity
     */
    public int getUnitWidth() {
	return 8;
    }

    /**
     * Returns the minimum height of the entity (for a resizable entity).
     * When the entity is resized, its new height is a multiple of this minimum height.
     * @return the minimum height of the entity
     */
    public int getUnitHeight() {
	return 8;
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
     * Returns the x coordinate of the entity's hotspot on the map.
     * @return the x coordinate of the entity's hotspot on the map
     */
    public int getX() {
	return positionInMap.x + hotSpot.x;
    }

    /**
     * Returns the y coordinate of the entity's hotspot on the map.
     * @return the y coordinate of the entity's hotspot on the map
     */
    public int getY() {
	return positionInMap.y + hotSpot.y;
    }
    
    /**
     * Returns the width of the entity
     * @return the entity's width
     */
    public int getWidth() {
	return positionInMap.width;
    }

    /**
     * Returns the height of the entity
     * @return the entity's height
     */
    public int getHeight() {
	return positionInMap.height;
    }
    
    /**
     * Returns the size of the entity
     * @return the entity's size
     */
    public Dimension getSize() {
	return new Dimension(getWidth(), getHeight());
    }

    /**
     * Draws the entity on the map view.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public abstract void paint(Graphics g, double zoom, boolean showTransparency);

    /**
     * Returns an integer identifying the kind of entity: ENTITY_TILE, ENTITY_ENTRANCE...
     * @return the type of entity
     */
    public abstract int getType();

    /**
     * Returns a string describing this entity: the entity type, the layer, the coordinates,
     * the id (if any) and the direction (if any).
     * Subclasses should redefine this method to add their own information (if any).
     * @return a string representation of the entity.
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();
	buff.append(getType());
	buff.append('\t');
	buff.append(layer);
	buff.append('\t');
	buff.append(getX());
	buff.append('\t');
	buff.append(getY());

	if (hasName()) {
	    buff.append('\t');
	    buff.append(getName());
	}
	
	if (hasDirection()) {
	    buff.append('\t');
	    buff.append(getDirection());
	}

	return buff.toString();
    }
}
