package editors;

import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.util.*;

/**
 * Represents an entry point in a map.
 */
public class MapEntrance extends InteractiveEntity implements ImageObserver {
    
    /**
     * Icons of an entrance for each direction. 
     */
    private static ImageIcon[] icons;

    // load the icons
    static {
	String path = "editors/images/";

	icons = new ImageIcon[4];
	for (int i = 0; i < 4; i++) {
	    icons[i] = new ImageIcon(path + "map_entrance_" + i + ".png");
	}
    }

    /**
     * Creates a map entrance at the specified location.
     * @param x x coordinate of the entrance
     * @param y y coordinate of the entrance
     */
    public MapEntrance(int x, int y) {
	super(LAYER_LOW, x, y, 16, 16);

	setHotSpot(8, 16);
	setDirection(1);
    }

    /**
     * Creates a map entrance from a string.
     * @param tokenizer the string tokenizer, which has already parsed the common part of the string
     * (i.e. the layer, the coordinates and the type of entity have already been handled)
     * @throws ZSDXException if there is a syntax error in the string
     */
    public MapEntrance(StringTokenizer tokenizer) throws ZSDXException {
	this(0, 0);
	
	try {
	    setDirection(Integer.parseInt(tokenizer.nextToken()));
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Draws the entrance on the map editor.
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

	// TODO: make a 32*32 icon

	int dx2 = dx1 + positionInMap.width * scale;
	int dy2 = dy1 + positionInMap.height * scale;

	g.drawImage(icons[getDirection()].getImage(), dx1, dy1, dx2, dy2, 0, 0, 16, 16, this);
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
	buff.append(MapEntity.ENTITY_ENTRANCE);
	buff.append('\t');
	buff.append(getDirection());

	return buff.toString();
    }
    
    /**
     * Returns whether the entity has a direction.
     * @return true
     */
    public boolean hasDirection() {
	return true;
    }
}
