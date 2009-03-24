package zsdx.entities;

import zsdx.*;

/**
 * A switch is a button that can be enabled by the hero or a block.
 */
public class Switch extends MapEntity {

    /**
     * Subtypes of switches.
     */
    public enum Subtype implements EntitySubtype {
	INVISIBLE,
	NORMAL;

	public static final String[] humanNames = {
	  "Invisible", "Normal"  
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("miscellaneous_entities.png", 56, 16, 16, 16), // invisible
	new EntityImageDescription("miscellaneous_entities.png", 72, 16, 16, 16), // normal
    };

    /**
     * Creates a new switch.
     * @param map the map
     */
    public Switch(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {

	if (subtype != this.subtype) {
	    if (subtype == Subtype.INVISIBLE) {
		setProperty("needsBlock", false);
	    }
	    super.setSubtype(subtype);
	}
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("needsBlock", false);
	setProperty("disableWhenLeaving", false);
	setSubtype(Subtype.NORMAL);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	if (getSubtype() == Subtype.INVISIBLE && getBooleanProperty("needsBlock")) {
	    throw new MapException("Cannot put a block on an invisible switch");
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription = generalImageDescriptions[subtype.getId()];
    }
}
