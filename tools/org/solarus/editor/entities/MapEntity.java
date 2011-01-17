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
package org.solarus.editor.entities;

import java.awt.*;
import java.lang.reflect.*;
import java.util.*;

import org.solarus.editor.*;
import org.solarus.editor.Map;

/**
 * Represents an entity placed on the map with the map editor.
 *
 * To create a new kind of entity, you should do the following steps:
 * - Add your new type of entity into the EntityType enumeration.
 * - Add the entity in the AddEntitiesToolbar.cells array.
 * - Add the entity in the AddEntitiesMenu.itemDefinitions array.
 * - Create a subclass of MapEntity for your entity and:
 *   - If your entity has a notion of subtype, create a Subtype enumeration
 *       that implements the EntitySubtype interface.
 *   - Define the specific properties and their default values
 *       by redefining the method setPropertiesDefaultValues().
 *   - Redefine the checkProperties() method: public boolean checkProperties()
 *       to check the validity of the specific properties.
 *   - Create a constructor with the following signature:
 *       public YourEntity(Map map) throws MapException.
 *   - Redefine if necessary the getObstacle() method: public Obstacle getObstacle()
 *       if your entity is an obstacle.
 *   - Redefine if necessary some methods to define the direction property of your type of entity:
 *       - public int getNbDirections(): returns the number of possible
 *           directions of this kind of entity (should be 0, 4 or 8; default is zero)
 *       - public boolean canHaveNoDirections() (only when getNbDirections() is not zero):
 *           indicates that an additional direction 'none' also exists (default is false)
 *       - public String getNoDirectionText() (only when canHaveNoDirections() is true):
 *           returns the text to display in the GUI for the special direction 'none'
 *           (default is "No direction")
 *   - Redefine if necessary the hasName() method:
 *       public boolean hasName(): indicates whether the entity has a 
 *       name property, i.e. if it is identifiable (default is false)
 *   - Redefine if necessary the isResizable() method:
 *       public boolean isResizable(): indicates whether the entity can
 *       be resized (default is false). If the entity is resizable, you can
 *       also redefine the getUnitarySize() method (default is 8*8).
 *   - Redefine the getOrigin() method: public Point getOrigin()
 *       if the origin is not the top-left corner of the entity.
 *   - Define the static generalImageDescriptions field:
 *       public static final EntityImageDescription[] generalImageDescriptions
 *       to define a default 16*16 image representing this kind of entity for each subtype.
 *       These 16*16 images will be used to build the toolbar to add entities on the map.
 *   - Redefine if necessary the non-static updateImageDescription() method:
 *       public void updateImageDescription()
 *       which updates the image representing the entity on the map in its current state.
 *   - If your entity is not drawn from a fixed image file but in a more complex way,
 *       you cannot use updateImageDescription() and you have to redefine directly the paint() method:
 *       public abstract void paint(Graphics g, double zoom, boolean showTransparency).
 * - If your entity type has specific properties, create a subclass of EditEntityComponent.
 * - Declare this class in the EditEntityComponent.editEntityComponentClasses array
 *     (or just EditEntityComponent.class if your don't have specific properties).
 */
public abstract class MapEntity extends Observable {

    /**
     * The map.
     */
    protected Map map;

    /**
     * Indicates that the entity has been initialized.
     */
    protected boolean initialized;

    /**
     * Position of the entity in the map.
     */
    protected Rectangle positionInMap;

    /**
     * Layer of the entity on the map.
     */
    protected Layer layer;

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
     * The subtype of entity, or null if the entity has no subtype.
     */
    protected EntitySubtype subtype;

    /**
     * All other properties of the entity, specific to each entity type.
     */
    protected LinkedHashMap<String, String> specificProperties;

    /**
     * Color to display instead of the transparent pixels of the image.
     */
    public static final Color bgColor = new Color(255, 0, 255);

    /**
     * Coordinates of the origin point of an entity by default.
     */
    private static final Point defaultOrigin = new Point(0, 0);

    /**
     * Default minimum size of a resizable entity.
     */
    private static final Dimension defaultUnitarySize = new Dimension(8, 8);

