package zsdx.entities;

import java.awt.*;
import java.awt.image.*;
import zsdx.*;

/**
 * This class describes the image to draw to represent an entity.
 * The image is described by its file name and a rectangle in this image.
 */
public class EntityImageDescription implements ImageObserver {

    // the description
    private String imageFileName;
    private Rectangle rectangle;

    // the image
    private Image image;

    /**
     * Constructor.
     * @param imageFileName name of the file containing the source image
     * @param x x coordinate of the top-left corner of the rectangle to extract
     * @param y y coordinate of the top-left corner of the rectangle to extract 
     * @param width width of the rectangle
     * @param height height of the rectangle
     */
    public EntityImageDescription(String imageFileName, int x, int y, int width, int height) {
	this.imageFileName = imageFileName;
	this.rectangle = new Rectangle(x, y, width, height);
	this.image = null;
    }

    /**
     * Copy constructor.
     * @param other another entity image description
     */
    public EntityImageDescription(EntityImageDescription other) {
	this.imageFileName = other.getImageFileName();
	this.rectangle = new Rectangle(other.getRectangle());
	this.image = null;
    }

    /**
     * Returns the name of the image file.
     * @return the name of the image file
     */
    public String getImageFileName() {
	return imageFileName;
    }
    
    /**
     * Sets the name of the image file.
     * @param imageFileName name of the image file
     */
    public void setImageFileName(String imageFileName) {
	this.imageFileName = imageFileName;
    }

    /**
     * Returns the rectangle representing the entity in the image.
     * @return the entity's rectangle in the image
     */
    public Rectangle getRectangle() {
	return rectangle;
    }
    
    /**
     * Sets the rectangle representing the entity in the image.
     * @param x x coordinate of the top-left corner of the rectangle to extract
     * @param y y coordinate of the top-left corner of the rectangle to extract 
     * @param width width of the rectangle
     * @param height height of the rectangle
     */
    public void setRectangle(int x, int y, int width, int height) {
	rectangle.setRect(x, y, width, height);
    }
    
    /**
     * Sets the x coordinate of the rectangle representing the entity in the image.
     * @param x x coordinate of the top-left corner of the rectangle
     */
    public void setX(int x) {
	rectangle.x = x;
    }
    
    /**
     * Sets the y coordinate of the rectangle representing the entity in the image.
     * @param y y coordinate of the top-left corner of the rectangle
     */
    public void setY(int y) {
	rectangle.y = y;
    }
    
    
    /**
     * Draws the image.
     * @param g the graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     * @param positionInDestinationImage position of the entity on the destination image
     */
    public void paint(Graphics g, double zoom, boolean showTransparency,
		      Rectangle positionInDestinationImage) {

	// get the image
	if (image == null) {
	    image = Project.getEditorImage(imageFileName);
	}
	
	// check the zoom
	int scale = (int) zoom;
	if (scale != 2) {
	    throw new UnsupportedOperationException("Zoom mode not yet supported: " + zoom);
	}
	
	// calculate the coordinates
	int dx1 = positionInDestinationImage.x * scale;
	int dy1 = positionInDestinationImage.y * scale;

	int dx2 = dx1 + positionInDestinationImage.width * scale;
	int dy2 = dy1 + positionInDestinationImage.height * scale;
	
	int sx1 = rectangle.x;
	int sx2 = sx1 + rectangle.width;

	int sy1 = rectangle.y;
	int sy2 = sy1 + rectangle.height;

	// display the entity
	if (showTransparency) {
	    g.drawImage(image, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, this);
	}
	else {
	    g.drawImage(image, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, MapEntity.bgColor, this);
	}
    }
    
    /**
     * This function is called when some requested information about the image comes.
     * @return true
     */
    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height) {
	return true;
    }
}
