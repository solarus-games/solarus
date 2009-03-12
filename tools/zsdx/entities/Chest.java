package zsdx.entities;

import zsdx.*;

/**
 * Represents a chest placed on the map.
 * Specific properties of a chest:
 * - bigChest
 * - content
 * - amount
 * - savegameVariable
 */
public class Chest extends MapEntity implements DynamicEntity {

    /**
     * Description of the default images representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
	{new EntityImageDescription("chest.png", 0, 0, 16, 16)};

    /**
     * Creates a new chest.
     * @param map the map
     */
    public Chest(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("bigChest", false);
	setProperty("content", TreasureContent.NOTHING.getIndex());
	setProperty("amount", 1);
	setProperty("savegameVariable", 0);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	boolean bigChest = isBigChest();

	if (bigChest) {
	    if (!map.isInDungeon()) {
		throw new MapException("Cannot have a big chest outside a dungeon");
	    }
            setSizeImpl(32, 24);
	}
	else {
            setSizeImpl(16, 16);	    
	}

	TreasureContent content = TreasureContent.get(getIntegerProperty("content"));
 
	if (content.mustBeInDungeon() && !map.isInDungeon()) {
	    throw new MapException("This kind of treasure can only exist in a dungeon");
	}

	if (content == TreasureContent.SMALL_KEY && !map.hasSmallKeys()) {
	    throw new MapException("The small keys are not enabled for this map. Please enable them first by choosing a variable to save them in this map.");
	}

	int savegameVariable = getIntegerProperty("savegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid savegame variable");
	}

	int amount = getIntegerProperty("amount");
	if (amount < 0) {
	    throw new MapException("The amount cannot be negative");
	}

	if (content.hasAmount() && amount <= 0) {
	    throw new MapException("This kind of treasure must have a positive amount");
	}
    }

    public boolean isBigChest() {
	return getBooleanProperty("bigChest");
    }
    
    /**
     * Returns whether this entity is an obstacle.
     * @return true
     */
    public int getObstacle() {
	return OBSTACLE;
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	if (isBigChest()) {
	    currentImageDescription.setRectangle(0, 16, 32, 24);
	}
	else {
	    currentImageDescription.setRectangle(0, 0, 16, 16);
	}
    }

}