    /**
     * Description of the image representing currently this particular entity.
     */
    protected EntityImageDescription currentImageDescription;

    /**
     * Creates an entity.
     * If the entity is identifiable, a default name
     * is given to the entity (this name is computed such that it is
     * different from the other entities of the same kind on the map).
     * @param map the map
     * @param width width of the entity
     * @param height height of the entity
     */
    protected MapEntity(Map map, int width, int height) throws MapException {

	this.map = map;

	// default values
	this.layer = Layer.LOW;
	this.positionInMap = new Rectangle(0, 0, width, height);
	this.specificProperties = new LinkedHashMap<String, String>();

	if (hasSubtype()) {
	    setSubtype(getDefaultSubtype());
	}

	try {
	    setPropertiesDefaultValues();
	}
	catch (MapException ex) {
	    System.err.println("Unexpected error: could not set the default values for entity '" + getType() + "': " + ex.getMessage());
	}

	initializeImageDescription();

	if (hasName()) {
	    computeDefaultName();
	}
    }

    /**
     * Creates a new map entity with the specified type.
     * @param map the map
     * @param entityType the type of entity to create
     * @param entitySubtype the subtype of entity to create
     * @return the entity created
     */
    public static MapEntity create(Map map, EntityType entityType, EntitySubtype entitySubtype) throws MapException {

	MapEntity entity = null;
	Class<? extends MapEntity> entityClass = null;
	try {
	    entityClass = entityType.getEntityClass();
	    Constructor<? extends MapEntity> constructor = entityClass.getConstructor(Map.class);
	    entity = constructor.newInstance(map);
	    entity.setSubtype(entitySubtype);
	    entity.initializeImageDescription();
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("Cannot find the constructor of " + entityClass);
	    System.exit(1);
	}
	catch (InvocationTargetException ex) {
	    System.err.println("Cannot create the entity: " + ex.getMessage());
	    ex.getCause().printStackTrace();
	    System.exit(1);
	}
	catch (InstantiationException ex) {
	    System.err.println("Cannot create the entity: " + ex.getMessage());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot create the entity: " + ex.getMessage());
	    ex.printStackTrace();
	    System.exit(1);
	}

	entity.initialized = true;
	return entity;
    }

    /**
     * Creates a map entity from a string description as returned by toString().
     * @param map the map (needed for some types of entities)
     * @param description a string describing the entity, as returned by toString()
     * @throws MapException if the string is incorrect
     */
    public static MapEntity createFromString(Map map, String description) throws MapException {

	int index = description.indexOf('\t');
	EntityType entityType = EntityType.get(Integer.parseInt(description.substring(0, index)));

	MapEntity entity = MapEntity.create(map, entityType, entityType.getDefaultSubtype());
	entity.parse(description.substring(index + 1));
	entity.updateImageDescription();

	// now the origin is valid
	entity.setPositionInMap(entity.positionInMap.x, entity.positionInMap.y);
	entity.setInitialized(true);
	return entity;
    }

    /**
     * Parses the entity.
     * @param description a string describing the entity, as returned by toString(),
     * except that the first value is missing (the entity type)
     * @throws MapException if the line contains an error
     */
    protected final void parse(String description) throws MapException {

	String token = null;
	try {
	    StringTokenizer tokenizer = new StringTokenizer(description, "\t");

	    token = tokenizer.nextToken();
	    Layer layer = Layer.get(Integer.parseInt(token));

	    token = tokenizer.nextToken();
	    int x = Integer.parseInt(token);

	    token = tokenizer.nextToken();
	    int y = Integer.parseInt(token);

	    setLayer(layer);
	    positionInMap.x = x; // for now the origin is (0,0)
	    positionInMap.y = y;

	    int width = 0;
	    int height = 0;
	    if (isSizeVariable()) {
		token = tokenizer.nextToken();
		width = Integer.parseInt(token);
		token = tokenizer.nextToken();
		height = Integer.parseInt(token);
	    }

	    if (hasName()) {
		token = tokenizer.nextToken();
		setName(token);
	    }

	    if (hasDirectionProperty()) {
		token = tokenizer.nextToken();
		setDirection(Integer.parseInt(token));
	    }

	    if (hasSubtype()) {
		token = tokenizer.nextToken();
		setSubtype(getSubtype(Integer.parseInt(token)));
	    }

	    // specific properties
	    for (String name: specificProperties.keySet()) {
		token = tokenizer.nextToken();
		setProperty(name, token);
	    }

	    if (isSizeVariable()) {
		setSize(width, height); // some entities need to know their properties before they can be resized
	    }
	}
	catch (NoSuchElementException ex) {
	    if (token == null) {
		throw new MapException("Value expected, empty line found");
	    }
	    else {
		throw new MapException("Value expected after '" + token + "'");
	    }
	}
	catch (NumberFormatException ex) {
	    throw new MapException("Integer expected, found '" + token + "'");
	}
    }

