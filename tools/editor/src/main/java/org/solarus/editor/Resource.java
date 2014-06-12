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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Observable;

/**
 * This class lists all elements of a certain resource.
 * A Resource object contain the maps, another one contains the musics, etc...
 */
public class Resource extends Observable {

    /**
     * Id and human-readable name of each element.
     */
    private HashMap<String, String> elements;

    private ArrayList<String> elementsIndexes;

    /**
     * Creates the resource.
     */
    public Resource() {

        this.elements = new HashMap<String, String>();
        this.elementsIndexes = new ArrayList<String>();
    }

    /**
     * Returns an iterator over the resource ids.
     * @return an iterator over the ids
     */
    public Iterator<String> iterator() {
        return elementsIndexes.iterator();
    }

    /**
     * Returns an array with the id of all elements.
     * @return an array with the id of all elements.
     */
    public String[] getIds() {
        return elementsIndexes.toArray(new String[elementsIndexes.size()]);
    }

    /**
     * Returns whether an element exists.
     * @param id id of the element
     * @return true if it exists in the resource, false otherwise
     */
    public boolean exists(String id) {
        return elements.containsKey(id);
    }

    /**
     * Returns the name of an element from its id.
     * @param id id of an element
     * @return the name of this element
     * @throws QuestEditorException if this element doesn't exist
     */
    public String getElementName(String id) throws QuestEditorException {

        String name = elements.get(id);

        if (name == null) {
            throw new QuestEditorException("There is no element with id '" + id + "'");
        }

        return name;
    }

    /**
     * Changes the name of an element. If the element doesn't exist in the
     * database, it is added.
     * @param id id of an element
     * @param name the name of the element
     * @throws QuestEditorException if the id is not valid
     */
    public void setElementName(String id, String name) throws QuestEditorException {

        if (id == null || id.length() == 0) {
            throw new QuestEditorException("Empty id for element '" + name  + "'");
        }

        String oldName = elements.get(id);
        if (oldName == null || !name.equals(oldName)) {
            // the element doesn't exist yet, or its name has just been changed
            elements.put(id, name);
            // if the element didn't exists, we add it to the keys
            if (oldName == null) {
            	elementsIndexes.add(id);
            }

            setChanged();
            notifyObservers();
        }
    }

    /**
     * Add an element to the resource.
     * If the element already exists in the database, throws an exception
     * @param id Id of the element to add.
     * @param name Human-readable name of this element. Cannot be null.
     */
    public void addElement(String id, String name) throws QuestEditorException {

        if (id == null || id.isEmpty()) {
            throw new QuestEditorException("Empty id");
        }

        if (name == null || name.isEmpty()) {
            throw new QuestEditorException("Empty name");
        }

        if (elements.get(id) == null) {
            elements.put(id, name);
            elementsIndexes.add(id);
            setChanged();
            notifyObservers();
        }
        else {
            throw new QuestEditorException("The element " + id + " already exists");
        }
    }

    /**
     * Removes an element from the resource.
     * Throws an exception if the element doesn't exist
     * in the database
     */
    public void removeElement(String id) throws QuestEditorException {
        if (elements.get(id) == null) {
            throw new QuestEditorException(
                    "Element '" + id + "' doesn't exist in the resource.");
        }
        elementsIndexes.remove(id);
        elements.remove(id);
        setChanged();
        notifyObservers();
    }

    /**
     * Changes the id of an existing element in this resource.
     * @param oldId Id of an element
     * @param newId
     */
    public void moveElement(String oldId, String newId)
            throws QuestEditorException {

    	String value = elements.get(oldId);
        if (value == null) {
	        throw new QuestEditorException(
	                "Element '" + oldId + "' doesn't exist in the resource.");
        }
    	elements.remove(oldId);
    	int index = elementsIndexes.indexOf(oldId);
    	elementsIndexes.set(index, newId);

        elements.put(newId, value);

        setChanged();
        notifyObservers();
    }

    /**
     * Removes all elements from the resource.
     */
    public void clear() {
        elements.clear();
        elementsIndexes.clear();
    }
}
