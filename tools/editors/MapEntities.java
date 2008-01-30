package editors;

import java.util.*;

/**
 * Represents all entities placed on the same layer of the map.
 */
public class MapEntities implements Iterable<MapEntity> {

    // the tiles
    private LinkedList<TileOnMap> tiles;

    /**
     * Constructor.
     */
    public MapEntities() {
	tiles = new LinkedList<TileOnMap>();
    }

    /**
     * Copy constructor.
     */
    public MapEntities(MapEntities other) {
	tiles = new LinkedList<TileOnMap>(other.tiles);
    }

    /**
     * Returns the tiles.
     * @return the tiles
     */
    public LinkedList<TileOnMap> getTiles() {
	return tiles;
    }

    /**
     * Returns the number of entities.
     * @return the number of entities
     */
    public int size() {
	return tiles.size();
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
	// TODO
	return null;
    }

    public void add(MapEntity entity) {
	// TODO
    }

    public void addFirst(MapEntity entity) {
	// TODO
    }

    public void addLast(MapEntity entity) {
	// TODO
    }

    public void remove(MapEntity entity) {
	// TODO
    }

}
