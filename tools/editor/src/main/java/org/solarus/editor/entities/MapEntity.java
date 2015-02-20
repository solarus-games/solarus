/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.entities;

import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.luaj.vm2.*;

import java.awt.*;
import java.io.PrintWriter;
import java.lang.reflect.*;
import java.util.*;

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
 *       by redefining the method createProperties().
 *   - Redefine the checkProperties() method: public boolean checkProperties()
 *       to check the validity of the specific properties.
 *   - Create a constructor with the following signature:
 *       public YourEntity(Map map) throws MapException.
 *   - Redefine if necessary some methods to define the direction property of your type of entity:
 *       - public int getNbDirections(): returns the number of possible
 *           directions of this kind of entity (should be 0, 4 or 8; default is zero)
 *       - public boolean canHaveNoDirections() (only when getNbDirections() is not zero):
 *           indicates that an additional direction 'none' also exists (default is false)
 *       - public String getNoDirectionText() (only when canHaveNoDirections() is true):
 *           returns the text to display in the GUI for the special direction 'none'
 *           (default is "No direction")
 *   - Redefine if necessary the hasInitialLayer() method:
 *       public boolean hasInitialLayer(): indicates that your entity already
 *       knows on what layer to be created. If you return false, the layer will
 *       be automatically set to the layer under the cursor.
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
    private Map map;

    /**
     * Position of the entity in the map.
     */
    private Rectangle positionInMap;

    /**
     * Layer of the entity on the map.
     */
    private Layer layer;

    /**
     * Direction of the entity (0 to 3).
     * Not used by all kinds of entities.
     */
    private int direction;

    /**
     * Name identifying the entity or null.
     */
    private String name;

    /**
     * Sprite representing this entity or null.
     */
    private Sprite sprite;

    /**
     * The subtype of entity, or null if the entity has no subtype.
     */
    private EntitySubtype subtype;

    /**
     * All other properties of the entity, specific to each entity type.
     */
    private LinkedHashMap<String, String> specificProperties;

    /**
     * Properties that are optional in specificProperties
     * and their default values.
     */
    private LinkedHashMap<String, String> optionalPropertiesDefaultValues;

    /**
     * Type of each existing property for this entity
     * (may be String, Integer or Boolean).
     * This structure is only used to remember what variant
     * of setProperty() was called.
     */
    private HashMap<String, Class<?>> propertyTypes;

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
        this.optionalPropertiesDefaultValues = new LinkedHashMap<String, String>();
        this.propertyTypes = new HashMap<String, Class<?>>();

        if (hasSubtype()) {
            setSubtype(getDefaultSubtype());
        }

        try {
            createProperties();
        }
        catch (MapException ex) {
            System.err.println("Unexpected error: could not set the initial values for entity '" + getType() + "': " + ex.getMessage());
        }

        initializeImageDescription();
    }

    /**
     * Creates a new map entity with the specified type.
     * @param map the map
     * @param entityType the type of entity to create
     * @param entitySubtype the subtype of entity to create
     * @return The entity created (never null).
     * @throws MapException If the entity creation failed.
     */
    public static MapEntity create(Map map, EntityType entityType, EntitySubtype entitySubtype)
            throws MapException {

        MapEntity entity = null;
        Class<? extends MapEntity> entityClass = null;
        try {
            entityClass = entityType.getEntityClass();
            Constructor<? extends MapEntity> constructor = entityClass.getConstructor(Map.class);
            entity = constructor.newInstance(map);
            entity.setSubtype(entitySubtype);
            entity.initializeImageDescription();
        }
        catch (InvocationTargetException ex) {
            System.err.println("Cannot create the entity: " + ex.getCause().getMessage());
            ex.getCause().printStackTrace();
            throw new MapException(ex.getCause().getMessage());
        }
        catch (NoSuchMethodException ex) {
            System.err.println("Cannot find the constructor of " + entityClass);
            ex.getCause().printStackTrace();
            throw new MapException(ex.getMessage());
        }
        catch (InstantiationException ex) {
            System.err.println("Cannot create the entity: " + ex.getMessage());
            ex.printStackTrace();
            throw new MapException(ex.getMessage());
        }
        catch (IllegalAccessException ex) {
            System.err.println("Cannot create the entity: " + ex.getMessage());
            ex.printStackTrace();
            throw new MapException(ex.getMessage());
        }

        return entity;
    }

    /**
     * Creates a copy of the specified entity.
     * If the entity has a name, a different name is automatically assigned to the copy.
     * @param other An entity.
     * @return The copy created.
     * @throws QuestEditorException If the entity could not be copied.
     */
    public static MapEntity createCopy(MapEntity other) throws QuestEditorException {

        MapEntity entity = null;
        try {
            entity = MapEntity.create(
                    other.getMap(),
                    other.getType(),
                    other.getType().getDefaultSubtype());

            Rectangle otherPositionInMap = other.getPositionInMap();

            entity.setLayer(other.getLayer());
            entity.positionInMap.x = otherPositionInMap.x; // for now the origin is (0,0)
            entity.positionInMap.y = otherPositionInMap.y;

            if (other.hasName()) {
                entity.setName(other.getName());
            }

            if (other.hasDirectionProperty()) {
                entity.setDirection(other.getDirection());
            }

            if (other.hasSubtype()) {
                entity.setSubtype(other.getSubtype());
            }

            // specific properties
            entity.setProperties(other.getProperties());

            if (other.isSizeVariable()) {
                entity.setSize(other.getWidth(), other.getHeight()); // some entities need to know their properties before they can be resized
            }

            entity.updateImageDescription();

            return entity;
        }
        catch (QuestEditorException ex) {
            ex.printStackTrace();
            throw new QuestEditorException("Failed to copy entity '" + entity + "': " + ex.getMessage());
        }
    }

    /**
     * Sets all attributes of this entity from a Lua table.
     * @param table A Lua table containing the properties of the entity.
     * @throws MapException If the attributes are invalid.
     */
    public void loadFromLua(LuaTable table) throws MapException {

        Layer layer = Layer.get(table.get("layer").checkint());
        int x = table.get("x").checkint();
        int y = table.get("y").checkint();
        String name = table.get("name").optjstring(null);

        if (name != null) {
            setName(name);
        }
        setLayer(layer);
        positionInMap.x = x;
        positionInMap.y = y; // For now the origin is (0,0).

        int width = 0;
        int height = 0;
        if (isSizeVariable()) {
            width = table.get("width").checkint();
            height = table.get("height").checkint();
        }

        if (hasDirectionProperty()) {
            int direction = -1;
            if (canHaveNoDirection()) {
                direction = table.get("direction").optint(-1);
            }
            else {
                direction = table.get("direction").checkint();
            }
            setDirection(direction);
        }

        if (hasSubtype()) {
            String subtype = table.get("subtype").checkjstring();
            setSubtype(getSubtype(subtype));
        }

        // specific properties
        for (String key: specificProperties.keySet()) {
            try {
                LuaValue value = table.get(key);
                if (value.isint() || value.isstring()) {
                    setStringProperty(key, value.tojstring());
                }
                else if (value.isboolean()) {
                    setBooleanProperty(key, value.toboolean());
                }
            }
            catch (LuaError ex) {
                throw new LuaError("Failed to read property '" + key + "' for an entity of type " + getType().getHumanName());
            }
        }

        if (isSizeVariable()) {
            // Some entities need to know their properties before they can be resized.
            setSizeUnchecked(width, height);
        }

        updateImageDescription();

        // Now the origin is valid.
        setPositionInMapUnchecked(positionInMap.x, positionInMap.y);
    }

    /**
     * @brief Saves this entity as Lua data to a text file.
     * @param out The text file to write.
     */
    public void saveAsLua(PrintWriter out) {

        // Entity type.
        out.println(getType().getLuaName() + "{");

        // Position in the map.
        out.println("  layer = " + getLayer().getId() + ",");
        out.println("  x = " + getX() + ",");
        out.println("  y = " + getY() + ",");
        if (isSizeVariable()) {
          out.println("  width = " + getWidth() + ",");
          out.println("  height = " + getHeight() + ",");
        }

        // Entity id.
        if (hasName()) {
          out.println("  name = \"" + getName() + "\",");
        }

        // Direction.
        if (hasDirectionProperty()) {
          out.println("  direction = " + getDirection() + ",");
        }

        // Subtype.
        if (hasSubtype()) {
            out.println("  subtype = \"" + getSubtypeId() + "\",");
        }

        // Properties specific to this type of entity.
        for (String key: specificProperties.keySet()) {

            String stringValue = getStringProperty(key);
            if (stringValue == null) {
                // Property not set.
                continue;
            }

            if (optionalPropertiesDefaultValues.containsKey(key)) {
                // This property is optional: see if it differs from the
                // default value.
                String defaultStringValue = optionalPropertiesDefaultValues.get(key);
                if (defaultStringValue != null && stringValue.equals(defaultStringValue)) {
                    // No need to write the value.
                    continue;
                }
            }

            Class<?> propertyType = propertyTypes.get(key);
            String value = null;
            if (propertyType == String.class) {
                value = '"' + stringValue + '"';
            }
            else if (propertyType == Integer.class) {
                value = stringValue;
            }
            else if (propertyType == Boolean.class) {
                value = stringValue.equals("1") ? "true" : "false";
            }
            else {
                throw new IllegalStateException("Unknown property type for key '" + key + "'");
            }
            out.println("  " + key + " = " + value + ",");
        }

        out.println("}");
        out.println();
    }

    /**
     * Returns the current map of this entity.
     */
    public Map getMap() {
        return map;
    }

    /**
     * Changes the map of this entity.
     * @param map The new map.
     * @param QuestEditorException if this entity cannot exist in the new map.
     */
    public void setMap(Map map) throws QuestEditorException {

        Map oldMap = this.map;
        Tileset oldTileset = oldMap != null ? oldMap.getTileset() : null;
        this.map = map;
        notifyMapChanged(oldMap, map);
        notifyTilesetChanged(oldTileset, map.getTileset());
    }

    /**
     * Returns the tileset of the current map of this entity.
     * @return The tileset or null.
     */
    public Tileset getTileset() {
        if (map == null) {
            return null;
        }
        return map.getTileset();
    }

    /**
     * Returns whether the entity is valid.
     * An invalid entity can be loaded but must be fixed before it is saved.
     * @return true if the entity is valid.
     */
    public final boolean isValid() {

        try {
            checkValidity();
            return true;
        }
        catch (MapException ex) {
            return false;
        }
    }

    /**
     * Checks the entity's validity.
     * An invalid entity can be loaded but must be fixed before it is saved.
     * @throws MapException If the entity is not valid.
     */
    public final void checkValidity() throws MapException {

        // Check the common properties.
        checkPositionTopLeft(positionInMap.x, positionInMap.y);

        if (isResizable()) {
            checkSize(positionInMap.width, positionInMap.height);
        }

        // Check the specific properties.
        checkProperties();
    }

    /**
     * Returns whether the entity sets a specific layer when it is created on
     * a map.
     * If you return false, when creating the entity and adding it to a map,
     * the entity will be placed at the highest existing layer under the cursor.
     * @return true if the entity sets an initial layer when it is created
     */
    public boolean hasInitialLayer() {
        return false;
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
     * Changes the position of the entity on the map, by specifying its rectangle.
     * @param position a rectangle
     */
    public void setPositionInMapUnchecked(Rectangle position) {

        setPositionTopLeftUnchecked(position.x, position.y);
        setSizeUnchecked(position.width, position.height);
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
     * Changes the position of the entity on the map, by specifying the
     * coordinates of its origin point.
     * The size of the entity is not changed.
     * @param x x coordinate of the origin point.
     * @param y y coordinate of the origin point.
     * @throws MapException if the coordinates of the top-left corner are not divisible by 8
     */
    public void setPositionInMap(int x, int y) throws MapException {

        // calculate the new coordinates of the top-left corner
        Point origin = getOrigin();
        setPositionTopLeft(x - origin.x, y - origin.y);
    }

    /**
     * Changes the position of the entity on the map, by specifying the
     * coordinates of its origin point.
     * The size of the entity is not changed.
     * Unchecked version: use checkPositionTopLeft() later to verify if the
     * position is correct.
     * @param x x coordinate of the origin point.
     * @param y y coordinate of the origin point.
     * @throws MapException if the coordinates of the top-left corner are not divisible by 8
     */
    public void setPositionInMapUnchecked(int x, int y) {

        // calculate the new coordinates of the top-left corner
        Point origin = getOrigin();
        setPositionTopLeftUnchecked(x - origin.x, y - origin.y);
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
            throw new MapException("Wrong upper-left corner x value (" + x + "): coordinates must be aligned to the grid (divisible by 8)");
        }

        if (y % 8 != 0) {
            throw new MapException("Wrong upper-left corner y value (" + y + "): coordinates must be aligned to the grid (divisible by 8)");
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
        setPositionTopLeftUnchecked(x, y);

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
    protected void setPositionTopLeftUnchecked(int x, int y) {
        positionInMap.x = x;
        positionInMap.y = y;
    }

    /**
     * Returns whether the top-left corner of this entity is aligned to the
     * 8*8 grid.
     */
    public boolean isAlignedToGrid() {
        return positionInMap.x % 8 == 0 && positionInMap.y % 8 == 0;
    }

    /**
     * Makes sure the top-left corner of this entity is aligned to the 8*8 grid.
     */
    public void setAlignedToGrid() {
        int x = positionInMap.x + 4;
        if (x < 0) {
            x -= 8;
        }
        positionInMap.x = x - x % 8;

        int y = positionInMap.y + 4;
        if (y < 0) {
            y -= 8;
        }
        positionInMap.y = y - y % 8;

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
        setSizeUnchecked(width, height);

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
     * Unchecked version: use checkSize() later to verify if the
     * size is correct.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     */
    public void setSizeUnchecked(int width, int height) {
        positionInMap.width = width;
        positionInMap.height = height;
    }

    /**
     * Changes the size of the entity.
     * Unchecked version: use checkSize() later to verify if the
     * size is correct.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     */
    public void setSizeUnchecked(Dimension size) {

        setSizeUnchecked(size.width, size.height);
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
     * Returns whether this kind of entity is allowed to have a name.
     *
     * This function returns true by default.
     *
     * @return true if the entity can have a name.
     */
    public boolean canHaveName() {
        return true;
    }

    /**
     * Returns whether the entity has a name.
     * @return true if the entity has a name, false otherwise.
     */
    public final boolean hasName() {
        return name != null;
    }

    /**
     * Returns the name of the entity if any.
     * @return the entity's name, or null if this entity has no name.
     */
    public final String getName() {
        return name;
    }

    /**
     * Changes the name of the entity.
     * If the name specified is already used, another name is automatically set.
     * @param name the new entity's name (possibly null).
     * @throws MapException if this name is not valid
     */
    public final void setName(String name) throws MapException {

        if (name == null) {
            this.name = null;
            return;
        }

        if (name.indexOf(' ') != -1 || name.indexOf('\t') != -1) {
            throw new MapException("The entity name cannot have whitespaces");
        }

        MapEntity other = map.getEntityWithName(name);
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
            while (map.getEntityWithName(prefix.toString() + counter) != null) {
                counter++;
            }
            name = prefix.toString() + counter;
        }

        // debug
        other = map.getEntityWithName(name);
        if (other != null && other != this) {
            throw new MapException("An entity with this name already exists and I was unable to compute a new one");
        }

        this.name = name;
        setChanged();
        notifyObservers();
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
     * Returns the sprite of this entity if any.
     * @return The sprite or null.
     */
    public Sprite getSprite() {
        return sprite;
    }

    /**
     * Sets the sprite of this entity.
     *
     * If you set a sprite, it will be used to draw the entity in the editor
     * instead of currentImageDescription.
     *
     * @param sprite The sprite or null.
     */
    public void setSprite(Sprite sprite) {
        this.sprite = sprite;
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
     * This method draws the entity's image as described by the sprite or
     * the currentImageDescription field,
     * which you can modify by redefining the updateImageDescription() method.
     * To draw a more complex image, redefine the paint() method.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

        if (sprite != null && !sprite.getDefaultAnimationName().isEmpty()) {
            // There is a sprite, display it.
            int direction = getDirection();
            int numDirections = sprite.getAnimation(sprite.getDefaultAnimationName()).getNbDirections();
            if (direction < 0 || direction >= numDirections) {
                // The direction property of an entity may be independent from
                // the one of its sprite, so being out of the range is not an error.
                direction = 0;
            }
            sprite.paint(g, zoom, showTransparency, getX(), getY(), null, direction, 0);
        }
        else {
            // Use the built-in image description of the editor.
            currentImageDescription.paint(g, zoom, showTransparency, positionInMap);
        }
    }

    /**
     * Draws a border around a rectangle.
     * @param g the graphic context
     * @param zoom zoom of the image
     * @param outlineColor a color to fill the area between the two lines of
     * the border.
     * @param rectangle The region to draw a border on, in map coordinates.
     */
    protected void drawRectangleOutline(Graphics g, double zoom,
            Color outlineColor, Rectangle region) {

        int dx1 = (int) (region.x * zoom);
        int dy1 = (int) (region.y * zoom);
        int dx2 = (int) (dx1 + region.width * zoom);
        int dy2 = (int) (dy1 + region.height * zoom);

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
     * Draws a rectangle border on the entity.
     * This function can be called by the paint() method of suclasses that need
     * to draw the entity in a complex way (i.e. not just by blitting an image file).
     * @param g the graphic context
     * @param zoom zoom of the image
     * @param outlineColor a color to fill the area between the two lines of the border
     */
    protected void drawEntityOutline(Graphics g, double zoom, Color outlineColor) {

        drawRectangleOutline(g, zoom, outlineColor, positionInMap);
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
    public final String getSubtypeId() {

        if (subtype == null) {
            return null;
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
     * @param id The subtype id.
     * @return The subtype.
     */
    public final EntitySubtype getSubtype(String id) {
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
     * @param subtype The subtype id.
     * @throws MapException If the subtype is not valid.
     */
    public final void setSubtype(String subtype) throws MapException {
        setSubtype(getSubtype(subtype));
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype The subtype to set.
     * @throws MapException If the subtype is not valid.
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {
        this.subtype = subtype;
        setChanged();
        notifyObservers();
    }

    /**
     * Returns whether a value specific to the current entity type is defined.
     * @param name name of the property to look for
     * @return true if this property exists.
     */
    public final boolean hasProperty(String name) {
        return propertyTypes.containsKey(name);
    }

    /**
     * Returns a string value specific to the current entity type.
     * @param name Name of the property to get.
     * @param value Value of the property, possibly null.
     */
    public final String getStringProperty(String name) {

        if (!hasProperty(name)) {
            throw new IllegalArgumentException("There is no property with name '" + name +
                    "' for entity " + getType().getHumanName());
        }
        String value = specificProperties.get(name);

        if (value == null) {
            value = optionalPropertiesDefaultValues.get(name);
        }

        return value;
    }

    /**
     * Returns a integer value specific to the current entity type.
     * @param name Name of the property to get.
     * @param value Value of the property, possibly null.
     */
    public final Integer getIntegerProperty(String name) {

        String value = getStringProperty(name);
        return value == null ? null : Integer.parseInt(value);
    }

    /**
     * Returns a boolean value specific to the current entity type.
     * @param name Name of the property to get.
     * @param value Value of the property, possibly null.
     */
    public final Boolean getBooleanProperty(String name) {

        String value = getStringProperty(name);
        return value == null ? null : Integer.parseInt(value) != 0;
    }

    /**
     * Sets a property specific to the current entity type.
     * @param name Name of the property.
     * @param value Value of the property, possibly null.
     */
    public void setStringProperty(String name, String value) throws MapException {

        if (!hasProperty(name)) {
            throw new IllegalArgumentException(
                    "There is no property with name '" + name +
                    "' for entity " + getType().getHumanName());
        }

        specificProperties.put(name, value);
        notifyPropertyChanged(name, value);
    }

    /**
     * Sets a property specific to the current entity type.
     * @param name Name of the property.
     * @param value Value of the property, possibly null.
     */
    public final void setIntegerProperty(String name, Integer value) throws MapException {

        if (!hasProperty(name)) {
            throw new IllegalArgumentException(
                    "There is no property with name '" + name +
                    "' for entity " + getType().getHumanName());
        }

        String stringValue = null;
        if (value != null) {
            stringValue = Integer.toString(value);
        }
        specificProperties.put(name, stringValue);
        notifyPropertyChanged(name, stringValue);
    }

    /**
     * Sets a property specific to the current entity type.
     * @param name Name of the property.
     * @param value Value of the property, possibly null.
     */
    public final void setBooleanProperty(String name, Boolean value) throws MapException {

        if (!hasProperty(name)) {
            throw new IllegalArgumentException(
                    "There is no property with name '" + name +
                    "' for entity " + getType().getHumanName());
        }

        String stringValue = null;
        if (value != null) {
            stringValue = value ? "1" : "0";
        }
        specificProperties.put(name, stringValue);
        notifyPropertyChanged(name, stringValue);
    }

    /**
     * Defines a new property of type String.
     * @param name Name of the property to create.
     * @param optional Indicates that the property is optional.
     * @param initialValue Initial value of the property (will also be stored
     * as default if optional is true).
     */
    protected void createStringProperty(String name, 
            boolean optional, String initialValue) throws MapException {

        if (hasProperty(name)) {
            throw new IllegalArgumentException(
                    "A property with name '" + name +
                    "' already exists for entity " + getType().getHumanName());
        }

        propertyTypes.put(name, String.class);

        if (optional) {
            optionalPropertiesDefaultValues.put(name, initialValue);
        }

        setStringProperty(name, initialValue);
    }

    /**
     * Defines a new property of type Integer.
     * @param name Name of the property to create.
     * @param optional Indicates that the property is optional.
     * @param initialValue Initial value of the property (will also be stored
     * as default if optional is true).
     */
    protected void createIntegerProperty(String name, 
            boolean optional, Integer initialValue) throws MapException {

        if (hasProperty(name)) {
            throw new IllegalArgumentException(
                    "A property with name '" + name +
                    "' already exists for entity " + getType().getHumanName());
        }

        propertyTypes.put(name, Integer.class);

        String stringValue = null;
        if (initialValue != null) {
            stringValue = Integer.toString(initialValue);
        }

        if (optional) {
            optionalPropertiesDefaultValues.put(name, stringValue);
        }

        setStringProperty(name, stringValue);
    }

    /**
     * Defines a new property of type Boolean.
     * @param name Name of the property to create.
     * @param optional Indicates that the property is optional.
     * @param initialValue Initial value of the property (will also be stored
     * as default if optional is true).
     */
    protected void createBooleanProperty(String name, 
            boolean optional, Boolean initialValue) throws MapException {

        if (hasProperty(name)) {
            throw new IllegalArgumentException(
                    "A property with name '" + name +
                    "' already exists for entity " + getType().getHumanName());
        }

        propertyTypes.put(name, Boolean.class);

        String stringValue = null;
        if (initialValue != null) {
            stringValue = initialValue ? "1" : "0";
        }

        if (optional) {
            optionalPropertiesDefaultValues.put(name, stringValue);
        }

        setStringProperty(name, stringValue);
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
     * @param properties The specific properties to copy.
     */
    public final void setProperties(LinkedHashMap<String, String> properties) throws MapException {

        for (java.util.Map.Entry<String, String> entry: properties.entrySet()) {
            setStringProperty(entry.getKey(), entry.getValue());
        }
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     * Redefine this method to define the default value of your properties.
     */
    public void createProperties() throws MapException {
    }

    /**
     * Notifies this entity that a property specific to its type has just changed.
     * Does nothing by default.
     * @param name Name of the property that has changed.
     * @param value The new value.
     */
    protected void notifyPropertyChanged(String name, String value) throws MapException {
    }

    /**
     * Checks the specific properties.
     * Redefine this method to check the values.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

    }

    /**
     * Check that properties "treasure_name", "treasure_variant" and
     * "treasure_savegame_variable" are valid.
     *
     * Call this function from your implementation of checkProperties()
     * if your entity has a treasure.
     *
     * @throws MapException If the treasure is not valid.
     */
    public void checkTreasureProperties() throws MapException {

        String treasureName = getStringProperty("treasure_name");
        if (treasureName != null &&
                !Project.getResource(ResourceType.ITEM).exists(treasureName)) {
            throw new MapException("No such item: '" + treasureName + "'");
        }

        Integer variant = getIntegerProperty("treasure_variant");
        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String savegameVariable = getStringProperty("treasure_savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid treasure savegame variable");
        }
    }

    /**
     * @brief Returns whether a string is a valid savegame variable,
     * that is, a valid Lua identifier.
     * @param id The savegame variable to check.
     * @return true if this is a legal Lua identifier.
     */
    public static boolean isValidSavegameVariable(String id) {

        if (id.isEmpty() || Character.isDigit(id.charAt(0))) {
            return false;
        }

        for (int i = 0; i < id.length(); i++) {
            char c = id.charAt(i);
            if (!Character.isLetterOrDigit(c) && c != '_') {
                return false;
            }
        }

        return true;
    }

    /**
     * Returns whether the specified string is an existing sprite name,
     * @param spriteName A sprite name.
     * @return true if a sprite exists with this name.
     */
    public static boolean isValidSpriteName(String spriteName) {

        return spriteName != null &&
          Project.getResource(ResourceType.SPRITE).exists(spriteName);
    }

    /**
     * Notifies this entity that it now belongs to another map.
     * By default, nothing is done.
     * @param oldMap The previous map or null.
     * @param newMap The new map.
     * @throws MapException If this entity is invalid in the new map.
     */
    public void notifyMapChanged(Map oldMap, Map newMap) throws MapException {
    }

    /**
     * Notifies this entity the tileset has changed.
     * By default, the sprite (if any) is notified.
     * @param oldTileset The previous tileset or null.
     * @param tileset The new tileset.
     * @throws MapException If this entity is invalid with the new tileset.
     */
    public void notifyTilesetChanged(Tileset oldTileset, Tileset newTileset) throws MapException {

        if (sprite != null) {
            try {
                sprite.notifyTilesetChanged(newTileset.getId());
            } catch (SpriteException ex) {
                throw new MapException(ex.getMessage());
            }
        }
    }
}

