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

import java.util.*;
import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.solarus.editor.map_editor_actions.*;

/**
 * Represents the entities selected in the map editor.
 */
public class MapEntitySelection extends Observable implements Iterable<MapEntity> {

    /**
     * The selected entities, in the same order than the map.
     */
    private HashSet<MapEntity> entities;

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
        this.entities = new HashSet<MapEntity>();
    }

    /**
     * Returns all the selected entities.
     * @return the selected entities
     */
    public Collection<MapEntity> getEntities() {
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
     * Returns an arbitrary selected entity.
     * @return A selected entity, or null if no entities are selected.
     */
    public MapEntity getEntity() {
        if (isEmpty()) {
            return null;
        }
        return iterator().next();
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
        return entities.isEmpty();
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
     * Make some entities selected.
     * If other entities were selected, they are not unselected.
     * @param entities the entities to select
     */
    public void select(Collection<MapEntity> entities) {

        for (MapEntity entity: entities) {
            this.entities.add(entity);
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
     * Make some entities unselected.
     * @param entities The entities to unselect
     */
    public void unselect(Collection<MapEntity> entities) {

        for (MapEntity entity: entities) {
            this.entities.remove(entity);
        }
        setChanged();
        notifyObservers();
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
    public void removeFromMap() throws QuestEditorException {

        if (!entities.isEmpty()) {
            map.getHistory().doAction(new ActionRemoveEntities(map, entities));
        }
    }

    /**
     * Returns the layer of the selected entities, if all selected entities have the same layer.
     * Otherwise, returns null.
     * @return the common layer, or null if all selected entities have not the same layer
     */
    public Layer getLayer() {

        Layer layer = null;

        for (MapEntity entity: entities) {
            if (layer == null) {
                // First entity.
                layer = entity.getLayer();
            }
            else {
                if (entity.getLayer() != layer) {
                    return null;
                }
            }
        }

        return layer;
    }

    /**
     * Changes the layer of the selected entities.
     * @param layer the new layer
     */
    public void setLayer(Layer layer) throws QuestEditorException {

        map.getHistory().doAction(new ActionChangeLayer(map, entities, layer));
    }

    /**
     * Brings the selected entities to the front in their layer.
     */
    public void bringToFront() throws QuestEditorException {

        map.getHistory().doAction(new ActionBringToFront(map, entities));
    }

    /**
     * Brings the selected entities to the back in their layer.
     */
    public void bringToBack() throws QuestEditorException {

        map.getHistory().doAction(new ActionBringToBack(map, entities));
    }

    /**
     * Returns whether the selection is resizable.
     * @return true if all selected entities are resizable.
     */
    public boolean isResizable() {

        for (MapEntity entity: entities) {

            if (!entity.isResizable()) {
                return false;
            }
        }

        return true;
    }

    /**
     * Returns whether the entities selected have a direction property.
     * @return true if all selected entities have a direction property, false otherwise
     */
    public boolean hasDirectionProperty() {

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

        MapEntity e = getEntity();

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
     * the same direction. Otherwise, -100 is returned. If at least an entity has
     * no direction property, -100 is returned as well.
     * @return the common direction (possibly -1) or -100 if all selected
     * entities have not the same direction
     */
    public int getDirection() {

        MapEntity e = getEntity();

        if (!e.hasDirectionProperty()) {
            return -100;
        }

        int direction = e.getDirection();

        for (MapEntity entity: entities) {

            if (!entity.hasDirectionProperty() || entity.getDirection() != direction) {
                return -100;
            }
        }

        return direction;
    }

    /**
     * Changes the direction of the selected entities.
     * @param direction the new direction
     */
    public void setDirection(int direction) throws QuestEditorException {

        map.getHistory().doAction(new ActionChangeDirection(map, entities, direction));
    }

    /**
     * For entities that includes an option to set 'no direction'
     * (i.e. when canHaveNoDirection() returns true),
     * this method returns the text that will be displayed in the direction chooser.
     * If at least one of the selected entities cannot have a 'no direction' property,
     * null is returned. If the text differs between the selected entities, a default text
     * is returned.
     * @return the text that will be displayed in the direction chooser
     * for the 'no direction' option if any
     */
    public String getNoDirectionText() {

        if (!canHaveNoDirection()) {
            return null;
        }

        String text = getEntity().getNoDirectionText();
        for (MapEntity entity: entities) {

            if (!entity.getNoDirectionText().equals(text)) {
                return "No direction";
            }
        }

        return text;
    }

    /**
     * Returns whether all selected entities can have the special direction value -1
     * indicating that no direction is set.
     * @return true if all selected entities can have the special direction value -1
     */
    public boolean canHaveNoDirection() {

        for (MapEntity entity: entities) {

            if (!entity.hasDirectionProperty() || !entity.canHaveNoDirection()) {
                return false;
            }
        }

        return true;
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

