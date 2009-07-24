/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package zsdx.entities;

import java.awt.*;
import zsdx.*;

/**
 * This class describes the image to draw to represent an entity.
 * The image is described by its file name and a rectangle in this image.
 */
public class EntityImageDescription {

    // the description
    private String imageFileName;
    private Rectangle rectangle;

    // the image
    private Image image;

    private boolean relativeToEditor;

    /**
     * Constructor.
     * @param imageFileName name of the file containing the source image
     * @param x x coordinate of the top-left corner of the rectangle to extract
     * @param y y coordinate of the top-left corner of the rectangle to extract 
     * @param width width of the rectangle
     * @param height height of the rectangle
     */
    public EntityImageDescription(String imageFileName, int x, int y, int width, int height) {
	this.rectangle = new Rectangle(x, y, width, height);
	this.image = null;
	this.imageFileName = imageFileName;
	this.relativeToEditor = true;
    }

    /**
     * Copy constructor.
     * @param other another entity image description
     */
    public EntityImageDescription(EntityImageDescription other) {
	this.rectangle = new Rectangle(other.getRectangle());
	this.image = null;
	this.imageFileName = other.getImageFileName();
	this.relativeToEditor = other.relativeToEditor;
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
     * @param imageFileName name of the image file, relative to the editor images path
     */
    public void setImageFileName(String imageFileName) {
	setImageFileName(imageFileName, true);
    }
    
    /**
     * Sets the name of the image file.
     * @param imageFileName name of the image file
     * @param relativeToEditor true to consider the image file name relative
     * to the editor images directory, false to make it relative to the project data directory
     */
    public void setImageFileName(String imageFileName, boolean relativeToEditor) {
	this.imageFileName = imageFileName;
	this.relativeToEditor = relativeToEditor;
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
     * Sets the coordinates of the rectangle representing the entity in the image.
     * @param x x coordinate of the top-left corner of the rectangle
     * @param y y coordinate of the top-left corner of the rectangle
     */
    public void setXY(int x, int y) {
      setX(x);
      setY(y);
    }

    /**
     * Sets the size of the rectangle representing the entity in the image.
     * @param width width of the rectangle
     * @param height height of the rectangle
     */
    public void setSize(int width, int height) {
      rectangle.width = width;
      rectangle.height = height;
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

	  if (relativeToEditor) {
	    image = Project.getEditorImage(imageFileName);
	  }
	  else {
	    image = Project.getProjectImage(imageFileName);
	  }
	}

	// calculate the coordinates
	int dx1 = (int) (positionInDestinationImage.x * zoom);
	int dy1 = (int) (positionInDestinationImage.y * zoom);

	int dx2 = (int) (dx1 + positionInDestinationImage.width * zoom);
	int dy2 = (int) (dy1 + positionInDestinationImage.height * zoom);
	
	int sx1 = rectangle.x;
	int sx2 = sx1 + rectangle.width;

	int sy1 = rectangle.y;
	int sy2 = sy1 + rectangle.height;

	// display the entity
	if (showTransparency) {
	    g.drawImage(image, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, null);
	}
	else {
	    g.drawImage(image, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, MapEntity.bgColor, null);
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
