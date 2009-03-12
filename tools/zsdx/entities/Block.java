package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.Map;
import zsdx.*;

/**
 * Represents a block or a statue that the hero can push or pull.
 */
public class Block extends MapEntity implements DynamicEntity {

    /**
     * Subtypes of blocks.
     */
    public enum Subtype implements EntitySubtype {
	NORMAL_BLOCK,
	STATUE;

	public static final String[] humanNames = {
	    "Normal block",
	    "Statue",
	};

	public static Subtype get(int id) {
	    return values()[id];
	}

	public int getId() {
	    return ordinal();
	}
    }

    /**
     * Block skins.
     */
    public enum Skin {
	BROWN,
	YELLOW,
	BLUE;

	public static final String[] humanNames = {
	    "Brown block",
	    "Yellow block",
	    "Blue block",
	};

	private static final String[] animationNames = {
	    "brown_block",
	    "yellow_block",
	    "blue_block",
	};

	public static Skin get(String animationName) {
	    for (Skin skin: values()) {
		if (skin.getAnimationName().equals(animationName)) {
		    return skin;
		}
	    }
	    throw new NoSuchElementException("Unknown skin '" + animationName + "'");
	}

	public String getAnimationName() {
	    return animationNames[ordinal()];
	}
    }

    /**
     * Limitations of moves for blocks.
     */
    public enum MaximumMoves {
	NONE,
	ONE,
	INFINITE;

	public static final String[] humanNames = {
	    "Cannot move",
	    "One move only",
	    "Infinite moves",
	};

	public static MaximumMoves get(int id) {
	    return values()[id];
	}

	public int getId() {
	    return ordinal();
	}
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("miscellaneous_entities.png", 56, 0, 16, 16),
	new EntityImageDescription("miscellaneous_entities.png", 32, 0, 24, 24),
    };

    /**
     * Description of the image currently representing this kind of entity.
     */
    public static final EntityImageDescription[] currentImageDescriptions = {
	generalImageDescriptions[0],
	new EntityImageDescription("miscellaneous_entities.png", 36, 0, 16, 24),
    };

    /**
     * Origin point of each subtype.
     */
    private static final Point[] origins = {
	new Point(8, 13),
	new Point(8, 21),
    };

    /**
     * Size of each subtype.
     */
    private static final Dimension[] sizes = {
	new Dimension(16, 16),
	new Dimension(16, 24),
    };

    /**
     * Creates a new block.
     * @param map the map
     */
    public Block(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
	return origins[getSubtypeId()];
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns whether this entity is an obstacle.
     * @return true
     */
    public int getObstacle() {
	return OBSTACLE;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {
	super.setSubtype(subtype);

	Dimension size = sizes[getSubtypeId()];
	setSizeImpl(size.width, size.height);

	setChanged();
	notifyObservers();
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription = currentImageDescriptions[getSubtypeId()];
	if (subtype == Subtype.NORMAL_BLOCK) {
	    Skin skin = Skin.get(getProperty("skin"));
	    currentImageDescription.setX(56 + skin.ordinal() * 16);
	}
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("skin", Skin.BROWN.getAnimationName());
	setProperty("maximumMoves", MaximumMoves.ONE.getId());
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	String skin = getProperty("skin");
	if (skin.length() == 0 || skin.contains(" ") || skin.contains("\t")) {
	    throw new MapException("Invalid skin name: please choose an animation name for the sprite block");
	}
    }
}
