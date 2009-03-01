package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * A teletransporter is an entity that moves Link to a destination point
 * on another map or the same map.
 */
public class Teletransporter extends DynamicEntity {

    // subtypes of teletransporters
    public static final int INVISIBLE         = 0;
    public static final int YELLOW            = 1;
    public static final int BLUE              = 2;

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("teletransporter.png", 0, 0, 32, 32), // invisible
	new EntityImageDescription("teletransporter.png", 32, 0, 16, 16), // yellow
    };

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
	destinationMapId = map.getId();
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
	buff.append(getSubtypeIndex());
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
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns whether or not the entity is currently resizable.
     * @return true if the entity is resizable
     */
    public boolean isResizable() {
	return !initialized || subtype == INVISIBLE;
    }

    /**
     * Returns whether two entities of this type can have different
     * sizes, even if they are not resizable.
     * @return true
     */
    public boolean isSizeVariable() {
	return true;
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
    public int getSubtypeIndex() {
	return subtype;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtypeIndex(int subtype) throws MapException {

	if (subtype != this.subtype) {

	    if (isResizable()) {
		setSize(16, 16);
	    }

	    this.subtype = subtype;
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
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	if (subtype == YELLOW) {
	    currentImageDescription.setRectangle(32, 0, 16, 16);
	}
	else if (subtype == BLUE) {
	    currentImageDescription.setRectangle(32, 16, 16, 16);
	}
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

	    int x = (int) (positionInMap.x * zoom);
	    int y = (int) (positionInMap.y * zoom);
	    int w = (int) (positionInMap.width * zoom);
	    int h = (int) (positionInMap.height * zoom);

	    g.setColor(new Color(240, 200, 80));
	    g.fillRect(x, y, w, h);

	    int dx1 = (int) ((positionInMap.x + positionInMap.width / 2 - 8) * zoom);
	    int dy1 = (int) ((positionInMap.y + positionInMap.height / 2 - 8) * zoom);
	    int dx2 = (int) (dx1 + 16 * zoom);
	    int dy2 = (int) (dy1 + 16 * zoom);

	    g.drawImage(resizableTeletransporterImage, dx1, dy1, dx2, dy2, 0, 0, 32, 32, null);

	    dx1 = (int) (positionInMap.x * zoom);
	    dy1 = (int) (positionInMap.y * zoom);
	    dx2 = (int) (dx1 + positionInMap.width * zoom);
	    dy2 = (int) (dy1 + positionInMap.height * zoom);

	    g.setColor(Color.black);
	    g.drawLine(dx1, dy1, dx2 - 1, dy1);
	    g.drawLine(dx1 + 3, dy1 + 3, dx2 - 4, dy1 + 3);
	    g.drawLine(dx1, dy2 - 1, dx2 - 1, dy2 - 1);
	    g.drawLine(dx1 + 3, dy2 - 4, dx2 - 4, dy2 - 4);
	    g.drawLine(dx1, dy1, dx1, dy2 - 1);
	    g.drawLine(dx1 + 3, dy1 + 3, dx1 + 3, dy2 - 4);
	    g.drawLine(dx2 - 1, dy1, dx2 - 1, dy2 - 1);
	    g.drawLine(dx2 - 4, dy1 + 3, dx2 - 4, dy2 - 4);

	    g.setColor(new Color(240, 215, 142));
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
