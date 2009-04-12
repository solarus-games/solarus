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
import zsdx.*;
import zsdx.Map;
import zsdx.map_editor_actions.*;

/**
 * Represents the entities selected in the map editor.
 */
public class MapEntitySelection extends Observable implements Iterable<MapEntity> {

    /**
     * The selected entities, in the same order than the map.
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
    public Iterator<MapEntity> iterator() {
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
//	    entities.add(computeIndex(entity), entity);
	    entities.add(entity);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * For an entity about to be selected, finds an index in the selected entities list
     * such that the selected entities list remain in the same order the map entities. 
     * @param entityToAdd an entity
     * @return the index where this entity can be added to the selection
     */
    /*
    private int computeIndex(MapEntity entityToAdd) {

	int index = 0;
	boolean stop = false;
	Iterator<MapEntity> it = entities.iterator();
	while (it.hasNext() && !stop) {
	    MapEntity e = it.next();
	    if (map.isBehind(entityToAdd, e)) {
		stop = true;
	    }
	    else {
		index++;
	    }
	}
	return index;
    }
    */

    /**
     * Make some entities selected.
     * If other entities were selected, they are not unselected.
     * @param entities the entities to select
     */
    public void select(List<MapEntity> entities) {

	for (MapEntity entity: entities) {
	    select(entity);
	}
	setChanged();
	notifyObservers();
    }

    /**
     * Unselects an entity.
     * @param entity the entity to unselect
     */
    public void unselect(MapEntity entity) {
	if (entities.contains(entity)) {
	    entities.remove(entity);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects all entities.
     */
    public void unselectAll() {
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
	    unselect(entity);
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
     * Otherwise, returns null.
     * @return the common layer, or null if all selected entities have not the same layer
     */
    public Layer getLayer() {

	Layer layer = entities.get(0).getLayer();

	for (int i = 1; i < entities.size(); i++) {
	    if (entities.get(i).getLayer() != layer) {
		return null;
	    }
	}

	return layer;
    }

    /**
     * Changes the layer of the selected entities.
     * @param layer the new layer
     */
    public void setLayer(Layer layer) throws ZSDXException {

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
	    
	    if (!entity.hasDirectionProperty()) {
		return false;
	    }
	}

	return true;
    }

    /**
     * Returns the number of possible directions of the selected entities, if all selected
     * entities have the same number of directions. Otherwise, 0 is returned.
     * @return the common number of possible direction, or 0 if all selected entities
     * have not the same number of directions
     */
    public int getNbDirections() {

	MapEntity e = entities.get(0);
	
	if (!e.hasDirectionProperty()) {
	    return 0;
	}

	int nbDirections = e.getNbDirections();
	
	for (MapEntity entity: entities) {

	    if (entity.getNbDirections() != nbDirections) {
		return 0;
	    }
	}

	return nbDirections;
    }

    /**
     * Returns the direction of the selected entities, if all selected entities have
     * the same direction. Otherwise, -1 is returned. If at least an entity has no direction,
     * -1 is returned as well.
     * @return the common direction, or -1 if all selected entities have not the same direction
     */
    public int getDirection() {

	MapEntity e = entities.get(0);
	
	if (!e.hasDirectionProperty()) {
	    return -1;
	}

	int direction = e.getDirection();
	
	for (MapEntity entity: entities) {

	    if (!entity.hasDirectionProperty() || entity.getDirection() != direction) {
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
