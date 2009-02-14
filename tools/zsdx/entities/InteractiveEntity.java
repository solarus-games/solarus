package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.Map;
import zsdx.*;

/**
 * Represents an entity that triggers a message or an event when the player
 * presses the action key in front of it.
 */
public class InteractiveEntity extends DynamicEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Interactive entity";

    /**
     * Subtypes of interactive entities.
     */
    public enum Subtype {
	CUSTOM,
	NON_PLAYING_CHARACTER,
	SIGN,
	WATER_FOR_BOTTLE,
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("interactive_entities.png", 0, 0, 32, 32),
	new EntityImageDescription("interactive_entities.png", 32, 0, 16, 16),
	new EntityImageDescription("interactive_entities.png", 48, 0, 16, 16),
	new EntityImageDescription("interactive_entities.png", 64, 0, 32, 32)
    };

    /**
     * Description of the image currently representing this kind of entity.
     */
    public static final EntityImageDescription[] currentImageDescriptions = {
	generalImageDescriptions[0],
	new EntityImageDescription("interactive_entities.png", 32, 0, 16, 24),
	generalImageDescriptions[2],
	generalImageDescriptions[3]
    };

    /**
     * Origin point of each type of interactive entity.
     */
    private static final Point[] origins = {
	new Point(0, 0),
	new Point(8, 21),
	new Point(8, 13),
	new Point(0, 0)
    };

    /**
     * Size of each type of interactive entity.
     */
    private static final Dimension[] sizes = {
	new Dimension(16, 16),
	new Dimension(16, 24),
	new Dimension(16, 16),
	new Dimension(16, 16)
    };

    /**
     * Subtype of interactive entity.
     */
    private Subtype subtype;

    // specific fields
    
    /**
     * Name of the sprite to display for this entity, or null if to make the entity invisible.
     */
    private String sprite;
    
    /**
     * Id of the message to display when the player interacts with this entity,
     * or null to call the map script instead.
     */
    private String messageId;

    /**
     * Creates a new interactive entity at the specified location.
     * @param map the map
     * @param x x coordinate of the entity
     * @param y y coordinate of the entity
     */
    public InteractiveEntity(Map map, int x, int y) throws MapException {
	super(map, LAYER_LOW, x, y, 16, 16);

	setSubtype(Subtype.CUSTOM);
	sprite = "";
	messageId = "";

	setDirection(3);
    }

    /**
     * Creates an existing interactive entity from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the subtype of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public InteractiveEntity(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);

	// parse the fields
	try {
	    Subtype subtype = Subtype.values()[Integer.parseInt(tokenizer.nextToken())];
	    this.sprite = tokenizer.nextToken();
	    this.messageId = tokenizer.nextToken();

	    if (sprite.equals("_none")) {
		sprite = null;
	    }

	    if (messageId.equals("_none")) {
		messageId = null;
	    }

	    setSubtype(subtype);
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Returns a string describing this entity.
     * @return a string representation of the entity
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of this entity
	String sprite = getSprite();
	if (sprite == null) {
	    sprite = "_none";
	}
	String messageId = getMessageId();
	if (messageId == null) {
	    messageId = "_none";
	}

	buff.append('\t');
	buff.append(getSubtypeIndex());
	buff.append('\t');
	buff.append(sprite);
	buff.append('\t');
	buff.append(messageId);

	return buff.toString();
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return the type of entity
     */
    public int getType() {
	return ENTITY_INTERACTIVE;
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
	return origins[getSubtypeIndex()];
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
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
     * Returns the subtype of this entity.
     * @return the subtype of this entity
     */
    public int getSubtypeIndex() {
	if (subtype == null) {
	    return 0;
	}
	return subtype.ordinal();
    }

    /**
     * Returns the subtype of this entity.
     * @return the subtype of entity
     */
    public Subtype getSubtype() {
	return subtype;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     */
    public void setSubtypeIndex(int subtype) throws MapException {
	setSubtype(Subtype.values()[subtype]);
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     */
    public void setSubtype(Subtype subtype) throws MapException {
	this.subtype = subtype;

	Dimension size = sizes[getSubtypeIndex()];
	setSizeImpl(size.width, size.height);

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the name of the sprite displaying this entity.
     * @return the name of the sprite, or null if the entity is not visible
     */
    public String getSprite() {
        return sprite;
    }

    /**
     * Sets the name of the sprite displaying this entity.
     * @param sprite name of the sprite, or null if the entity is not visible
     */
    public void setSprite(String sprite) throws MapException {

	if (!isSpriteValid(sprite)) {
	    throw new MapException("Invalid sprite name");
	}

	this.sprite = sprite;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether the specified sprite name is valid
     * @param sprite a sprite name
     * @return true if it is valid
     */
    public boolean isSpriteValid(String sprite) {
	return (sprite == null ||
		(sprite.length() != 0 && sprite.charAt(0) != '_'));
    }

    /**
     * Returns the id of the message to display when the player interacts
     * with this entity
     * @return the id of the message to display, or null to call the map script instead
     */
    public String getMessageId() {
        return messageId;
    }

    /**
     * Sets the id of the message to display when the player interacts
     * with this entity.
     * @param messageId id of the message to display, or null to call the map script instead
     */
    public void setMessageId(String messageId) throws MapException {

	if (!isMessageIdValid(messageId)) {
	    throw new MapException("Invalid message id");
	}

        this.messageId = messageId;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether the specified message id is valid
     * @param messageId a message id
     * @return true if it is valid
     */
    public boolean isMessageIdValid(String messageId) {
	return (messageId == null ||
		(messageId.length() != 0 && messageId.charAt(0) != '_'));
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription = currentImageDescriptions[getSubtypeIndex()];
    }

    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public boolean isValid() {
	return super.isValid() && isSpriteValid(sprite) && isMessageIdValid(messageId);
    }
}
