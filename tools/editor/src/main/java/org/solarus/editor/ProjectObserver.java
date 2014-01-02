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

/**
 * The classes implementing this interface can be notified when a project is loaded.
 */
public interface ProjectObserver {

    /**
     * This method is called when a project has just been loaded.
     */
    public void currentProjectChanged();

    /**
     * Called when a new resource element has just been created.
     * @param resourceType Type of resource.
     * @param id Id of the new element.
     */
    public void resourceElementAdded(ResourceType resourceType, String id);

    /**
     * Called when a new resource element has just been deleted.
     * @param resourceType Type of resource.
     * @param id Id of the deleted element.
     */
    public void resourceElementRemoved(ResourceType resourceType, String id);

    /**
     * Called when a new resource element has just been moved.
     * @param resourceType Type of resource.
     * @param oldId Old id of the element.
     * @param newId New id of the element.
     */
    public void resourceElementMoved(ResourceType resourceType,
            String oldId, String newId);

    /**
     * Called when a resource element has just been renamed.
     * @param resourceType Type of resource.
     * @param id Id of the element.
     * @param name New human-readable name of the element.
     */
    public void resourceElementRenamed(ResourceType resourceType,
            String id, String name);
}