    /**
     * Creates a copy of the specified entity.
     * If the entity has a name, a different name is automatically assigned to the copy.
     * @param map the map
     * @param other an entity
     * @return the copy created
     */
    public static MapEntity createCopy(Map map, MapEntity other) {

	try {
	    return createFromString(map, other.toString());
	}
	catch (ZSDXException ex) {
	    System.err.println("Unexpected error: cannot create a copy of entity '" + other + "'");
	    return null;
	}
    }

    /**
     * Returns a string describing this entity.
     * @return a string representation of the entity
     */
    public final String toString() {

	StringBuffer buff = new StringBuffer();
	buff.append(getType().getIndex());
	buff.append('\t');
	buff.append(getLayer().getId());
	buff.append('\t');
	buff.append(getX());
	buff.append('\t');
	buff.append(getY());

	if (isSizeVariable()) {
	    buff.append('\t');
	    buff.append(getWidth());
	    buff.append('\t');
	    buff.append(getHeight());
	}

	if (hasName()) {
	    buff.append('\t');
	    buff.append(getName());
	}

	if (hasDirectionProperty()) {
	    buff.append('\t');
	    buff.append(getDirection());
	}

	if (hasSubtype()) {
	    buff.append('\t');
	    buff.append(getSubtypeId());
	}

	for (String value: specificProperties.values()) {
	    buff.append('\t');
	    buff.append(value);
	}

	return buff.toString();
    }

    /**
     * Sets whether the entity is initialized.
     * @param initialized true if the entity is initialized
     */
    private void setInitialized(boolean initialized) {
	this.initialized = initialized;
    }

    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public final boolean isValid() {
	
	try {
	    // check the common properties
	    checkPositionTopLeft(positionInMap.x, positionInMap.y);

	    if (isResizable()) {
		checkSize(positionInMap.width, positionInMap.height);
	    }

	    // check the specific properties
	    checkProperties();

	    return true;
	}
	catch (MapException ex) {
	    return false;
	}
    }
    
