package zsdx;

import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.util.*;

/**
 * Represents an exit point in a map.
 */
public class MapExit extends InteractiveEntity implements ImageObserver {

    // TODO private fields

    /**
     * Icon of an exit point.
     */
    private static ImageIcon icon = new ImageIcon("zsdx/images/map_exit.png");;

    /**
     * Creates an exit at the specified location.
     * @param map the map
     * @param x x coordinate of the exit
     * @param y y coordinate of the exit
     */
    public MapExit(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);

	// TODO fields
    }

    /**
     * Creates a map exit from a string.
     * @param tokenizer the string tokenizer, which has already parsed the common part of the string
     * (i.e. the layer, the coordinates and the type of entity have already been handled)
     * @param map the map
     * @throws ZSDXException if there is a syntax error in the string
     */
    public MapExit(StringTokenizer tokenizer, Map map) throws ZSDXException {
	this(null, 0, 0);

	// TODO
	/*	
	try {
	    // TODO
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
	*/
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_EXIT
     */
    public int getType() {
	return ENTITY_EXIT;
    }

    /**
     * Returns whether the entity has a direction.
     * @return false
     */
    public boolean hasDirection() {
	return false;
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
	
	int dx1 = positionInMap.width * scale;
	int dy1 = positionInMap.height * scale;
	int dx2 = dx1 + 16;
	int dy2 = dy1 + 16;

	if (showTransparency) {
	    g.drawImage(icon.getImage(), dx1, dy1, dx2, dy2, this);
	}
	else {

	}
    }

    /**
     * This function is called when some requested information about the image comes.
     * @return true
     */
    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height) {
	return true;
    }
    
    /**
     * Returns a string describing this map entrance.
     * @return a string representation of the map entrance
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string (i.e. the layer and the coordinates)
	buff.append(super.toString());

	buff.append('\t');
	buff.append(MapEntity.ENTITY_EXIT);

	// TODO

	return buff.toString();
    }
}
