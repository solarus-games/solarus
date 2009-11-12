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
 * A dynamic tile is a tile that can be enabled or disabled dynamically
 * on the map during the game.
 */
public class DynamicTile extends Tile {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = null;

    /**
     * Creates a new dynamic tile tile on the map.
     * @param map the map
     * @throws MapException if the tile is not valid
     */
    public DynamicTile(Map map) throws MapException {
	super(map);
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	super.setPropertiesDefaultValues();
	setProperty("enabled", true);
    }

    /**
     * Creates a static tile with the same properties than this dynamic tile.
     * @return the static tile corresponding to this dynamic tile
     * @throws ZSDXException if the static tile could not be created
     */
    public final Tile createStaticTile() throws ZSDXException {

	String description = toString();

	// remove the name and the 'enabled' boolean
	int index = description.lastIndexOf('\t');
	description = description.substring(0, index);
	index = description.lastIndexOf('\t');
	description = description.substring(0, index);
	index = description.lastIndexOf('\t');
	description = description.substring(0, index + 1);
	description += getTilePatternId();

	// change the entity type
	index = description.indexOf('\t');
	description = EntityType.TILE.getIndex() + description.substring(index);

	return (Tile) createFromString(map, description); 
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }
}