    /**
     * Returns the entity's obstacle property (default is Obstacle.NONE).
     * @return the obstacle property
     */
    public Obstacle getObstacle() {
	return Obstacle.NONE;
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
	
	setPositionTopLeft(position.x, position.y);
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
     * or its height is zero, or the coordinates are wrong
     */
    public void setPositionInMap(int x1, int y1, int x2, int y2) throws MapException {

	int x = Math.min(x1, x2);
	int width = Math.abs(x2 - x1);
	
	int y = Math.min(y1, y2);
	int height = Math.abs(y2 - y1);

	checkPositionTopLeft(x, y);
	checkSize(width, height);

	setPositionTopLeft(x, y);
	setSize(width, height);
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
     * Returns whether two entities of this type can have different sizes.
     * This might happen for some kind of entities even if they are not resizable.
     * If this function returns true, the editor will parse the width an the height 
     * for this kind of entity.
     * @return true if two entities of this type can have different sizes
     */
    public boolean isSizeVariable() {
	return isResizable();
    }

    /**
     * Returns whether or not the entity can be resized by extending it
     * horizontally or vertically.
     * This function only makes sense for resizable entities, otherwise
     * the behavior is unspecified.
     * By default, this function returns true. The subclasses which represent
     * resizable entities should can redefine this method if they want
     * to put some constraints on the resizing direction.
     * @param direction one of the two main directions (0: horizontal, 1: vertical)
     * @return whether or not the entity can be expanded in that direction
     */
    public boolean isExtensible(int direction) {
	return true;
    }

    /**
     * Returns whether the entity has to remain square when it is being resized.
     * By default, false is returned.
     * @return true if the entity must be square
     */
    public boolean mustBeSquare() {
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

	if (!isSizeVariable()) {
	    throw new MapException("This entity's size is not variable");
	}

	if (width <= 0 || height <= 0) {
	    throw new MapException("The entity's size must be positive (the size specified is ("
		    + width + "x" + height + "))"); 
	}
	
	Dimension unitSize = getUnitarySize();
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

	if (!isSizeVariable()) {
	    throw new MapException("This entity is not resizable");
	}

	checkSize(width, height);
	setSizeImpl(width, height);
	
	// notify
	setChanged();
	notifyObservers();
    }

    /**
     * Changes the size of the entity on the map.
     * The location of the entity is not changed.
     * @param size of the entity in pixels
     * @throws MapException if the entity is not resizable,
     * or the size specified is lower than or equal to zero,
     * or the size specified is not divisible by 8
     */
    public void setSize(Dimension size) throws MapException {

        setSize(size.width, size.height);
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
     * Changes the size of the entity.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     */
    protected void setSizeImpl(Dimension size) {

        setSizeImpl(size.width, size.height);
    }

    /**
     * Returns whether this type of entity has a direction property.
     * The subclasses which represent entities with a direction
     * should return true.
     * @return true if the entity has a direction, false otherwise
     */
    public boolean hasDirectionProperty() {
	return getNbDirections() > 1;
    }

    /**
     * Returns the number of possible directions of the entity.
     * By default, an entity has no direction property and this function returns 0.
     * The number returned does not include the possible special value of -1 that
     * exists for certain types of entity. 
     * @return the number of possible directions of the entity (or 0
     * if the entity has not a direction property)
     */
    public int getNbDirections() {
	return 0;
    }

    /**
     * Returns the direction of the entity.
     * The entity should have a direction (i.e. hasDirection() returns true).
     * The value returned is between 0 and getNbDirections() - 1.
     * When the canHaveNoDirection() is true, this method can also return -1
     * to indicate that no direction is set.
     * @return the entity's direction
     */
    public int getDirection() {
	return direction;
    }

    /**
     * Returns whether this entity can have the special direction value -1
     * indicating that no direction is set.
     * This method makes sense only for entities having a direction property.
     * By default, this method returns false.
     * @return true if this entity can have the special direction value -1
     */
    public boolean canHaveNoDirection() {
	return false;
    }

    /**
     * For an entity that includes an option to set 'no direction'
     * (i.e. when canHaveNoDirection() returns true),
     * this method returns the text that will be displayed in the direction chooser.
     * By default, "No direction" is returned but you can redefine this method
     * to display a more precise text.
     * @return the text that will be displayed in the direction chooser for the 'no direction' option if any
     */
    public String getNoDirectionText() {
	return canHaveNoDirection() ? "No direction" : null;
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

	if (!hasDirectionProperty()) {
	    throw new UnsupportedOperationException("This entity has no direction property");
	}

	if (direction == -1 && !canHaveNoDirection()) {
	    throw new IllegalArgumentException("The direction '-1' is not valid for this entity");
	}

	if (direction < -1 || direction >= getNbDirections()) {
	    throw new IllegalArgumentException("The direction '" + direction
		    	+ "' is incorrect: the number of possible directions is " + getNbDirections());
	}

	this.direction = direction;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether the entity has a name.
     * By default, an entity has no name and this function returns false.
     * The subclasses which represent identifiable entities should return true.
     * @return true if the entity has a name, false otherwise
     */
    public boolean hasName() {
	return false;
    }

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
     * If the name specified is already used, another name is automatically set.
     * @param name the new entity's name
     * @throws UnsupportedOperationException if the entity is not identifiable
     * @throws MapException if this name is not valid
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

	    int counter = 2;
	    String[] words = name.split("_");
	    StringBuffer prefix;

	    if (words.length > 1) {

		prefix = new StringBuffer();
		for (int i = 0; i < words.length - 1; i++) {
		    prefix.append(words[i]);
		    prefix.append('_');
		}
		String suffix = words[words.length - 1];

		try {
		    counter = Integer.parseInt(suffix);
		}
		catch (NumberFormatException ex) {
		    prefix.append(suffix);
		    prefix.append('_');
		}
	    }
	    else {
		prefix = new StringBuffer(name);
		prefix.append('_');
	    }

	    counter = 2;
	    while (map.getEntityWithName(getType(), prefix.toString() + counter) != null) {
		counter++;
	    }
	    name = prefix.toString() + counter;
	}

	// debug
	other = map.getEntityWithName(getType(), name);
	if (other != null && other != this) {
	    throw new MapException("An entity with this name already exists and I was unable to compute a new one");
	}

	this.name = name;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the prefix of the default name for this kind of entity.
     */
    protected String getDefaultNamePrefix() {
	EntityType entityType = getType();
	return entityType.getName().toLowerCase().replace(' ', '_');
    }

    /**
     * Sets a default name to the entity.
     * @param map the map
     */
    private void computeDefaultName() {
	
	try {
	    setName(getDefaultNamePrefix());
	}
	catch (MapException ex) {
	    // should not happen since setName() makes sure the name is unique
	    System.err.println("Unexcepted error: " + ex.getMessage());
	    ex.printStackTrace();
	    System.exit(1);
	}
    }

    /**
     * Makes sure the name of this entity is unique on the map, renaming it if necessary.
     * setName() guarantees that no other entity has the same name on the map, but two
     * entities may have been created with the same name without detecting it because they are
     * not added to the map yet. This can happen with copy/paste operations.
     * If the entity is not identifiable, this method does nothing.
     * @param map the map
     */
    public void ensureNameIsUnique() {
	
	if (hasName()) {
	    try {
		setName(name);
	    }
	    catch (MapException ex) {
		// should not happen
		System.err.println("Unexcepted error: " + ex.getMessage());
		ex.printStackTrace();
		System.exit(1);
	    }
	}
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return the minimum size of the entity
     */
    public Dimension getUnitarySize() {
	return defaultUnitarySize;
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
     * Returns the layer of this entity.
     * @return the layer of this entity (Layer.LOW for most of the entities)
     */
    public Layer getLayer() {
	return layer;
    }

    /**
     * Changes the layer of this entity.
     * @param layer the new layer of this entity
     */
    public void setLayer(Layer layer) {
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
     * Initializes the description of the image currently representing the entity.
     * By default, the image description is initialized to a copy of the
     * image description of this kind of entity.
     */
    protected void initializeImageDescription() {
	EntityImageDescription generalImageDescription = getImageDescription(getType(), getSubtype());

	if (generalImageDescription != null) {
	    currentImageDescription = new EntityImageDescription(generalImageDescription);
	    updateImageDescription();
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     * By default, the image description is a copy of the general image description of this kind of entity.
     * Redefine this method to display the entity with an image containing
     * the entity's current properties, by modifying the currentImageDescription field.
     */
    public void updateImageDescription() {
	
      EntityImageDescription generalImageDescription = getImageDescription(getType(), getSubtype());
	
      if (generalImageDescription != null) {
	// by default, the image description is the general description of the subtype
	currentImageDescription.set(generalImageDescription);
      }
    }

    /**
     * Returns the images representing a specified kind entity.
     * @param type a type of entity
     * @return the image descriptions of this kind of entity
     */
    public static EntityImageDescription[] getImageDescriptions(EntityType type) {

	EntityImageDescription imageDescriptions[] = null;

	Class<? extends MapEntity> entityClass = type.getEntityClass();
	try {
	    Field imageDescriptionField = entityClass.getField("generalImageDescriptions");
	    imageDescriptions = (EntityImageDescription[]) imageDescriptionField.get(null);
	}
	catch (NoSuchFieldException ex) {
	    System.err.println("The field 'generalImageDescriptions' is missing in class " + entityClass.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot access the field 'generalImageDescriptions' in class " + entityClass.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	return imageDescriptions;
    }
    
    /**
     * Returns the number of images representing a specified kind entity.
     * @param type a type of entity
     * @return the number of image descriptions of this kind of entity
     */
    public static int getNbImageDescriptions(EntityType type) {
	return getImageDescriptions(type).length;
    }

    /**
     * Returns the description of an image representing a specified kind entity.
     * @param type a type of entity
     * @param subtype the subtype of entity
     * @return an image description of this subtype of entity
     */
    public static EntityImageDescription getImageDescription(EntityType type, EntitySubtype subtype) {

	EntityImageDescription[] imageDescriptions = getImageDescriptions(type);
	if (imageDescriptions == null) {
	    return null;
	}

	int index;
	if (type.hasSubtype()) {
	    if (subtype == null) {
		subtype = type.getDefaultSubtype();
	    }
	    index = ((Enum<?>) subtype).ordinal();
	}
	else {
	    index = 0;
	}

	return getImageDescriptions(type)[index];
    }

    /**
     * Draws the entity on the map view.
     * This method draws the entity's image as described by the currentImageDescription field,
     * which you can modify by redefining the updateImageDescription() method.
     * To draw a more complex image, redefine the paint() method.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {
	currentImageDescription.paint(g, zoom, showTransparency, positionInMap);
    }

    /**
     * Draws a rectangle border on the entity.
     * This function can be called by the paint() method of suclasses that need
     * to draw the entity in a complex way (i.e. not just by blitting an image file).
     * @param g the graphic context
     * @param zoom zoom of the image
     * @param outlineColor a color to fill the area between the two lines of the border
     */
    protected void drawEntityOutline(Graphics g, double zoom, Color outlineColor) {

	int dx1 = (int) (positionInMap.x * zoom);
	int dy1 = (int) (positionInMap.y * zoom);
	int dx2 = (int) (dx1 + positionInMap.width * zoom);
	int dy2 = (int) (dy1 + positionInMap.height * zoom);

	g.setColor(Color.black);
	g.drawLine(dx1, dy1, dx2 - 1, dy1);
	g.drawLine(dx1 + 3, dy1 + 3, dx2 - 4, dy1 + 3);
	g.drawLine(dx1, dy2 - 1, dx2 - 1, dy2 - 1);
	g.drawLine(dx1 + 3, dy2 - 4, dx2 - 4, dy2 - 4);
	g.drawLine(dx1, dy1, dx1, dy2 - 1);
	g.drawLine(dx1 + 3, dy1 + 3, dx1 + 3, dy2 - 4);
	g.drawLine(dx2 - 1, dy1, dx2 - 1, dy2 - 1);
	g.drawLine(dx2 - 4, dy1 + 3, dx2 - 4, dy2 - 4);

	g.setColor(outlineColor);
	g.drawLine(dx1 + 1, dy1 + 1, dx2 - 2, dy1 + 1);
	g.drawLine(dx1 + 2, dy1 + 2, dx2 - 3, dy1 + 2);
	g.drawLine(dx1 + 2, dy2 - 3, dx2 - 3, dy2 - 3);
	g.drawLine(dx1 + 1, dy2 - 2, dx2 - 2, dy2 - 2);
	g.drawLine(dx1 + 1, dy1 + 1, dx1 + 1, dy2 - 2);
	g.drawLine(dx1 + 2, dy1 + 2, dx1 + 2, dy2 - 3);
	g.drawLine(dx2 - 3, dy1 + 2, dx2 - 3, dy2 - 3);
	g.drawLine(dx2 - 2, dy1 + 1, dx2 - 2, dy2 - 2);
    }

    /**
     * Returns a value identifying the kind of entity: TILE, DESTINATION_POINT...
     * @return the type of entity
     */
    public final EntityType getType() throws IllegalStateException {
	return EntityType.get(getClass());
    }

    /**
     * Returns whether this entity has a subtype.
     * Whether the entity has
     * a subtype depends on the subtype class specified in the EntityType enumeration.
     * @return true if this entity has a subtype
     */
    public final boolean hasSubtype() {
	return getType().hasSubtype();
    }

    /**
     * Returns the subtype id of this entity.
     * @return the subtype id
     */
    public final int getSubtypeId() {

	if (subtype == null) {
	    return 0;
	}
	
	return subtype.getId();
    }

    /**
     * Returns the subtype of this entity.
     * @return the subtype, or null if this type of entity has no subtype
     */
    public final EntitySubtype getSubtype() {
	return subtype;
    }

    /**
     * Returns the subtype corresponding to the specified subtype id
     * for this type of entity.
     * You might redefine this method to declare a more specific return type.
     * @param id the subtype id
     * @return the subtype
     */
    public final EntitySubtype getSubtype(int id) {
	return getType().getSubtype(id);
    }

    /**
     * Returns the default subtype of this type of entity,
     * i.e. the subtype with id 0.
     * @return the default subtype
     */
    public final EntitySubtype getDefaultSubtype() {
	return getType().getDefaultSubtype();
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype
     * @throws MapException if the subtype is not valid
     */
    public final void setSubtypeId(int subtype) throws MapException {
	setSubtype(getSubtype(subtype));
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype to set
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {
	this.subtype = subtype;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether a value specific to the current entity type is defined.
     * @param name name of the property to look for
     * @return true if this property has a value
     */
    public final boolean hasProperty(String name) {
	return specificProperties.get(name) != null;
    }

    /**
     * Returns a string value specific to the current entity type.
     * @param name name of the property to get
     * @return the value of this property
     */
    public final String getProperty(String name) {

	String value = specificProperties.get(name);
	if (value == null) {
	    throw new IllegalArgumentException("There is no property with name '" + name +
		    "' for entity " + getType());
	}
	return value;
    }

    /**
     * Returns a integer value specific to the current entity type.
     * @param name name of the property to get
     * @return the value of this property
     */
    public final int getIntegerProperty(String name) {
	return Integer.parseInt(getProperty(name));
    }

    /**
     * Returns a boolean value specific to the current entity type.
     * @param name name of the property to get
     * @return the value of this property
     */
    public final boolean getBooleanProperty(String name) {
	return Integer.parseInt(getProperty(name)) != 0;
    }

    /**
     * Sets a property specific to the current entity type.
     * @param name name of the property
     * @param value value of the property
     */
    public void setProperty(String name, String value) throws MapException {
        specificProperties.put(name, value);
    }

    /**
     * Sets a property specific to the current entity type.
     * @param name name of the property
     * @param value value of the property
     */
    public final void setProperty(String name, int value) throws MapException {
	setProperty(name, Integer.toString(value));
    }

    /**
     * Sets a property specific to the current entity type.
     * @param name name of the property
     * @param value value of the property
     */
    public final void setProperty(String name, boolean value) throws MapException {
	setProperty(name, value ? "1" : "0");
    }

    /**
     * Returns all specific properties of the current entity.
     * @return the specific properties
     */
    public final LinkedHashMap<String, String> getProperties() {
	return specificProperties;
    }

    /**
     * Sets all specific properties of the current entity.
     * @param properties the specific properties
     */
    public final void setProperties(LinkedHashMap<String, String> properties) throws MapException {

        for (java.util.Map.Entry<String, String> entry: properties.entrySet()) {
	    setProperty(entry.getKey(), entry.getValue());
	}
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     * Redefine this method to define the default value of your properties.
     */
    public void setPropertiesDefaultValues() throws MapException {

    }

    /**
     * Checks the specific properties.
     * Redefine this method to check the values.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

    }


    /**
     * Changes the tileset used to represent this entity on the map.
     * By default, nothing is done since most of the entities do not use the tileset.
     * @param tileset the tileset
     * @throws MapException if the new tileset could not be applied to this entity
     */
    public void setTileset(Tileset tileset) throws MapException {

    }
}
