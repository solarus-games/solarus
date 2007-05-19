package tileset_editor;

import java.io.*;

/**
 * This class represents a rectangle, identified by its coordinates and its dimensions.
 */
public class Rectangle implements Serializable {

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
    
}
