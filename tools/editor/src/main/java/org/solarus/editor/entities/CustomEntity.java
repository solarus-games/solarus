package org.solarus.editor.entities;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;

import org.solarus.editor.*;

/**
 * A map entity fully defined by scripts.
 */
public class CustomEntity extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("entity_custom.png", 0, 0, 32, 32)
    };

    /**
     * Resizable image of this entity type when there is no sprite.
     */
    private static Image resizableImage;

    /**
     * The sprite representing this entity (if any).
     */
    private Sprite sprite;

    /**
     * Unitary size of a custom entity.
     */
    private static final Dimension unitarySize = new Dimension(8, 8);

    /**
     * Creates a new custom entity.
     * @param map The map.
     */
    public CustomEntity(Map map) throws MapException {
        super(map, 16, 16);
        setDirection(3);  // Look to the south initially.
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {

        if (sprite == null) {
            return new Point(8, 13);
        }

        int spriteNumDirections = sprite.getAnimation(
                sprite.getDefaultAnimationName()).getNbDirections();

        if (getDirection() < spriteNumDirections) {
            return sprite.getOrigin(null, getDirection());
        }
        else {
            return sprite.getOrigin(null, 0);
        }
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4.
     */
    public int getNbDirections() {
        return 4;
    }

    /**
     * Returns whether or not the entity is currently resizable.
     * @return true if the entity is resizable.
     */
    public boolean isResizable() {
        return true;
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return (8, 8)
     */
    public Dimension getUnitarySize() {
        return unitarySize;
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("sprite", true, null);
        createStringProperty("model", true, null);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        String model = getStringProperty("model");
        Resource entityResource = Project.getResource(ResourceType.ENTITY);
        if (model != null
                && !entityResource.exists(model)) {
            throw new MapException("Unknown custom entity model: '" + model + "'");
        }
    }

    /**
     * Notifies this entity that a property specific to its type has just changed.
     * Does nothing by default.
     * @param name Name of the property that has changed.
     * @param value The new value.
     */
    protected void notifyPropertyChanged(String name, String value) throws MapException {

        if (name.equals("sprite")) {

            if (isValidSpriteName(value)) {
                sprite = new Sprite(value, getMap());
            }
            else {
                sprite = null;
            }
        }
    }

    /**
     * Draws the entity on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

        if (sprite != null) {
            // Display the sprite.
            int direction = getDirection();
            int spriteNumDirections = sprite.getAnimation(
                    sprite.getDefaultAnimationName()).getNbDirections();

            if (direction >= spriteNumDirections) {
                direction = 0;
            }
            sprite.paint(g, zoom, showTransparency,
                    getX(), getY(), null, direction, 0);
        }
        else {
            // Draw the resizable icon.
            if (resizableImage == null) {
                resizableImage = Project.getEditorImageOrEmpty("entity_custom_resizable.png");
            }

            Rectangle positionInMap = getPositionInMap();
            int x = (int) (positionInMap.x * zoom);
            int y = (int) (positionInMap.y * zoom);
            int w = (int) (positionInMap.width * zoom);
            int h = (int) (positionInMap.height * zoom);

            g.setColor(new Color(224, 108, 72));
            g.fillRect(x, y, w, h);

            int dx1 = (int) ((positionInMap.x + positionInMap.width / 2 - 8) * zoom);
            int dy1 = (int) ((positionInMap.y + positionInMap.height / 2 - 8) * zoom);
            int dx2 = (int) (dx1 + 16 * zoom);
            int dy2 = (int) (dy1 + 16 * zoom);

            int sx1 = 0;
            int sx2 = sx1 + 32;

            g.drawImage(resizableImage, dx1, dy1, dx2, dy2, sx1, 0, sx2, 32, null);

            drawEntityOutline(g, zoom, new Color(184, 96, 96));
        }
    }
}

