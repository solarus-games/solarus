package zsdx;

import java.awt.*;
import java.awt.image.*;
import java.lang.reflect.*;
import java.util.*;

/**
 * Represents an entity placed on the map with the map editor,
 * and how the entity is placed on the map: its position and its layer.
 * 
 * To create a new kind of entity, you should make the following steps:
 * - Add your class in the MapEntity.entityClasses array.
 * - Create a field in your class to declare the entity name:
 *     public static final String entityTypeName.
 * - Create two constructors with the following signatures:
 *     public YourEntity(Map map, int x, int y):
 *         creates a new entity with some default properties
 *     public YourEntity(Map map, String tokenizer) throws ZSDXException:
 *         creates an existing entity from a string
 * - Create the toString() method: public String toString()
 *     to export the entity to a string
 * - Define the getType() method: public int getType()
 *     which returns the entity type number.
 * - Redefine the getObstacle() method: public int getObstacle()
 *     if your entity is an obstacle.
 * - Redefine the methods getNbDirections(), hasName(), isResizable()
 *     to indicate how the graphical components will be organized.
 *     - public int getNbDirections(): returns the number of possible
 *         directions of this kind of entity (can be zero)
 *     - public boolean hasName(): indicates whether the entity has a 
 *         name field (i.e. if it is identifiable)
 *     - public boolean isResizable(): indicates whether the entity can
 *         be resized. If the entity is resizable, you should also redefine
 *         the getUnitSize() method.
 * - Redefine the getOrigin() method: public Point getOrigin()
 *     if the origin is not the top-left corner of the entity.
 * - Redefine the isValid() method: public boolean isValid()
 *     to check the validity of the fields (don't forget to call super.isValid()).    
 * - TODO paint() method
 * - Add the specific fields of your type of entity and the corresponding
 *     get and set methods.
 * - Create a subclass of EditEntityComponent and declare it in
 *     EditEntityComponent.editEntityComponentClasses.
 * - Create a subclass of ActionEditEntity
 */
public abstract class MapEntity extends Observable implements ImageObserver {

    /**
     * The map.
     */
    protected Map map;
    
    /**
     * Position of the entity in the map.
     */
    protected Rectangle positionInMap;
    
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
    
    /**
     * Coordinates of the origin point of an entity by default.
     */
    private static final Point defaultOrigin = new Point(0, 0);

    /**
     * Default minimum size of a resizable entity.
     */
    private static final Dimension defaultUnitSize = new Dimension(8, 8);
    
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
    public static final int ENTITY_PICKABLE_ITEM = 3;
    public static final int ENTITY_TRANSPORTABLE_ITEM = 4;
    public static final int ENTITY_NB_TYPES = 5;

    // concrete subclasses of MapEntity
    public static final Class<?>[] entityClasses = {
	TileOnMap.class,          // ENTITY_TILE
	MapEntrance.class,        // ENTITY_ENTRANCE
	MapExit.class,            // ENTITY_EXIT
	PickableItem.class,       // ENTITY_PICKABLE_ITEM
	TransportableItem.class   // ENTITY_TRANSPORTABLE_ITEM
    };

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

