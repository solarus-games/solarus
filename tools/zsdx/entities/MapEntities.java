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

import java.util.*;


/**
 * Represents all entities placed on the same layer of the map.
 */
public class MapEntities implements Iterable<MapEntity> {

    // the entities
    private List<Tile> tiles;                // the static tiles
    private List<MapEntity> dynamicEntities; // the dynamic entities (including dynamic tiles)

    /**
     * Constructor.
     */
    public MapEntities() {
	tiles = new LinkedList<Tile>();
	dynamicEntities = new LinkedList<MapEntity>();
    }

    /**
     * Copy constructor.
     */
    public MapEntities(MapEntities other) {
	tiles = new LinkedList<Tile>(other.tiles);
	dynamicEntities = new LinkedList<MapEntity>(other.dynamicEntities);
    }

    /**
     * Returns the tiles.
     * @return the tiles
     */
    public List<Tile> getTiles() {
	return tiles;
    }

    /**
     * Returns the number of tiles.
     * @return the number of tiles
     */
    public int getNbTiles() {
	return tiles.size();
    }

    /**
     * Returns the dynamic entities.
     * @return the dynamic entities
     */
    public List<MapEntity> getDynamicEntities() {
	return dynamicEntities;
    }

    /**
     * Returns the number of dynamic entities.
     * @return the number of dynamic entities
     */
    public int getNbDynamicEntities() {
	return dynamicEntities.size();
    }

    /**
     * Returns all entities of a kind, except the tiles.
     * @param entityType a type of entity:
     * MapEntity.ENTITY_DESTINATION_POINT, MapEntity.ENTITY_ENEMY...
     * @return the list of the entities of this kind
     */
    public List<MapEntity> getEntitiesOfType(EntityType entityType) {

	Class<? extends MapEntity> cl = entityType.getEntityClass();
	List<MapEntity> list = new LinkedList<MapEntity>();

	if (MapEntity.class.isAssignableFrom(cl)) {
	    for (MapEntity e: dynamicEntities) {
		
		if (cl.isInstance(e)) {
		    list.add(e);
		}
	    }
	}

	return list;
    }

    /**
     * Returns an entity, specifying its type and its name.
     * @param type the type of entity
     * @param name the name of the entity
     * @return the entity, or null if there is no entity with this name
     */
    public MapEntity getEntityWithName(EntityType type, String name) {

	Class<? extends MapEntity> cl = type.getEntityClass();

	if (MapEntity.class.isAssignableFrom(cl)) {
	    for (MapEntity e: dynamicEntities) {

		if (cl.isInstance(e) && e.hasName() && e.getName().equals(name)) {
		    return e;
		}
	    }
	}

	return null;
    }
    
    /**
     * Returns the number of entities.
     * @return the number of entities
     */
    public int size() {
	return tiles.size() + dynamicEntities.size();
    }

    /**
     * Returns an iterator over the entities.
     * @return an iterator over the entities
     */
    public Iterator<MapEntity> iterator() {
	return listIterator(0);
    }

    /**
     * Returns a list iterator over the entities.
     * @return a list iterator over the entities
     */
    public ListIterator<MapEntity> listIterator(int index) {
	return new MapEntitiesIterator(index);
    }

    /**
     * Adds an entity to the end of the list.
     * @param entity the entity to add
     */
    public void add(MapEntity entity) {
	addLast(entity);
    }

    /**
     * Adds an entity to the beginning of the list.
     * @param entity the entity to add
     */
    public void addFirst(MapEntity entity) {

	if (entity.getType() == EntityType.TILE) {
	    tiles.add(0, (Tile) entity);
	}
	else {
	    dynamicEntities.add(0, entity);
	}
    }

    /**
     * Adds an entity to the end of the list.
     * @param entity the entity to add
     */
    public void addLast(MapEntity entity) {

	if (entity.getType() == EntityType.TILE) {
	    tiles.add((Tile) entity);
	}
	else {
	    dynamicEntities.add(entity);
	}
    }

    /**
     * Removes an entity from the list.
     * @param entity the entity to remove
     */
    public void remove(MapEntity entity) {

	if (entity.getType() == EntityType.TILE) {
	    tiles.remove(entity);
	}
	else {
	    dynamicEntities.remove(entity);
	}
    }

    /**
     * List iterator over the map entities.
     */
    private class MapEntitiesIterator implements ListIterator<MapEntity> {

	private int index; // index of the next element to return

	private int nbTiles;
	private int nbDynamic;

	private ListIterator<Tile> tileIterator;
	private ListIterator<MapEntity> dynamicIterator;

	/**
	 * Constructor.
	 * @param index index of the first element to get
	 */
	public MapEntitiesIterator(int index) {
	    this.index = index;

	    this.nbTiles = tiles.size();
	    this.nbDynamic = dynamicEntities.size();

	    if (index == 0) {
		this.tileIterator = tiles.listIterator(0);
		this.dynamicIterator = dynamicEntities.listIterator(0);
	    }
	    else if (index == size()) {
		this.tileIterator = tiles.listIterator(nbTiles);
		this.dynamicIterator = dynamicEntities.listIterator(nbDynamic);
	    }
	    else {
		throw new IllegalArgumentException();
	    }
	}

	public void add(MapEntity e) throws UnsupportedOperationException {
	    throw new UnsupportedOperationException();
	}

	public void set(MapEntity e) throws UnsupportedOperationException {
	    throw new UnsupportedOperationException();
	}

	public void remove() throws UnsupportedOperationException {
	    throw new UnsupportedOperationException();
	}

	public boolean hasNext() {
	    return index < size();
	}

	public int nextIndex() {
	    return index;
	}

	public MapEntity next() throws NoSuchElementException {

	    int i = this.index;
	    this.index++;

	    if (i < nbTiles) {
		return tileIterator.next();
	    }
	    else {
		i = i - nbTiles;
		if (i < nbDynamic) {
		    return dynamicIterator.next();
		}
		else {
		    throw new NoSuchElementException();
		}
	    }
	}

	public boolean hasPrevious() {
	    return index > 0;
	}

	public int previousIndex() {
	    return index - 1;
	}

	public MapEntity previous() throws NoSuchElementException {

	    this.index--;
	    int i = this.index;

	    if (i < nbTiles) {
		return tileIterator.previous();
	    }
	    else {
		i = i - nbTiles;
		if (i < nbDynamic) {
		    return dynamicIterator.previous();
		}
		else {
		    throw new NoSuchElementException();
		}
	    }
	}

    }
}
