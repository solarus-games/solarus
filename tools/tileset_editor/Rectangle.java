package tileset_editor;

import java.io.*;

/**
 * This class represents a rectangle, identified by its coordinates and its dimensions.
 */
public class Rectangle implements Serializable {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 1L;

    /**
     * X coordinate of the rectangle.
     */
    public int x;

    /**
     * Y coordinate of the rectangle.
     */
    public int y;

    /**
     * Width of the rectangle.
     */
    public int width;

    /**
     * Height of the rectangle.
     */
    public int height;

    /**
     * Creates a new rectangle.
     * @param x x coordinate of the rectangle
     * @param y y coordinate of the rectangle
     * @param width width of the rectangle
     * @param height height of the rectangle
     */
    public Rectangle(int x, int y, int width, int height) {
	this.x = x;
	this.y = y;
	this.width = width;
	this.height = height;
    }

    /**
     * Returns whether or not a point is inside the rectangle.
     * @param x point's x
     * @param y point's y
     * @return true if the point is inside the rectangle, false otherwise
     */
    public boolean isPointInside(int x, int y) {
	return x >= this.x
	    && y >= this.y
	    && x < this.x + this.width
	    && y < this.y + this.height;
    }
    
    /**
     * Returns a string representation of the rectangle.
     * @return a string representation of the rectangle.
     */
    public String toString() {
	return "Position: (" + x + "," + y + "), size: (" + width + "," + height + ")";
    }
}
