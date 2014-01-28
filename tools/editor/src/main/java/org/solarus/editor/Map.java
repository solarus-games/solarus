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
package org.solarus.editor;

import java.util.*;
import java.io.*;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Rectangle;
import org.luaj.vm2.*;
import org.luaj.vm2.lib.*;
import org.luaj.vm2.compiler.*;
import org.solarus.editor.entities.*;

/**
 * This class describes a map.
 * A map is observable.
 */
public class Map extends Observable {

    // map properties

    /**
     * Id of the map.
     */
    private String mapId;

    /**
     * Size of the map, in pixels (the width and the height
     * must be multiples of 8).
     */
    private Dimension size;

    /**
     * Id of the background music: name of a music file, Music.noneId or Music.unchangedId.
     */
    private String musicId;

    /**
     * Id of the tileset of the map (or an empty string if no tileset is set).
     */
    private String tilesetId;

    /**
     * Tileset of the map.
     * The tileset is the set of small images (tiles) used to build the map.
     */
    private Tileset tileset;

    /**
     * A name identifying the world where this map, or null.
     * This can be used to link maps together.
     */
    private String world;

    /**
     * Floor of this map, or null if there is no floor.
     */
    private Integer floor;

    /**
     * Location of this map in its floor or in its world.
     */
    private Point location;

    // content of the map

    /**
     * Entities of the map.
     * This is an array of three entity lists, one for each layer.
     */
    private MapEntities[] allEntities;

    /**
     * The entities selected.
     */
    private MapEntitySelection entitySelection;

    /**
     * True if some tiles could not be found in the tileset
     * when the tileset was loaded.
     */
    private boolean badTiles;

    /**
     * The history of the actions made by the user on the map.
     */
    private MapEditorHistory history;

    /**
     * View settings of this map (e.g. what entities are shown).
     */
    private MapViewSettings viewSettings;

    /**
     * Minimum width of a map in pixels.
     */
    public static final int MINIMUM_WIDTH = 320;

    /**
     * Minimum height of a map in pixels.
     */
    public static final int MINIMUM_HEIGHT = 240;

    /**
     * Creates or loads a map.
     * @param mapId Id of the map to create (may be a new map or an existing one).
     * @throws QuestEditorException if the map could not be loaded
     */
    public Map(String mapId) throws QuestEditorException {

        if (!isValidId(mapId)) {
            throw new MapException("Invalid map ID: '" + mapId + "'");
        }

        this.size = new Dimension(MINIMUM_WIDTH, MINIMUM_HEIGHT);
        this.location = new Point(0, 0);
        this.tileset = null;
        this.tilesetId = "";
        this.musicId = Music.unchangedId;
        this.world = null;
        this.floor = null;
        this.mapId = mapId;
        initialize();

        load();
    }

    /**
     * Initializes the object.
     */
    private void initialize() {
        this.allEntities = new MapEntities[3];
        for (Layer layer: Layer.values()) {
            this.allEntities[layer.getId()] = new MapEntities();
        }

        this.entitySelection = new MapEntitySelection(this);
        this.history = new MapEditorHistory();
        this.viewSettings = new MapViewSettings(this);
    }

    /**
     * Delete the files associated to a map
     * @param id Id of the map to delete.
     */
    public static void delete(String id) throws QuestEditorException {
        File mapFile = Project.getMapFile(id);
        if (!mapFile.delete()) {
            throw new QuestEditorException("Can't remove the file " + mapFile.getAbsolutePath());
        }
        File mapScriptFile = Project.getMapScriptFile(id);
        if (!mapScriptFile.delete()) {
            throw new QuestEditorException("Can't remove the file " + mapScriptFile.getAbsolutePath());            
        }
    }
    
    /**
     * Returns the id of the map.
     * @return the id of the map
     */
    public String getId() {
        return mapId;
    }

