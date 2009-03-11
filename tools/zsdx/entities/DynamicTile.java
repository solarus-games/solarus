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
