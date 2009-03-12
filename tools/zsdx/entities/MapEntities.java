package zsdx.entities;

import java.util.*;


/**
 * Represents all entities placed on the same layer of the map.
 */
public class MapEntities implements Iterable<MapEntity> {

    // the entities
    private List<Tile> tiles;                    // the static tiles
    private List<DynamicEntity> dynamicEntities; // the dynamic entities (including dynamic tiles)

    /**
     * Constructor.
     */
    public MapEntities() {
	tiles = new LinkedList<Tile>();
	dynamicEntities = new LinkedList<DynamicEntity>();
    }

    /**
     * Copy constructor.
     */
    public MapEntities(MapEntities other) {
	tiles = new LinkedList<Tile>(other.tiles);
	dynamicEntities = new LinkedList<DynamicEntity>(other.dynamicEntities);
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
    public List<DynamicEntity> getDynamicEntities() {
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
    public List<DynamicEntity> getEntitiesOfType(EntityType entityType) {

	Class<? extends MapEntity> cl = entityType.getEntityClass();
	List<DynamicEntity> list = new LinkedList<DynamicEntity>();

	if (DynamicEntity.class.isAssignableFrom(cl)) {
	    for (DynamicEntity e: dynamicEntities) {
		
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
    public DynamicEntity getEntityWithName(EntityType type, String name) {

	Class<? extends MapEntity> cl = type.getEntityClass();

	if (DynamicEntity.class.isAssignableFrom(cl)) {
	    for (DynamicEntity e: dynamicEntities) {

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
	    dynamicEntities.add(0, (DynamicEntity) entity);
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
	    dynamicEntities.add((DynamicEntity) entity);
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
	private ListIterator<DynamicEntity> dynamicIterator;

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
		    return (MapEntity) dynamicIterator.next();
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
		    return (MapEntity) dynamicIterator.previous();
		}
		else {
		    throw new NoSuchElementException();
		}
	    }
	}

    }
}