    /**
     * @brief Returns whether a string is a valid map id.
     * @param mapId The id to check.
     * @return true if this is legal.
     */
    public static boolean isValidId(String mapId) {

        if (mapId.isEmpty()) {
            return false;
        }

        for (int i = 0; i < mapId.length(); i++) {
            char c = mapId.charAt(i);
            if (!Character.isLetterOrDigit(c) && c != '_') {
                return false;
            }
        }

        return true;
    }

    /**
     * Returns a string representation of this tileset.
     * @return The name of the tileset.
     */
    @Override
    public String toString() {
        return getName();
    }

    /**
     * Returns the map name.
     * @return The human name of the map.
     */
    public String getName() {
        Resource mapResource = Project.getResource(ResourceType.MAP);
        String name = mapId;
        
        try {
            name = mapResource.getElementName(mapId);
        }
        catch (QuestEditorException ex) {
            // Cannot happen: the map id must be valid.
            ex.printStackTrace();
        }
        return name;
    }

    /**
     * Changes the name of the map.
     * @param name New human-readable name of the map.
     */
    public void setName(String name) throws QuestEditorException {

        if (!name.equals(getName())) {
            Project.renameResourceElement(ResourceType.MAP, mapId, name);
        }
    }

    /**
     * Returns the history of actions of the map.
     * @return the map history
     */
    public MapEditorHistory getHistory() {
        return history;
    }

    /**
     * Returns the view settings of this map.
     * @return The view settings.
     */
    public MapViewSettings getViewSettings() {
        return viewSettings;
    }

    /**
     * Returns the map size.
     * @return the map size (in pixels)
     */
    public Dimension getSize() {
        return size;
    }

    /**
     * Returns the map width.
     * @return the map width (in pixels)
     */
    public int getWidth() {
        return size.width;
    }

    /**
     * Returns the map height.
     * @return the map height (in pixels)
     */
    public int getHeight() {
        return size.height;
    }

    /**
     * Changes the map size.
     * If the new size is lower than the old one, the tiles in the removed area
     * are not destroyed. The minimum size of a map is 320*240.
     * The width and the height must be multiples of 8.
     * @param size the new map size (in pixels)
     * @throws MapException if the width or the height is incorrect
     */
    public void setSize(Dimension size) throws MapException {
        if (size.width < MINIMUM_WIDTH || size.height < MINIMUM_HEIGHT) {
            throw new RuntimeException("The minimum size of a map is " +
                    MINIMUM_WIDTH + '*' + MINIMUM_HEIGHT + '.');
        }

        if (size.width < MINIMUM_WIDTH || size.height < MINIMUM_HEIGHT) {
            throw new MapException("The minimum size of a map is " +
                    MINIMUM_WIDTH + '*' + MINIMUM_HEIGHT + '.');
        }

        if (size.width % 8 != 0 || size.height % 8 != 0) {
            throw new MapException("The width and the height of the map must be multiples of 8.");
        }

        this.size = size;

        setChanged();
        notifyObservers();
    }

    /**
     * Returns the tileset associated to this map.
     * The tileset is the set of small images (tiles) used to build the map.
     * @return the tileset (null if no tileset is set)
     */
    public Tileset getTileset() {
        return tileset;
    }

    /**
     * Returns the id of the tileset associated to this map.
     * @return the tileset id (or an empty string if no tileset is set)
     */
    public String getTilesetId() {
        return tilesetId;
    }

