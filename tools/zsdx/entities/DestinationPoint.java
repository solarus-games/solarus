package zsdx.entities;

import java.awt.*;
import zsdx.*;

/**
 * Represents the destination point of a teletransporter in a map.
 */
public class DestinationPoint extends MapEntity {

    /**
     * Subtypes of destination points.
     */
    public enum Subtype implements EntitySubtype {
	INVISIBLE,
	GRAY;

	public static final String[] humanNames = {
	  "Invisible", "Gray"  
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
	new EntityImageDescription("destination_point.png", 32, 0, 32, 32),    // invisible (up)
	new EntityImageDescription("destination_point.png", 128, 0, 16, 16)    // gray
    };

    /**
     * Indicates whether the direction of Link is changed when he arrives
     * on this destination. If this boolean is true, then Link' direction is
     * indicated by this entity's direction.
     */
    private boolean changeDirection;

    /**
     * Origin point of a destination point.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Creates a new destination point.
     * @param map the map
     */
    public DestinationPoint(Map map) throws MapException {
	super(map, 16, 16);
	subtype = Subtype.INVISIBLE;
	setDirection(1);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
	return origin;
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }

    /**
     * Returns the direction of the entity.
     * This is a redefinition of MapEntity.setDirection to handle the special value of -1
     * indicating that the direction is not changed.
     * @return the entity's direction
     */
    public int getDirection() {

	if (!changeDirection) {
	    return -1;
	}

	return super.getDirection();
    }

    /**
     * Changes the direction of the entity.
     * This is a redefinition of MapEntity.setDirection to handle the special value of -1
     * indicating that the direction is not changed.
     * @param direction the entity's direction, or -1
     */
    public void setDirection(int direction) {
	if (direction == -1) {
	    changeDirection = false;
	}
	else {
	    changeDirection = true;
	    super.setDirection(direction);
	}
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
		if (this.subtype != null) {
		    setDirection(1);
		}
	    }
	    else {
		    setDirection(-1);
	    }
	    super.setSubtype(subtype);
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	if (subtype == Subtype.INVISIBLE) {
	    currentImageDescription.setRectangle(getDirection() * 32, 0, 32, 32);
	}
	else {
	    currentImageDescription.setRectangle(128, 0, 16, 16);	    
	}
    }
}
