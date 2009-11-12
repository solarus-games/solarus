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

import zsdx.*;

/**
 * This exception is thrown when an invalid entity is detected on the map.
 */
public class InvalidEntityException extends MapException {

    private MapEntity entity;

    /**
     * Constructor.
     * @param message error message
     * @param entity the invalid entity detected
     */
    public InvalidEntityException(String message, MapEntity entity) {
	super(message);
	this.entity = entity;
    }
    
    /**
     * Returns the invalid entity that was detected.
     * @return the invalid entity
     */
    public MapEntity getEntity() {
	return entity;
    }
}