    /**
     * Changes the tileset of the map.
     * @param tilesetId id of the new tileset, or an empty string to set no tileset
     * @return true if the tileset was loaded successfuly, false if some tiles could
     * not be loaded in this tileset
     * @throws QuestEditorException if this tileset could not be applied
     */
    public boolean setTileset(String tilesetId) throws QuestEditorException {

        this.badTiles = false;

        // if the tileset is removed
        if (tilesetId.length() == 0 && this.tilesetId.length() != 0) {

            this.tilesetId = tilesetId;
            this.tileset = null;

            setChanged();
            notifyObservers();
        }

        // if the tileset is changed
        else if (!tilesetId.equals(this.tilesetId)) {

            Tileset newTileset = new Tileset(tilesetId);
            for (Layer layer: Layer.values()) {

                LinkedList<MapEntity> entitiesToRemove = new LinkedList<MapEntity>();
                for (MapEntity entity: allEntities[layer.getId()]) {

                    try {
                        entity.notifyTilesetChanged(this.tileset, newTileset);
                    }
                    catch (NoSuchTilePatternException ex) {
                        // the entity is not valid anymore, we should remove it from the map
                        entitiesToRemove.add(entity);
                        getEntitySelection.unselect(entity);
                        badTiles = true;
                    }
                }

                for (MapEntity entity: entitiesToRemove) {
                    allEntities[layer.getId()].remove(entity);
                }
            }

            this.tileset = newTileset;
            this.tilesetId = tilesetId;

            setChanged();
            notifyObservers(tileset);
        }

        return !badTiles;
    }

    /**
     * Returns the world where this map is.
     * @return The world of this map or null.
     */
    public String getWorld() {
        return world;
    }

    /**
     * Returns whether this map belongs to a world.
     * @return \c true if there is a world.
     */
    public boolean hasWorld() {
        return world != null;
    }

