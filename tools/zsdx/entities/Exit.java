package zsdx.entities;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents an exit point in a map.
 */
public class Exit extends ActiveEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Exit";

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
	{new EntityImageDescription("exit_32.png", 0, 0, 32, 32)};

    /**
     * Unitary size of an exit.
     */
    private static final Dimension unitarySize = new Dimension(16, 16);
    
    private int transition;
    private String destinationMapId;
    private String entranceName;

    /**
     * Icon of an exit point.
     */
    private static ImageIcon icon = new ImageIcon("zsdx/images/exit.png");

    /**
     * Creates a new exit at the specified location.
     * @param map the map
     * @param x x coordinate of the exit
     * @param y y coordinate of the exit
     */
    public Exit(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);
	
	// default field values
	transition = Transition.IMMEDIATE;
	destinationMapId = "";
	entranceName = "";
    }

    /**
     * Creates an existing map exit from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public Exit(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);

	try {
	    this.transition = Integer.parseInt(tokenizer.nextToken());
	    this.destinationMapId = tokenizer.nextToken();
	    this.entranceName = tokenizer.nextToken();	    
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }
    
    /**
     * Returns a string describing this exit.
     * @return a string representation of the exit.
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of a map exit
	buff.append('\t');
	buff.append(getTransition());
	buff.append('\t');
	buff.append(getDestinationMapId());
	buff.append('\t');
	buff.append(getEntranceName());

	return buff.toString();
    }

    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public boolean isValid() {
	return super.isValid()
	    && destinationMapId.length() != 0 && entranceName.length() != 0;	
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_EXIT
     */
    public int getType() {
	return ENTITY_EXIT;
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
     * A map exit is resizable (i.e. its rectangle can have any size).
     * @return true
     */
    public boolean isResizable() {
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
     * Returns the type of transition between the two maps.
     * @return the transition type
     */
    public int getTransition() {
	return transition;
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
	
	if (destinationMapId.equals(map.getId())) {
	    throw new MapException("The destination map cannot be the same map.");
	}
	
	this.destinationMapId = destinationMapId;

	setChanged();
	notifyObservers();
    }
    
    /**
     * Returns the entrance on the destination map.
     * @return the name of the entrance on the destination map
     */
    public String getEntranceName() {
	return entranceName;
    }

    /**
     * Sets the entrance on the destination map.
     * @param entranceName the name of the entrance on the destination map
     */
    public void setEntranceName(String entranceName) {
	this.entranceName = entranceName;

	setChanged();
	notifyObservers();
    }

    /**
     * Draws the exit on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

	int dx1 = (int) (positionInMap.x * zoom);
	int dy1 = (int) (positionInMap.y * zoom);
	int dx2 = (int) (dx1 + positionInMap.width * zoom);
	int dy2 = (int) (dy1 + positionInMap.height * zoom);
	
	g.drawImage(icon.getImage(), dx1, dy1, dx2, dy2, 0, 0, 32, 32, null);
	
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
}