	if (hasName() && computeDefaultName) {
	    computeDefaultName();
	}
    }

    /**
     * Parses the common information of the entity, i.e. the layer, the position,
     * the size (if the entity is resizable), the name (if the entity is identifiable)
     * and the direction (if the entity have a direction property).
     * This constructor should be called by the subclasses before parsing the specific part of the line.
     * @param map the map
     * @param tokenizer the string tokenizer parsing the entity line (the entity type has already been parsed)
     * @throws ZSDXException if the line contains an error
     */
    protected MapEntity(Map map, StringTokenizer tokenizer) throws ZSDXException {
	this(map, LAYER_LOW, 0, 0, 0, 0, false);

	try {
	    int layer = Integer.parseInt(tokenizer.nextToken());
	    int x = Integer.parseInt(tokenizer.nextToken());
	    int y = Integer.parseInt(tokenizer.nextToken());

	    setLayer(layer);
	    setPositionInMap(x, y);

	    if (isResizable()) {
		int width = Integer.parseInt(tokenizer.nextToken());
		int height = Integer.parseInt(tokenizer.nextToken());
		setSize(width, height);
	    }

	    if (hasName()) {
		setName(tokenizer.nextToken());
	    }

	    if (hasDirection()) {
		setDirection(Integer.parseInt(tokenizer.nextToken()));
	    }
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("Value expected");
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected: " + ex.getMessage());
	}
    }

    /**
     * Creates a map entity from a string description as returned by toString().
     * @param map the map (needed for some types of entities)
     * @param description a string describing the entity, as returned by toString()
     * @throws ZSDXException if the string is incorrect
     */
    public static MapEntity createFromString(Map map, String description) throws ZSDXException {

	StringTokenizer tokenizer = new StringTokenizer(description, "\t");
	int entityType = Integer.parseInt(tokenizer.nextToken());
	
	if (entityType < 0 || entityType >= ENTITY_NB_TYPES) {
	    throw new MapException("Unknown entity type: " + entityType);
	}
	
	MapEntity entity = null;
	
	Class<?> entityClass = entityClasses[entityType];
	Constructor<?> constructor;
	
	try {
	    constructor = entityClass.getConstructor(Map.class, StringTokenizer.class);
	    entity = (MapEntity) constructor.newInstance(map, tokenizer);
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
	catch (InstantiationException ex) {
	    System.err.println("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
	catch (InvocationTargetException ex) {
	    
	    Throwable cause = ex.getCause();
	    
	    if (cause instanceof RuntimeException) {
		throw (RuntimeException) cause;
	    }
	    else {
		throw (ZSDXException) cause;
	    }
	}
	
	return entity;
    }

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

	if (isResizable()) {
	    buff.append('\t');
	    buff.append(getWidth());
	    buff.append('\t');
	    buff.append(getHeight());
	}

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
    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public boolean isValid() {
	
	try {
	    checkPositionTopLeft(positionInMap.x, positionInMap.y);
	    
	    if (isResizable()) {
		checkSize(positionInMap.width, positionInMap.height);
	    }
	    return true;
	}
	catch (MapException ex) {
	    return false;
	}
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
     * Returns the coordinates of the origin point of the entity.
     * By default, it is (0,0) (the origin is the top-left corner).
     * Redefine this method in a subclass to change the origin.
     * @return the coordinates of the origin point
     */
    protected Point getOrigin() {
	return defaultOrigin;
    }

    /**
     * Returns the y coordinate of the origin point of the entity.
     * By default, it is zero (the origin is the top-left corner).
     * Redefine this method in a subclass to change the origin.
     * @return the y coordinate of the origin point
     */
    protected int getOriginY() {
	return 0;
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

	checkPositionTopLeft(position.x, position.y);
	checkSize(position.width, position.height);
	
	setPositionTopLeftImpl(position.x, position.y);
	setSizeImpl(position.width, position.height);
    }
    
    /**
     * Changes the position of the entity on the map, by specifying two points
     * (the top-left corner and the bottom-right corner).
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     * @throws MapException if the entity is not resizable, or the rectangle width
     * or its height is zero, or the coordinates are wrong
     */
    public void setPositionInMap(int x1, int y1, int x2, int y2) throws MapException {

	int x = Math.min(x1, x2);
	int width = Math.abs(x2 - x1);
	
	int y = Math.min(y1, y2);
	int height = Math.abs(y2 - y1);

	checkPositionTopLeft(x, y);
	checkSize(width, height);
	
	setPositionTopLeftImpl(x, y);
	setSizeImpl(width, height);
    }
    
    /**
     * Changes the position of the entity on the map, by specifying the coordinates of its hotspot.
     * The size of the entity is not changed.
     * @param x x coordinate of the hotspot
     * @param y y coordinate of the hotspot
     * @throws MapException if the coordinates of the top-left corner are not divisible by 8
     */
    public void setPositionInMap(int x, int y) throws MapException {
	
	// calculate the new coordinates of the top-left corner
	Point origin = getOrigin();
	setPositionTopLeft(x - origin.x, y - origin.y);
    }

    /**
     * Checks whether the specified position of the entity is correct (i.e. whether the
     * coordinates of the top-left corner are multiple of 8).
     * @param x the x coordinate of the top-left corner to check
     * @param y the y coordinate of the top-left corner to check
     * @throws MapException if the coordinates specified are not divisible by 8
     */
    protected void checkPositionTopLeft(int x, int y) throws MapException {

	if (x % 8 != 0) {
	    throw new MapException("Wrong x value (" + x + "): the coordinates of the top-left corner must divisible by 8");
	}

	if (y % 8 != 0) {
	    throw new MapException("Wrong y value (" + y + "): the coordinates of the top-left corner must divisible by 8");
	}
    }

    /**
     * Changes the position of the entity on the map, by specifying the coordinates of its
     * top-left corner.
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner
     * @throws MapException if the coordinates specified are not divisible by 8
     */
    public void setPositionTopLeft(int x, int y) throws MapException {
	
	checkPositionTopLeft(x, y);
	setPositionTopLeftImpl(x, y);
	
	// notify
	setChanged();
	notifyObservers();
    }
    
    /**
     * Changes the position of the entity on the map, by specifying the coordinates of its
     * top-left corner.
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner
     */
    protected void setPositionTopLeftImpl(int x, int y) throws MapException {
	positionInMap.x = x;
	positionInMap.y = y;
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
     * Checks whether the specified size of the entity is correct (i.e. whether it is
     * a multiple of getUnitWidth() and getUnitHeight()).
     * @param width the width to check
     * @param height the height to check
     * @throws MapException if the size is not correct
     */
    protected void checkSize(int width, int height) throws MapException {

	if (!isResizable()) {
	    throw new MapException("This entity is not resizable");
	}

	if (width <= 0 || height <= 0) {
	    throw new MapException("The entity's size must be positive"); 
	}
	
	Dimension unitSize = getUnitSize();
	if (width % unitSize.width != 0) {
	    throw new MapException("The entity's width must be a multiple of " + unitSize.width
		    + " (the width specified is " + width + ')');
	}

	if (height % unitSize.height != 0) {
	    throw new MapException("The entity's height must be a multiple of " + unitSize.height
		    + " (the height specified is " + height + ')');
	}
    }

    /**
     * Changes the size of the entity on the map.
     * The location of the entity is not changed.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     * @throws MapException if the entity is not resizable,
     * or the size specified is lower than or equal to zero,
     * or the size specified is not divisible by 8
     */
    public void setSize(int width, int height) throws MapException {

	checkSize(width, height);
	setSizeImpl(width, height);
	
	// notify
	setChanged();
	notifyObservers();
    }

    /**
     * Changes the size of the entity.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     */
    protected void setSizeImpl(int width, int height) {
	positionInMap.width = width;
	positionInMap.height = height;
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
     * or not valid
     */
    public void setName(String name) throws UnsupportedOperationException, MapException {

	if (!hasName()) {
	    throw new UnsupportedOperationException("This entity is not identifiable");
	}
	
	if (name.indexOf(' ') != -1 || name.indexOf('\t') != -1) {
	    throw new MapException("The entity name cannot have whitespaces");
	}
	
	MapEntity other = map.getEntityWithName(getType(), name);
	if (other != null && other != this) {
	    throw new MapException("The name '" + name
		    + "'is already used by another entity of this map");
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
	String prefix = getTypeName(entityType);
	int i = 1;
	
	// compute an unused name
	while (map.getEntityWithName(entityType, prefix + i) != null) {
	    i++;
	}
	
	try {
	    setName(getTypeName(entityType) + i);
	}
	catch (MapException ex) {
	    // should not happen if the while above works
	    System.err.println("Unexcepted error: " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return the minimum size of the entity
     */
    public Dimension getUnitSize() {
	return defaultUnitSize;
    }

    /**
     * Changes the position of the entity.
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     * @throws MapException if the new coordinates are not divisible by 8
     */
    public void move(int dx, int dy) throws MapException {
	
	int x = getX() + dx;
	int y = getY() + dy;

	setPositionInMap(x, y);

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
     * Returns the x coordinate of the entity's origin point on the map.
     * @return the x coordinate of the entity's origin point on the map
     */
    public int getX() {
	return positionInMap.x + getOrigin().x;
    }

    /**
     * Returns the y coordinate of the entity's origin point on the map.
     * @return the y coordinate of the entity's origin point on the map
     */
    public int getY() {
	return positionInMap.y + getOrigin().y;
    }

    /**
     * Returns the x coordinate of the entity's top left corner.
     * @return the x coordinate of the entity's top left corner
     */
    public int getXTopLeft() {
	return positionInMap.x;
    }

    /**
     * Returns the y coordinate of the entity's top left corner.
     * @return the y coordinate of the entity's top left corner
     */
    public int getYTopLeft() {
	return positionInMap.y;
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
     * This function is called when some requested information about the image comes.
     * @return true
     */
    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height) {
	return true;
    }

    /**
     * Returns an integer identifying the kind of entity: ENTITY_TILE, ENTITY_ENTRANCE...
     * @return the type of entity
     */
    public abstract int getType();
    
    /**
     * Returns the name of an entity type.
     * This method uses the 'entityTypeName' static field
     * that every concrete subclass of MapEntity must implement.
     * @param type a type of entity
     * @return the corresponding name
     */
    public static String getTypeName(int type) {

	String typeName = null;
	Class<?> entityClass = entityClasses[type];
	try {
	    Field typeNameField = entityClass.getField("entityTypeName");
	    typeName = (String) typeNameField.get(null);
	}
	catch (NoSuchFieldException ex) {
	    System.err.println("The field 'entityTypeName' is missing in class " + entityClass.getName());
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("The field 'entityTypeName' of class " + entityClass.getName() + " is not accessible");
	    System.exit(1);
	}
	return typeName;
    }
}