    /**
     * Sets the world where this map is.
     * @param world Name of the world or null.
     */
    public void setWorld(String world) {

        if (world == null && this.world == null) {
            // No change.
            return;
        }

        if (world == null
            || this.world == null
            || !world.equals(this.world)) {
            this.world = world;
            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns the floor of this map.
     * @return the floor or null.
     */
    public Integer getFloor() {
        return floor;
    }

    /**
     * Returns whether this map belongs to a floor.
     * @return true if there is a floor.
     */
    public boolean hasFloor() {
        return floor != null;
    }

    /**
     * Sets the floor of this map.
     * @param floor the floor (null for no floor)
     */
    public void setFloor(Integer floor) throws MapException {

        this.floor = floor;
        setChanged();
        notifyObservers();
    }

    /**
     * Returns the location of this map in its floor or its world.
     * @return the location of this map
     */
    public Point getLocation() {
        return location;
    }

    /**
     * Sets the location of this map in its floor or its world.
     * @param location the location of this map
     */
    public void setLocation(Point location) {
        this.location = location;
        setChanged();
        notifyObservers();
    }

    /**
     * Returns the total number of entities of the map.
     * @return the total number of entities of the map.
     */
    public int getNbEntities() {

        int nbEntities = 0;

        // count the entities of each layer
        for (Layer layer: Layer.values()) {
            nbEntities += allEntities[layer.getId()].size();
        }

        return nbEntities;
    }

    /**
     * Returns the total number of tiles of the map.
     * @return the total number of tiles of the map.
     */
    public int getNbTiles() {

        int nbTiles = 0;

        // count the tiles of each layer
        for (Layer layer: Layer.values()) {
            nbTiles += allEntities[layer.getId()].getNbTiles();
        }

        return nbTiles;
    }

    /**
     * Returns the total number of active entities of the map.
     * @return the total number of active entities of the map.
     */
    public int getNbDynamicEntities() {

        int nbDynamicEntities = 0;

        // count the dynamic entities of each layer
        for (Layer layer: Layer.values()) {
            nbDynamicEntities += allEntities[layer.getId()].getNbDynamicEntities();
        }

        return nbDynamicEntities;
    }

    /**
     * Returns the entities of the map.
     * @return an array of 3 MapEntity sets: a set for each layer
     */
    public MapEntities[] getAllEntities() {
        return allEntities;
    }

    /**
     * Sets the entities of the map.
     * @param allEntities an array of 3 MapEntity sets: a set for each layer (this array is copied)
     */
    public void setAllEntities(MapEntities[] allEntities) {

        for (Layer layer: Layer.values()) {
            this.allEntities[layer.getId()] = new MapEntities(allEntities[layer.getId()]);
        }

        setChanged();
        notifyObservers();
    }

    /**
     * Returns the entities of the map on a given layer.
     * @param layer the layer
     * @return the entities placed on that layer
     */
    public MapEntities getEntities(Layer layer) {
        return allEntities[layer.getId()];
    }

    /**
     * Finds all entities of a type on the map.
     * @param entityType a type of entity
     * @return the list of the entities of this kind on the map
     */
    public List<MapEntity> getEntitiesOfType(EntityType entityType) {

        List<MapEntity> list = new LinkedList<MapEntity>();
        for (Layer layer: Layer.values()) {
            list.addAll(allEntities[layer.getId()].getEntitiesOfType(entityType));
        }
        return list;
    }

    /**
     * Returns the first entity under a point of the map, in the specified layer.
     * @param layer the layer
     * @param x x of the point
     * @param y y of the point
     * @return the entity found, or null if there is no entity here
     */
    public MapEntity getEntityAt(Layer layer, int x, int y) {

        MapEntities entities = allEntities[layer.getId()];
        ListIterator<MapEntity> iterator = entities.listIterator(entities.size());
        while (iterator.hasPrevious()) {

            MapEntity entity = iterator.previous();
            if (entity.containsPoint(x, y)) {
                return entity;
            }
        }

        return null;
    }

    /**
     * Returns the highest layer where a specified rectangle overlaps an
     * existing entity.
     * @param rectangle a rectangle
     * @return the highest layer where an entity exists in this rectangle,
     * or Layer.LOW if there is nothing here
     */
    public Layer getLayerInRectangle(Rectangle rectangle) {

        Layer[] layers = { Layer.HIGH, Layer.INTERMEDIATE, Layer.LOW };
        for (Layer layer: layers) {
            for (MapEntity entity: allEntities[layer.getId()]) {
                if (rectangle.intersects(entity.getPositionInMap())) {
                    return layer;
                }
            }
        }
        return Layer.LOW;
    }

    /**
     * Returns the entities located in a rectangle defined by two points.
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     */
    public List<MapEntity> getEntitiesInRectangle(int x1, int y1, int x2, int y2) {

        List<MapEntity> entitiesInRectangle = new LinkedList<MapEntity>();

        int x = Math.min(x1, x2);
        int width = Math.abs(x2 - x1);

        int y = Math.min(y1, y2);
        int height = Math.abs(y2 - y1);

        Rectangle rectangle = new Rectangle(x, y, width, height);

        for (Layer layer: Layer.values()) {

            for (MapEntity entity: allEntities[layer.getId()]) {
                if (rectangle.contains(entity.getPositionInMap())) {
                    entitiesInRectangle.add(entity);
                }
            }
        }

        return entitiesInRectangle;
    }

    /**
     * Returns an entity, specifying its name.
     * @param name The name of the entity to get (cannot be null)
     * @return The entity, or null if there is no entity with this name
     */
    public MapEntity getEntityWithName(String name) {

        for (Layer layer: Layer.values()) {
            MapEntity entity = allEntities[layer.getId()].getEntityWithName(name);
            if (entity != null) {
                return entity;
            }
        }

        return null;
    }

    /**
     * Adds an entity on the map.
     * @param entity the entity to add
     */
    public void addEntity(MapEntity entity) {

        allEntities[entity.getLayer().getId()].add(entity);

        setChanged();
        notifyObservers();
    }

    /**
     * Removes an entity from the map.
     * @param entity the entity to remove
     */
    public void removeEntity(MapEntity entity) {

        allEntities[entity.getLayer().getId()].remove(entity);

        setChanged();
        notifyObservers();
    }

    /**
     * Changes the position of an entity on the map, by specifying the coordinates
     * of its origin point.
     * @param entity an entity
     * @param x x coordinate of the origin point
     * @param y y coordinate of the origin point
     * @throws MapException if the coordinates are not multiple of 8
     */
    public void setEntityPosition(MapEntity entity, int x, int y) throws MapException {
        entity.setPositionInMap(x, y);
        entity.updateImageDescription();

        setChanged();
        notifyObservers();
    }

    /**
     * Changes the position of an entity on the map, by specifying the coordinates
     * of its origin point.
     * @param entity an entity
     * @param x x coordinate of the origin point
     * @param y y coordinate of the origin point
     * @throws MapException if the coordinates are not multiple of 8
     */
    public void setEntityPositionUnchecked(MapEntity entity, int x, int y) {
        entity.setPositionInMapUnchecked(x, y);
        entity.updateImageDescription();

        setChanged();
        notifyObservers();
    }

    /**
     * Changes the position of an entity on the map, by specifying two points.
     * The entity is resized so that
     * it fits exactly in the rectangle formed by the two points.
     * @param entity an entity
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     * @throws MapException if the entity is not resizable of the rectangle width
     * or its height is zero or the coordinates or the coordinates are not multiple of 8
     */
    public void setEntityPosition(MapEntity entity, int x1, int y1, int x2, int y2) throws MapException {
        entity.setPositionInMap(x1, y1, x2, y2);

        setChanged();
        notifyObservers();
    }

    /**
     * Changes the position of an entity on the map, by specifying its rectangle.
     * The entity is resized so that it fits exactly in the rectangle.
     * @param entity an entity
     * @param position a rectangle
     * @throws MapException if the entity is not resizable of the rectangle width
     * or its height is zero
     */
    public void setEntityPosition(MapEntity entity, Rectangle position) throws MapException {
        entity.setPositionInMap(position);

        setChanged();
        notifyObservers();
    }

    /**
     * Changes the size of an entity on the map.
     * @param entity an entity
     * @param width the new width
     * @param height the new height
     * @throws MapException if the entity is not resizable or the size
     * specified is lower than or equal to zero
     * or the size specified is not divisible by 8
     */
    public void setEntitySize(MapEntity entity, int width, int height) throws MapException {
        entity.setSize(width, height);

        setChanged();
        notifyObservers();
    }

    /**
     * Changes the position of a group of entities.
     * @param entities the entities to move
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     * @throws MapException if the coordinates of an entity are not multiple of 8
     */
    public void moveEntities(Collection<MapEntity> entities, int dx, int dy) throws MapException {

        for (MapEntity entity: entities) {
            entity.setAlignedToGrid();
            entity.move(dx, dy);
        }

        setChanged();
        notifyObservers();
    }

    /**
     * Changes the direction of an entity.
     * @param entity the entity to change the direction
     * @param direction the new direction
     * @throws MapException if this entity has no direction
     */
    public void setEntityDirection(MapEntity entity, int direction) throws MapException {

        int oldDirection = entity.getDirection();

        if (direction != oldDirection) {

            entity.setDirection(direction);
            entity.updateImageDescription();

            setChanged();
            notifyObservers();
        }
    }

    /**
     * Changes the layer of an entity. You should call this method instead of
     * calling directly MapEntity.setLayer(), because the entity of the 3
     * layers are stored in 3 different structures.
     * If the entity is not known by the map (yet), this method just
     * calls MapEntity.setLayer().
     * @param entity the entity to change the layer
     * @param layer the new layer
     */
    public void setEntityLayer(MapEntity entity, Layer layer) {

        Layer oldLayer = entity.getLayer();

        if (layer != oldLayer) {
            entity.setLayer(layer);

            if (allEntities[oldLayer.getId()].remove(entity)) {
                allEntities[layer.getId()].add(entity);
            }

            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns the specified entities, sorted in the order of the map.
     * The first entity is the lowest one, the last entity is the highest one.
     * @param entities The entities to sort.
     * @return The same entities, sorted as they are in the map.
     */
    public List<MapEntity> getSortedEntities(Collection<MapEntity> entities) {

        List<MapEntity> sortedEntities = new ArrayList<MapEntity>();

        // sort the entities so that they have the same order as in the map
        for (Layer layer: Layer.values()) {

            for (MapEntity entity: allEntities[layer.getId()]) {

                if (entities.contains(entity)) {
                    sortedEntities.add(entity);
                }
            }
        }

        // now sortedEntities contains all entities of the list,
        // sorted in the same order as in the map
        return sortedEntities;
    }

    /**
     * Brings the specified entities to the back, keeping their layer.
     * The order of the specified entities in the map is unchanged.
     * @param entities the entities to move
     */
    public void bringToBack(Collection<MapEntity> entities) {

        List<MapEntity> sortedEntities = getSortedEntities(entities);

        // bring to back each entity from sortedEntities
        ListIterator<MapEntity> iterator = sortedEntities.listIterator(sortedEntities.size());
        while (iterator.hasPrevious()) {

            MapEntity entity = iterator.previous();
            Layer layer = entity.getLayer();
            allEntities[layer.getId()].remove(entity);
            allEntities[layer.getId()].addFirst(entity);
        }

        setChanged();
        notifyObservers();
    }

    /**
     * Brings the specified entities to the front, keeping their layer.
     * The order of the specified entities in the map is unchanged.
     * @param entities the entities to move
     */
    public void bringToFront(Collection<MapEntity> entities) {

        List<MapEntity> sortedEntities = getSortedEntities(entities);

        // bring to front each entity from sortedEntities
        ListIterator<MapEntity> iterator = sortedEntities.listIterator(0);
        while (iterator.hasNext()) {

            MapEntity entity = iterator.next();
            Layer layer = entity.getLayer();
            allEntities[layer.getId()].remove(entity);
            allEntities[layer.getId()].addLast(entity);
        }

        setChanged();
        notifyObservers();
    }

    /**
     * Returns whether this map has a background music.
     * @return true if the music of this map is different from Music.noneId.
     */
    public boolean hasMusic() {
        return !musicId.equals(Music.noneId);
    }

    /**
     * Returns the id of the background music of the map.
     * @return the name of the music, i.e. a music file name with the extension,
     * or Music.noneId or Music.unchangedId
     */
    public String getMusic() {
        return musicId;
    }

    /**
     * Changes the default background music of the map.
     * @param musicId the name of the music, i.e. a music file name with the extension,
     * or Music.noneId or Music.unchangedId
     */
    public void setMusic(String musicId) {

        if (!musicId.equals(this.musicId)) {

            this.musicId = musicId;

            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns the entities selected by the user.
     * @return the entities selected by the user
     */
    public MapEntitySelection getEntitySelection() {
        return entitySelection;
    }

    /**
     * Returns whether or not when the tileset was loaded, some tiles of the map refered
     * to non existent tiles in the tileset. These bad tiles were removed.
     * @return true if there was bad tiles when the tiles was loaded, false if all tiles
     * were loaded successfuly.
     */
    public boolean badTiles() {
        return badTiles;
    }

    /**
     * Checks that the map is valid, i.e. that it can be played without risk.
     * @throws MapException if the map is not valid (e.g. no tileset is selected,
     * or some entities are not in a valid state).
     */
    public void checkValidity() throws MapException {

        // check that a tileset is selected
        if (tilesetId.isEmpty()) {
            throw new MapException("No tileset is selected");
        }

        // check that all entities are valid
        for (MapEntities entities: allEntities) {
            for (MapEntity entity: entities) {
                try {
                    entity.checkValidity();
                }
                catch (MapException ex) {
                    throw new InvalidEntityException(ex.getMessage(), entity);
                }
            }
        }
    }

    /**
     * Loads the map from its file.
     * @throws QuestEditorException if the file could not be read
     */
    public void load() throws QuestEditorException {
        try {
            File mapFile = Project.getMapFile(mapId);
            LuaC.install();
            LuaTable environment = LuaValue.tableOf();

            environment.set("properties", new PropertiesFunction());
            for (EntityType entityType: EntityType.values()) {
                environment.set(entityType.getLuaName(), new MapEntityCreationFunction(entityType));
            }

            LuaFunction code = LoadState.load(new FileInputStream(mapFile),
                mapFile.getName(), environment);
            code.call();
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
        catch (LuaError ex) {
            if (ex.getCause() != null) {
                throw new QuestEditorException(ex.getCause().getMessage());
            }
            else {
                throw new QuestEditorException(ex.getMessage());
            }
        }

        history.setSaved();
        setChanged();
        notifyObservers();
    }

    /**
     * Saves the map into its file.
     * @throws QuestEditorException if the file could not be written for various reasons
     */
    public void save() throws QuestEditorException {

        // check that the map is valid
        checkValidity();
        
        try {
            // Open the map file in writing.
            File mapFile = Project.getMapFile(mapId);
            PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(mapFile)));

            // Print the map general info.
            out.println("properties{");
            out.println("  x = " + getLocation().x + ",");
            out.println("  y = " + getLocation().y + ",");
            out.println("  width = " + getWidth() + ",");
            out.println("  height = " + getHeight() + ",");
            if (hasWorld()) {
                out.println("  world = \"" + getWorld() + "\",");
            }
            if (hasFloor()) {
                out.println("  floor = " + getFloor() + ",");
            }
            out.println("  tileset = \"" + getTilesetId() + "\",");
            if (hasMusic()) {
                out.println("  music = \"" + getMusic() + "\",");
            }
            out.println("}");
            out.println();

            for (Layer layer: Layer.values()) {

                MapEntities entities = allEntities[layer.getId()];

                // Print the entities.
                for (MapEntity entity: entities) {
                    entity.saveAsLua(out);
                }
            }

            out.close();

            history.setSaved();
        }
        catch (IOException ex) {
            throw new MapException(ex.getMessage());
        }
    }
    
    /**
     * @brief Lua function properties() called by the map data file.
     */
    private class PropertiesFunction extends OneArgFunction {

        public LuaValue call(LuaValue arg) {

            try {
                LuaTable table = arg.checktable();
    
                int x = table.get("x").checkint();
                int y = table.get("y").checkint();
                int width = table.get("width").checkint();
                int height = table.get("height").checkint();
                String world = table.get("world").optjstring(null);
                Integer floor = null;
                if (!table.get("floor").isnil()) {
                  floor = table.get("floor").checkint();
                }
                String tilesetId = table.get("tileset").checkjstring();
                String musicId = table.get("music").optjstring(Music.noneId);

                setSize(new Dimension(width, height));
                setWorld(world);
                setFloor(floor);
                setLocation(new Point(x, y));
                setTileset(tilesetId);
                setMusic(musicId);
    
                return LuaValue.NIL;
            }
            catch (QuestEditorException ex) {
                // Error in the input file.
                throw new LuaError(ex);
            }
            catch (Exception ex) {
                // Error in the editor.
                ex.printStackTrace();
                throw new LuaError(ex);
            }
        }
    }

    /**
     * @brief Lua function called by the map data file to define a map entity.
     */
    private class MapEntityCreationFunction extends OneArgFunction {

        private EntityType type;

        /**
         * @brief Constructor for a specified type of entity.
         * @param type The type of entity to create.
         */
        public MapEntityCreationFunction(EntityType type) {
            super();
            this.type = type;
        }

        public LuaValue call(LuaValue arg) {

            try {
                LuaTable table = arg.checktable();
                MapEntity entity = MapEntity.create(Map.this, type, type.getDefaultSubtype());
                entity.loadFromLua(table);
                addEntity(entity);
            }
            catch (NoSuchTilePatternException ex) {
                badTiles = true;
            }
            catch (QuestEditorException ex) {
                // Error in the input file.
                throw new LuaError(ex);
            }
            catch (Exception ex) {
                // Error in the editor.
                ex.printStackTrace();
                throw new LuaError(ex);
            }
            return LuaValue.NIL;
        }
    }
}

