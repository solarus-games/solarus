package zsdx;

import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.util.*;

/**
 * Represents an exit point in a map.
 */
public class MapExit extends InteractiveEntity implements ImageObserver {

    private int transition;
    private String destinationMapId;
    private String entranceName;

    /**
     * Icon of an exit point.
     */
    private static ImageIcon icon = new ImageIcon("zsdx/images/map_exit.png");;

    /**
     * Creates a new exit at the specified location.
     * @param map the map
     * @param x x coordinate of the exit
     * @param y y coordinate of the exit
     */
    public MapExit(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);
    }

    /**
     * Creates an existing map exit from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public MapExit(Map map, StringTokenizer tokenizer) throws ZSDXException {
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
     * Returns a string describing this map entrance.
     * @return a string representation of the map entrance
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
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_EXIT
     */
    public int getType() {
	return ENTITY_EXIT;
    }

    /**
     * Returns the number of directions of the entity.
     * @return 0 (an exit has no direction)
     */
    public int getNbDirections() {
	return 0;
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
     */
    public void setDestinationMapId(String destinationMapId) {
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
	
	int scale = (int) zoom;

	if (scale != 2) {
	    throw new UnsupportedOperationException("Zoom mode not yet supported: " + zoom);
	}
	
	int dx1 = positionInMap.x * scale;
	int dy1 = positionInMap.y * scale;
	int dx2 = dx1 + positionInMap.width * scale;
	int dy2 = dy1 + positionInMap.height * scale;

	g.drawImage(icon.getImage(), dx1, dy1, dx2, dy2, 0, 0, 32, 32, this);
    }

    /**
     * This function is called when some requested information about the image comes.
     * @return true
     */
    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height) {
	return true;
    }
}
