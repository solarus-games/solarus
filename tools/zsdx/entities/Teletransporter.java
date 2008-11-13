package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * A teletransporter is an entity that moves Link to a destination point
 * on another map or the same map.
 */
public class Teletransporter extends ActiveEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Teletransporter";

    // subtypes of teletransporters
    public static final int INVISIBLE         = 0;
    public static final int YELLOW            = 1;
    public static final int BLUE              = 2;

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
	{new EntityImageDescription("teletransporter.png", 0, 0, 32, 32)};

    /**
     * Unitary size of a teletransporter.
     */
    private static final Dimension unitarySize = new Dimension(16, 16);

    private int subtype;
    private int transition;
    private String destinationMapId;
    private String destinationPointName;

    /**
     * Resizable image of a teletransporter.
     */
    private static Image resizableTeletransporterImage;

    /**
     * Creates a new teletransporter at the specified location.
     * @param map the map
     * @param x x coordinate of the entity to create
     * @param y y coordinate of the entity to create
     */
    public Teletransporter(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);
	
	// default field values
	subtype = INVISIBLE;
	transition = Transition.FADE;
	destinationMapId = "";
	destinationPointName = "";
    }

    /**
     * Creates an existing teletransporter from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public Teletransporter(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);

	try {
	    this.subtype = Integer.parseInt(tokenizer.nextToken());
	    this.transition = Integer.parseInt(tokenizer.nextToken());
	    this.destinationMapId = tokenizer.nextToken();
	    this.destinationPointName = tokenizer.nextToken();	    
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

	// add the specific properties of a teletransporter
	buff.append('\t');
	buff.append(getSubtype());
	buff.append('\t');
	buff.append(getTransition());
	buff.append('\t');
	buff.append(getDestinationMapId());
	buff.append('\t');
	buff.append(getDestinationPointName());

	return buff.toString();
    }

    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public boolean isValid() {
	return super.isValid()
	    && destinationMapId.length() != 0 && destinationPointName.length() != 0;	
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_TELETRANSPORTER
     */
    public int getType() {
	return ENTITY_TELETRANSPORTER;
    }
    
    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns whether or not the entity is resizable.
     * A teletransporter is resizable if its subtype is INVISIBLE.
     * @return true if the subtype is INVISIBLE
     */
    public boolean isResizable() {
	return subtype == INVISIBLE;
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return (16,16)
     */
    public Dimension getUnitarySize() {
	return unitarySize;
    }

    /**
     * Returns the type of transition played when taking this teletransporter.
     * @return the transition type
     */
    public int getTransition() {
	return transition;
    }

    /**
     * Returns the subtype of this entity.
     * @return the subtype of this entity
     */
    public int getSubtype() {
	return subtype;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(int subtype) throws MapException {

	if (subtype != this.subtype) {

	    // set the default values of the other fields for this subtype
	    switch (subtype) {

	    case INVISIBLE:
		setTransition(Transition.FADE);
		break;

	    case YELLOW:
		setTransition(Transition.FADE);
		break;

	    case BLUE:
		setTransition(Transition.IMMEDIATE);
		setDestinationMapId(map.getId());
		break;

	    default:
		throw new MapException("Unknown teletransporter subtype: " + subtype);
	    }

	    this.subtype = subtype;
	    if (isResizable()) {
		setSize(16, 16);
	    }
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Sets the type of transition between the two maps.
     * @param transition the transition type
     */
    public void setTransition(int transition) {
	this.transition = transition;

	setChanged();
	notifyObservers();
    }
    
    /**
     * Returns the id of the destination map.
     * @return the id of the destination map
     */
    public String getDestinationMapId() {
	return destinationMapId;
    }

    /**
     * Sets the destination map.
     * @param destinationMapId the id of the destination map
     * @throws MapException if the destination map is the same map
     */
    public void setDestinationMapId(String destinationMapId) throws MapException {

	this.destinationMapId = destinationMapId;

	setChanged();
	notifyObservers();
    }
    
    /**
     * Returns the entrance on the destination map.
     * @return the name of the entrance on the destination map
     */
    public String getDestinationPointName() {
	return destinationPointName;
    }

    /**
     * Sets the entrance on the destination map.
     * @param destinationPointName the name of the entrance on the destination map
     */
    public void setDestinationPointName(String destinationPointName) {
	this.destinationPointName = destinationPointName;

	setChanged();
	notifyObservers();
    }

    /**
     * Draws the teletransporter on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

	if (subtype == INVISIBLE) {
	    if (resizableTeletransporterImage == null) {
		resizableTeletransporterImage = Project.getEditorImage("resizable_teletransporter.png");
	    }

	    int dx1 = (int) (positionInMap.x * zoom);
	    int dy1 = (int) (positionInMap.y * zoom);
	    int dx2 = (int) (dx1 + positionInMap.width * zoom);
	    int dy2 = (int) (dy1 + positionInMap.height * zoom);

	    g.drawImage(resizableTeletransporterImage, dx1, dy1, dx2, dy2, 0, 0, 32, 32, null);

	    g.setColor(Color.black);
	    g.drawLine(dx1, dy1, dx2 - 1, dy1);
	    g.drawLine(dx1 + 3, dy1 + 3, dx2 - 4, dy1 + 3);
	    g.drawLine(dx1, dy2 - 1, dx2 - 1, dy2 - 1);
	    g.drawLine(dx1 + 3, dy2 - 4, dx2 - 4, dy2 - 4);
	    g.drawLine(dx1, dy1, dx1, dy2 - 1);
	    g.drawLine(dx1 + 3, dy1 + 3, dx1 + 3, dy2 - 4);
	    g.drawLine(dx2 - 1, dy1, dx2 - 1, dy2 - 1);
	    g.drawLine(dx2 - 4, dy1 + 3, dx2 - 4, dy2 - 4);

	    g.setColor(new Color(192, 255, 192));
	    g.drawLine(dx1 + 1, dy1 + 1, dx2 - 2, dy1 + 1);
	    g.drawLine(dx1 + 2, dy1 + 2, dx2 - 3, dy1 + 2);
	    g.drawLine(dx1 + 2, dy2 - 3, dx2 - 3, dy2 - 3);
	    g.drawLine(dx1 + 1, dy2 - 2, dx2 - 2, dy2 - 2);
	    g.drawLine(dx1 + 1, dy1 + 1, dx1 + 1, dy2 - 2);
	    g.drawLine(dx1 + 2, dy1 + 2, dx1 + 2, dy2 - 3);
	    g.drawLine(dx2 - 3, dy1 + 2, dx2 - 3, dy2 - 3);
	    g.drawLine(dx2 - 2, dy1 + 1, dx2 - 2, dy2 - 2);
	}
	else {
	    super.paint(g, zoom, showTransparency);
	}
    }
}
