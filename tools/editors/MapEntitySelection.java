package editors;

import java.util.*;
import editors.map_editor_actions.*;

/**
 * Represents the entities selected in the map editor.
 */
public class MapEntitySelection extends Observable implements Iterable {

    /**
     * The selected entities.
     */
    private LinkedList<MapEntity> entities;

    /**
     * The map.
     */
    private Map map;

    /**
     * Constructor.
     * @param map the map
     */
    public MapEntitySelection(Map map) {
	this.map = map;
	this.entities = new LinkedList<MapEntity>();
    }

    /**
     * Returns a selected entity.
     * @param index of the entity to get
     * @return the entity at this index in the selection
     */
    public MapEntity getEntity(int index) {
	return entities.get(index);
    }

    /**
     * Returns all the selected entities.
     * @return the selected entities
     */
    public LinkedList<MapEntity> getEntities() {
	return entities;
    }

    /**
     * Returns an iterator over the selected entities.
     * @return an iterator over the selected entities.
     */
    public Iterator iterator() {
	return entities.iterator();
    }

    /**
     * Returns whether or not an entity is selected.
     * @param entity an entity
     * @return true if the entity is selected, false otherwise
     */
    public boolean isSelected(MapEntity entity) {
	return entities.contains(entity);
    }

    /**
     * Returns the number of entities selected.
     * @return the number of entities selected.
     */
    public int getNbEntitiesSelected() {
	return entities.size();
    }

    /**
     * Returns whether or not the selection is empty.
     * @return true if there is no entity selected, false otherwise
     */
    public boolean isEmpty() {
	return entities.size() == 0;
    }

    /**
     * Selects an entity.
     * @param entity the entity to select
     */
    public void select(MapEntity entity) {
	if (!isSelected(entity)) {
	    entities.add(entity);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects an entity.
     * @param entity the entity to unselect
     */
    public void unSelect(MapEntity entity) {
	if (entities.contains(entity)) {
	    entities.remove(entity);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects all entities.
     */
    public void unSelectAll() {
	entities.clear();
	setChanged();
	notifyObservers();
    }

    /**
     * Changes the selection state of an entity.
     * @param entity the entity to select or unselect
     */
    public void switchSelection(MapEntity entity) {
	if (isSelected(entity)) {
	    unSelect(entity);
	}
	else {
	    select(entity);
	}
    }

    /**
     * Removes the selected entities from the map.
     * If there is no entity selected, nothing is done.
     */
    public void removeFromMap() throws ZSDXException {

	if (entities.size() > 0) {
	    map.getHistory().doAction(new ActionRemoveEntities(map, entities));
	}
    }

    /**
     * Returns the layer of the selected entities, if all selected entities have the same layer.
     * Otherwise, returns -1.
     * @return the common layer, or -1 if all selected entities have not the same layer
     */
    public int getLayer() {
	
	int layer = entities.get(0).getLayer();
	
	for (int i = 1; i < entities.size(); i++) {
	    if (entities.get(i).getLayer() != layer) {
		return -1;
	    }
	}

	return layer;
    }

    /**
     * Changes the layer of the selected entities.
     * @param layer the new layer
     */
    public void setLayer(int layer) throws ZSDXException {

	map.getHistory().doAction(new ActionChangeLayer(map, entities, layer));
    }

    /**
     * Brings the selected entities to the front in their layer.
     */
    public void bringToFront() throws ZSDXException {

	map.getHistory().doAction(new ActionBringToFront(map, entities));
    }

    /**
     * Brings the selected entities to the back in their layer.
     */
    public void bringToBack() throws ZSDXException {

	map.getHistory().doAction(new ActionBringToBack(map, entities));
    }

    /**
     * Returns whether the selection is resizable.
     * @return true if there is exactly one entity selected, and if this entity is resizable
     */
    public boolean isResizable() {

	return getNbEntitiesSelected() == 1 && getEntity(0).isResizable();
    }

    /**
     * Returns whether the entities selected have a direction.
     * @return true if all selected entities have a direction, false otherwise
     */
    public boolean hasDirection() {
	
	for (MapEntity entity: entities) {
	    
	    if (!entity.hasDirection()) {
		return false;
	    }
	}

	return true;
    }

    /**
     * Returns the direction of the selected entities, if all selected entities have
     * the same direction. Otherwise, returns -1. If at least an entity has no direction,
     * -1 is returned as well.
     * @return the common direction, or -1 if all selected entities have not the same direction
     * or if an entity has no direction
     */
    public int getDirection() {

	MapEntity e = entities.get(0);
	
	if (!e.hasDirection()) {
	    return -1;
	}

	int direction = e.getDirection();
	
	for (MapEntity entity: entities) {

	    if (!entity.hasDirection() || entity.getDirection() != direction) {
		return -1;
	    }
	}

	return direction;
    }

    /**
     * Changes the direction of the selected entities.
     * @param direction the new direction
     */
    public void setDirection(int direction) throws ZSDXException {

	map.getHistory().doAction(new ActionChangeDirection(map, entities, direction));
    }

    /**
     * Returns whether the entities selected have a name.
     * @return true if all selected entities have a name, false otherwise
     */
    public boolean hasName() {
	
	for (MapEntity entity: entities) {
	    
	    if (!entity.hasName()) {
		return false;
	    }
	}

	return true;
    }

}
