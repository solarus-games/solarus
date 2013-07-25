package org.solarus.editor.entities;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Rectangle;

import org.solarus.editor.Map;
import org.solarus.editor.MapException;
import org.solarus.editor.Project;

/**
 * An horizontal or vertical separation between two parts of the map.
 * It acts like an invisible wall for the camera.
 */
public class Separator extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("separator.png", 0, 0, 32, 32)
    };

    /**
     * Resizable pattern of a separator.
     */
    private static Image patternImage;

    /**
     * Unitary size of a separator.
     */
    private static final Dimension unitarySize = new Dimension(16, 16);

    /**
     * Creates a new separator.
     * @param map The map.
     */
    public Separator(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns whether or not the entity is currently resizable.
     * @return true if the entity is resizable.
     */
    public boolean isResizable() {
        return true;
    }

    /**
     * Returns whether or not the entity can be resized by extending it
     * horizontally or vertically
     * @param direction One of the two main directions (0: horizontal, 1: vertical).
     * @return Whether or not the entity can be expanded in that direction.
     */
    public boolean isExtensible(int extensionDirection) {
        // TODO change the prototype of this method to something more precise
        // (the exact size wanted).

        if (extensionDirection == 0) {
            // Extend horizontally.
            return getHeight() == getUnitarySize().height;
        }

        else {
            // Extend vertically.
            return getWidth() == getUnitarySize().width;
        }
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
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        if (getWidth() != 16 && getHeight() != 16) {
            throw new MapException("The width or the height must be 16");
        }
    }

    /**
     * Draws the entity on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    @Override
    public void paint(Graphics g, double zoom, boolean showTransparency) {

        if (patternImage == null) {
            patternImage = Project.getEditorImage("resizable_separator.png");
        }

        Rectangle positionInMap = getPositionInMap();
        int x1 = (int) (positionInMap.x * zoom);
        int y1 = (int) (positionInMap.y * zoom);
        int w = (int) (positionInMap.width * zoom);
        int h = (int) (positionInMap.height * zoom);
        int x2 = x1 + w;
        int y2 = y1 + h;
        int step = (int) (16 * zoom);

        for (int x = x1; x < x2; x += step) {
            for (int y = y1; y < y2 ; y += step) {
                g.drawImage(patternImage, x, y, x + step, y + step, 0, 0, 16, 16, null);
            }
        }

        Color outlineColor = new Color(142, 142, 142);
        drawEntityOutline(g, zoom, outlineColor);
    }
}
