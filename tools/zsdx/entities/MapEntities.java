package zsdx.entities;

import java.util.*;


/**
 * Represents all entities placed on the same layer of the map.
 */
public class MapEntities implements Iterable<MapEntity> {

    // the entities
    private List<TileOnMap> tiles;
    private List<DynamicEntity> activeEntities;

    /**
     * Constructor.
     */
    public MapEntities() {
	tiles = new LinkedList<TileOnMap>();
	activeEntities = new LinkedList<DynamicEntity>();
    }

    /**
     * Copy constructor.
     */
    public MapEntities(MapEntities other) {
	tiles = new LinkedList<TileOnMap>(other.tiles);
	activeEntities = new LinkedList<DynamicEntity>(other.activeEntities);
    }

    /**
     * Returns the tiles.
     * @return the tiles
     */
    public List<TileOnMap> getTiles() {
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
     * Returns the active entities.
     * @return the active entities
     */
    public List<DynamicEntity> getActiveEntities() {
	return activeEntities;
    }

    /**
     * Returns the number of active entities.
     * @return the number of active entities
     */
    public int getNbActiveEntities() {
	return activeEntities.size();
    }

    /**
     * Returns all entities of a kind, except the tiles.
     * @param entityType a type of entity:
     * MapEntity.ENTITY_DESTINATION_POINT, MapEntity.ENTITY_ENEMY...
     * @return the list of the entities of this kind
     */
    public List<MapEntity> getEntitiesOfType(int entityType) {

	Class<?> cl = MapEntity.entityClasses[entityType];
	List<MapEntity> list = new LinkedList<MapEntity>();

	if (DynamicEntity.class.isAssignableFrom(cl)) {
	    for (MapEntity e: activeEntities) {
		
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
    public MapEntity getEntityWithName(int type, String name) {

	Class<?> cl = MapEntity.entityClasses[type];

	if (DynamicEntity.class.isAssignableFrom(cl)) {
	    for (MapEntity e: activeEntities) {
		
		if (e.getType() == type && e.hasName() && e.getName().equals(name)) {
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
	return tiles.size() + activeEntities.size();
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

	if (entity instanceof TileOnMap) {
	    tiles.add(0, (TileOnMap) entity);
	}
	else if (entity instanceof DynamicEntity) {
	    activeEntities.add(0, (DynamicEntity) entity);
	}
	else {
	    throw new IllegalArgumentException("Unknown entity type: " + entity);
	}
    }

    /**
     * Adds an entity to the end of the list.
     * @param entity the entity to add
     */
    public void addLast(MapEntity entity) {

	if (entity instanceof TileOnMap) {
	    tiles.add((TileOnMap) entity);
	}
	else if (entity instanceof DynamicEntity) {
	    activeEntities.add((DynamicEntity) entity);
	}
	else {
	    throw new IllegalArgumentException("Unknown entity type: " + entity);
	}
    }

    /**
     * Removes an entity from the list.
     * @param entity the entity to remove
     */
    public void remove(MapEntity entity) {

	if (entity instanceof TileOnMap) {
	    tiles.remove(entity);
	}
	else if (entity instanceof DynamicEntity) {
	    activeEntities.remove(entity);
	}
	else {
	    throw new IllegalArgumentException("Unknown entity type: " + entity);
	}
    }

    /**
     * List iterator over the map entities.
     */
    private class MapEntitiesIterator implements ListIterator<MapEntity> {

	private int index; // index of the next element to return

	private int nbTiles;
	private int nbActive;

	private ListIterator<TileOnMap> tileIterator;
	private ListIterator<DynamicEntity> activeIterator;

	/**
	 * Constructor.
	 * @param index index of the first element to get
	 */
	public MapEntitiesIterator(int index) {
	    this.index = index;

	    this.nbTiles = tiles.size();
	    this.nbActive = activeEntities.size();

	    if (index == 0) {
		this.tileIterator = tiles.listIterator(0);
		this.activeIterator = activeEntities.listIterator(0);
	    }
	    else if (index == size()) {
		this.tileIterator = tiles.listIterator(nbTiles);
		this.activeIterator = activeEntities.listIterator(nbActive);
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
		if (i < nbActive) {
		    return activeIterator.next();
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
		if (i < nbActive) {
		    return activeIterator.previous();
		}
		else {
		    throw new NoSuchElementException();
		}
	    }
	}

    }
}
