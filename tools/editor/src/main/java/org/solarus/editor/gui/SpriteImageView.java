/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import org.solarus.editor.*;

/**
 * This component shows the sprite image and allows the user to select
 * animation directions.
 */
public class SpriteImageView extends JComponent implements Observer, Scrollable {

    /**
     * Possible action the user is doing on this view.
     */
    private enum State {
        NORMAL,                   /**< Can select a direction. */
        DRAWING_NEW_DIRECTION,    /**< Drawing a rectangle for a direction. */
        MOVING_DIRECTION          /**< Moving an existing direction to another
                                   * place in the PNG image. */
    }

    /**
     * Current action of the user.
     */
    private State state;

    /**
     * The current sprite.
     */
    private Sprite sprite;

    /**
     * The currently selected direction in the sprite.
     */
    private SpriteAnimationDirection currentSelectedDirection;

    /**
     * In states DRAWING_NEW_DIRECTION and MOVING_DIRECTION:
     * point where the dragging started.
     */
    private Point draggingStartPoint;

    /**
     * In states DRAWING_DIRECTION and MOVING_DIRECTION:
     * point where the dragging is currently.
     */
    private Point draggingCurrentPoint;

    /**
     * In states DRAWING_NEW_DIRECTION and MOVING_DIRECTION:
     * position of the direction the user is creating
     * or moving.
     */
    private Rectangle currentArea;

    /**
     * In state MOVING_DIRECTION:
     * the direction being moved by the user.
     */
    private SpriteAnimationDirection directionBeingMoved;

    // Components.

    /**
     * A popup menu with an item "Create", displayed when the user selects a new direction.
     */
    private final JPopupMenu popupMenuCreate;

    /**
     * A popup menu with an item "Move here", displayed when the user moves a direction.
     */
    private final JPopupMenu popupMenuMove;

    /**
     * A popup menu with an item "Delete", displayed
     * when the user right-clicks on a direction.
     */
    private final JPopupMenu popupMenuSelectedDirection;

    /**
     * The zoom value of the view.
     */
    private Zoom zoom;

    /**
     * Constructor.
     */
    public SpriteImageView() {
        super();

        this.state = State.NORMAL;
        this.zoom = Zoom.get(2.0);

        addMouseListener(new SpriteImageMouseListener());
        addMouseMotionListener(new SpriteImageMouseMotionListener());
        MouseMiddleButtonScrollListener.install(this);

        JMenuItem item;

        // popup menu to create a direction
        popupMenuCreate = new JPopupMenu();

        item = new JMenuItem("Create direction");
        item.addActionListener(new ActionCreateDirection());
        popupMenuCreate.add(item);

        JMenuItem itemCancelCreate = new JMenuItem("Cancel");
        itemCancelCreate.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    sprite.unselectDirection();
                    startStateNormal();
                }
            });
        popupMenuCreate.addSeparator();
        popupMenuCreate.add(itemCancelCreate);

        // Popup menu to move a direction.
        popupMenuMove = new JPopupMenu();

        item = new JMenuItem("Move here");
        item.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                // Actually move the direction.
                try {
                    directionBeingMoved.setPosition(currentArea.getLocation());
                    sprite.setSaved(false);
                }
                catch (SpriteException ex) {
                    // Should not happen: this view is supposed to
                    // draw a valid rectangle.
                    ex.printStackTrace();
                }
                startStateNormal();
            }
        });
        popupMenuMove.add(item);

        item = new JMenuItem("Clone here");
        item.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                // Actually move the direction.
                try {
                    sprite.cloneDirection(currentArea.getLocation());
                }
                catch (SpriteException ex) {
                    JOptionPane.showMessageDialog(null,
                        "Unable to clone the direction here: " + ex.getMessage(),
                        "Error",
                        JOptionPane.ERROR_MESSAGE);
                }
                startStateNormal();
            }
        });
        popupMenuMove.add(item);

        JMenuItem itemCancelMove = new JMenuItem("Cancel");
        itemCancelMove.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    startStateNormal();
                }
            });
        popupMenuMove.addSeparator();
        popupMenuMove.add(itemCancelMove);

        // popup menu for a selected tile pattern
        popupMenuSelectedDirection = new JPopupMenu();

        item = new JMenuItem("Clone direction");
        item.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    sprite.cloneDirection();
                } catch (SpriteException ex) {
                    JOptionPane.showMessageDialog(null,
                        "Unable to clone the direction: " + ex.getMessage(),
                        "Error",
                        JOptionPane.ERROR_MESSAGE);
                }
            }
        });
        popupMenuSelectedDirection.add(item);

        item = new JMenuItem("Delete");
        item.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    try {
                        sprite.removeDirection();
                    } catch (SpriteException ex) {
                        JOptionPane.showMessageDialog(null,
                                              "Unable to remove the direction: " + ex.getMessage(),
                                              "Error",
                                              JOptionPane.ERROR_MESSAGE);
                    }
                }
            });
        popupMenuSelectedDirection.add(item);
    }

    /**
     * Returns the sprite animation's image size.
     * If no image is loaded, returns a default size.
     * @return the image size or a default size.
     */
    @Override
    public Dimension getPreferredSize() {
        int width, height;

        if (!isImageLoaded()) {
            width = 0;
            height = 0;
        }
        else {
            Image scaledImage = sprite.getScaledImage(zoom);
            width = scaledImage.getWidth(null);
            height = scaledImage.getHeight(null);
        }

        return new Dimension(width, height);
    }

    // interface Scrollable
    @Override
    public Dimension getPreferredScrollableViewportSize() {
        return getPreferredSize();
    }

    @Override
    public int getScrollableUnitIncrement(Rectangle visibleRect, int orientation, int direction) {
        return 16;
    }

    @Override
    public int getScrollableBlockIncrement(Rectangle visibleRect, int orientation, int direction) {
        return 160;
    }

    @Override
    public boolean getScrollableTracksViewportWidth() {
        return false;
    }

    @Override
    public boolean getScrollableTracksViewportHeight() {
        return false;
    }

    /**
     * Sets the observed sprite.
     * @param sprite the current sprite, or null if there is no sprite loaded
     */
    public void setSprite(Sprite sprite) {

        if (sprite == null) {

            if (this.sprite != null) {
                // the sprite has just been closed

                this.sprite.deleteObserver(this);
                this.sprite = null;
                repaint();
            }
        }
        else if (sprite != this.sprite) {
            // a sprite has just been loaded

            if (this.sprite != null) {
                this.sprite.deleteObserver(this);
            }

            this.sprite = sprite;
            sprite.addObserver(this);

            // initialize the popup menu of the selected direction
            update(sprite.getSelectedDirection(), null);  // TODO

            // load the sprite animation's image
            sprite.reloadImage();
        }

        setSize(getPreferredSize());
    }

    /**
     * This function is called when the sprite, the selected animation or direction changes.
     */
    @Override
    public void update(Observable o, Object obj) {

        if (o instanceof Sprite) {
            // the sprite has changed

            SpriteAnimationDirection newSelectedDirection = sprite.getSelectedDirection();
            if (newSelectedDirection != currentSelectedDirection) {
                // observe the new selected direction

                if (currentSelectedDirection != null) {
                    currentSelectedDirection.deleteObserver(this);
                }

                if (newSelectedDirection != null) {
                    newSelectedDirection.addObserver(this);
                }

                currentSelectedDirection = newSelectedDirection;
            }

            // redraw the image
            revalidate();
            repaint();
        }
        else if (o instanceof SpriteAnimationDirection) {
            // A direction has changed or has been added.
            revalidate();
            repaint();
        }
        else if (o == null && obj instanceof String) {

            String event = (String) obj;
            switch (event) {
                case ViewScroller.ZOOM_IN:
                    zoomIn();
                    break;
                case ViewScroller.ZOOM_OUT:
                    zoomOut();
                    break;
            }
        }
    }

    /**
     * Sets the zoom to next value if possible.
     */
    public void zoomIn() {
        if (!zoom.isLastValue()) {
            zoom = zoom.getNext();
            revalidate();
            repaint();
        }
    }

    /**
     * Sets the zoom to previous value if possible.
     */
    public void zoomOut() {
        if (!zoom.isFirstValue()) {
            zoom = zoom.getPrevious();
            revalidate();
            repaint();
        }
    }

    /**
     * Returns true if the image exists.
     * @return true if the sprite animation image exists
     */
    public boolean isImageLoaded() {
        return sprite != null && sprite.getImage() != null;
    }

    /**
     * Sets the state to normal.
     */
    private void startStateNormal() {

        state = State.NORMAL;
        draggingStartPoint = null;
        draggingCurrentPoint = null;
        currentArea = null;
        directionBeingMoved = null;
        repaint();
    }

    /**
     * Sets the state to drawing a new direction area.
     * @param startPoint Where the user starts drawing the rectangle.
     */
    private void startStateDrawingNewArea(Point startPoint) {

        state = State.DRAWING_NEW_DIRECTION;
        draggingStartPoint = startPoint;
        repaint();
    }

    /**
     * Sets the state to moving an existing direction.
     * @param direction the direction to move.
     * @param startPoint Where the user starts dragging the pattern.
     */
    private void startStateMovingDirection(SpriteAnimationDirection direction, Point startPoint) {

        state = State.MOVING_DIRECTION;
        directionBeingMoved = direction;
        draggingStartPoint = new Point(startPoint);
        currentArea = direction.getRect();
        repaint();
    }

    /**
     * Changes the position of the direction the user is creating or moving
     * If the specified area is the same than before, nothing is done.
     * @param newArea new position of the direction.
     */
    public void setCurrentArea(Rectangle newArea) {

        if (!newArea.equals(this.currentArea)) {

            this.currentArea = new Rectangle(newArea);
            repaint();
        }
    }

    /**
     * Returns whether or not the user is selecting a new direction.
     * @return true if the user is selecting a new direction.
     */
    public boolean isDrawingNewDirection() {
        return state == State.DRAWING_NEW_DIRECTION;
    }

    /**
     * This function is called to display the component.
     * @param g the graphic context
     */
    @Override
    public void paint(Graphics g) {

        if (isImageLoaded()) { // the image exists

            SpriteAnimationDirection selectedDirection = sprite.getSelectedDirection();
            Image scaledImage = sprite.getScaledImage(zoom);

            // Draw the image of the sprite.
            g.drawImage(scaledImage, 0, 0, null);

            Rectangle rectangleToDraw = null;

            switch (state) {

            case DRAWING_NEW_DIRECTION:
            case MOVING_DIRECTION:

                rectangleToDraw = currentArea;

                if (currentArea == null
                    || currentArea.width == 0
                    || currentArea.height == 0) {
                    // Invalid area.
                    g.setColor(Color.RED);
                }
                else {
                    // Valid area.
                    g.setColor(Color.GREEN);
                }
                break;

            case NORMAL:

                if (selectedDirection != null) {
                    rectangleToDraw = selectedDirection.getRect();
                }

                g.setColor(Color.BLUE);
                break;

            }

            if (rectangleToDraw != null) {

                double zoomValue = zoom.getValue();
                int x1 = (int) Math.round(rectangleToDraw.x * zoomValue);
                int x2 = (int) Math.round((rectangleToDraw.x + rectangleToDraw.width) * zoomValue) - 1;
                int y1 = (int) Math.round(rectangleToDraw.y * zoomValue);
                int y2 = (int) Math.round((rectangleToDraw.y + rectangleToDraw.height) * zoomValue) - 1;

                // draw the top & left lines
                g.drawLine(x1, y1, x2, y1);
                g.drawLine(x1, y2, x1, y1);

                g.drawLine(x1, y1 + 1, x2, y1 + 1);
                g.drawLine(x1 + 1, y2, x1 + 1, y1);

                // have a selection (MOVING_DIRECTION or NORMAL state)
                if (selectedDirection != null) {

                    int dx = 0, dy = 0;
                    // MOVE mode
                    if (rectangleToDraw == currentArea) {

                        Rectangle rect = selectedDirection.getRect();
                        dx = rectangleToDraw.x - rect.x;
                        dy = rectangleToDraw.y - rect.y;
                    }

                    // Draw rectangles.
                    for (Rectangle rect: selectedDirection.getRects()) {

                        x1 = (int) Math.round((dx + rect.x) * zoomValue);
                        x2 = (int) Math.round((dx + rect.x + rect.width) * zoomValue) - 1;
                        y1 = (int) Math.round((dy + rect.y) * zoomValue);
                        y2 = (int) Math.round((dy + rect.y + rect.height) * zoomValue) - 1;

                        g.drawLine(x2, y1, x2, y2);
                        g.drawLine(x2, y2, x1, y2);

                        g.drawLine(x2 - 1, y1, x2 - 1, y2);
                        g.drawLine(x2, y2 - 1, x1, y2 - 1);
                    }
                } // DRAWING_NEW_DIRECTION state
                else if (rectangleToDraw == currentArea) {

                    g.drawLine(x2, y1, x2, y2);
                    g.drawLine(x2, y2, x1, y2);

                    g.drawLine(x2 - 1, y1, x2 - 1, y2);
                    g.drawLine(x2, y2 - 1, x1, y2 - 1);
                }
            }
        }
        else if (sprite != null) {

            // a sprite is open but the image doesn't exist: draw an error message
            g.drawString("Image doesn't exist !", 10, 20);
        }
    }

    /**
     * Converts a position value from the view coordinates to the image coordinate system.
     * The image coordinate system differs from the image view coordinate system
     * because of the zoom.
     * @param x The x value relative to the image view.
     * @param y The y value relative to the image view.
     * @return The position value relative to the image.
     */
    public Point getMouseInImage (int x, int y) {

        if (isImageLoaded()) {
            double zoomValue = zoom.getValue();
            return new Point((int) (x / zoomValue), (int) (y / zoomValue));
        }

        return new Point(0, 0);
    }

    /**
     * The mouse listener associated to the sprite image.
     */
    private class SpriteImageMouseListener extends MouseAdapter {

        /**
         * This function is called when the user clicks on the sprite image.
         * If no direction was selected, then the direction clicked (if any) is selected.
         * For a right click: a popup menu allows to remove the direction clicked
         * @param mouseEvent information about the click
         */
        @Override
        public void mouseClicked(MouseEvent mouseEvent) {
            if (isImageLoaded()) {

                Image scaledImage = sprite.getScaledImage(zoom);

                double zoomValue = zoom.getValue();
                int x = (int) (Math.min(Math.max(mouseEvent.getX(), 0),
                        scaledImage.getWidth(SpriteImageView.this)) / zoomValue);
                int y = (int) (Math.min(Math.max(mouseEvent.getY(), 0),
                        scaledImage.getHeight(SpriteImageView.this)) / zoomValue);

                int clickedDirectionNb = sprite.getSelectedDirectionNb();
                SpriteAnimationDirection direction = sprite.getSelectedDirection();
                // if no selected direction or the position isn't in this direction
                if (direction == null || !direction.contains(x, y)) {
                    // search the direction clicked
                    clickedDirectionNb = sprite.getDirectionNbAt(x, y);
                }

                // a direction was just clicked
                if (clickedDirectionNb >= 0) {

                    // select the direction
                    startStateNormal();
                    try {
                        sprite.setSelectedDirectionNb(clickedDirectionNb);
                        if (mouseEvent.getButton() == MouseEvent.BUTTON3) {
                            popupMenuSelectedDirection.show(SpriteImageView.this,
                                    mouseEvent.getX(),
                                    mouseEvent.getY());
                        }
                    } catch (SpriteException ex) {
                        JOptionPane.showMessageDialog(null,
                                              "Unable to select the direction: " + ex.getMessage(),
                                              "Error",
                                              JOptionPane.ERROR_MESSAGE);
                    }
                }
            }
            else if (sprite != null) {
                sprite.reloadImage();
            }
        }

        /**
         * This function is called when the mouse button is pressed on the sprite animation image.
         * Only left clicks are considered.
         * If a direction was selected, it becomes unselected. Otherwise, the selection
         * for a new direction begins.
         * @param mouseEvent information about the click
         */
        @Override
        public void mousePressed(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            // Only consider left clicks.
            if (mouseEvent.getButton() == MouseEvent.BUTTON1) {

                Image scaledImage = sprite.getScaledImage(zoom);

                double zoomValue = zoom.getValue();
                int x = (int) (Math.min(Math.max(mouseEvent.getX(), 0),
                        scaledImage.getWidth(SpriteImageView.this)) / zoomValue);
                int y = (int) (Math.min(Math.max(mouseEvent.getY(), 0),
                        scaledImage.getHeight(SpriteImageView.this)) / zoomValue);

                switch (state) {

                case NORMAL:
                {
                    SpriteAnimationDirection selectedDirection = sprite.getSelectedDirection();

                    if (selectedDirection != null) {
                        // An existing direction is selected.
                        if (selectedDirection.contains(x,y)) {
                            // The user pressed the button inside: move it.
                            startStateMovingDirection(selectedDirection, new Point(x, y));
                        }
                        else {
                            // The user pressed the button outside: unselect it.
                            sprite.unselectDirection();
                        }
                    }

                    // Begin a selection.
                    if (state == State.NORMAL) {
                        Point startPoint = new Point(
                                (x + 4) / 8 * 8,
                                (y + 4) / 8 * 8
                        );
                        startStateDrawingNewArea(startPoint);
                    }
                }
                    break;

                case DRAWING_NEW_DIRECTION:
                    // A new direction was being drawn: unselect it.
                    sprite.unselectDirection();
                    startStateNormal();

                    break;

                case MOVING_DIRECTION:
                    startStateNormal();
                    break;
                }
            }
        }

        /**
         * This function is called when the mouse button is released on the sprite animation image.
         * Only left clicks are considered.
         * @param mouseEvent information about the click
         */
        @Override
        public void mouseReleased(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            // only consider left clicks
            if (mouseEvent.getButton() == MouseEvent.BUTTON1) {

                switch (state) {

                case NORMAL:
                    break;

                case DRAWING_NEW_DIRECTION:
                    // Keep the new area only if it exists.
                    if (currentArea != null
                            && currentArea.width > 0
                            && currentArea.height > 0) {

                        // The area is valid: show a popup menu with an item "Create".
                        popupMenuCreate.show(SpriteImageView.this,
                                mouseEvent.getX(),
                                mouseEvent.getY());
                    }
                    else {
                        // The area doesn't exist or is not valid: we cancel
                        // the selection.
                        sprite.unselectDirection();
                        startStateNormal();
                    }
                    break;

                case MOVING_DIRECTION:
                    if (currentArea != null
                            && currentArea.width > 0
                            && currentArea.height > 0
                            && !currentArea.equals(directionBeingMoved.getRect())) {
                        // The pattern was dropped in a valid area: show a popup menu
                        // with an item "Move here".
                        popupMenuMove.show(SpriteImageView.this,
                                mouseEvent.getX(),
                                mouseEvent.getY());
                    }
                    else {
                        // The direction was dropped at its original place
                        // or at an invalid place.
                        startStateNormal();
                    }
                    break;
                }
            }
        }
    }

    /**
     * The mouse motion listener associated to the sprite animation image.
     */
    private class SpriteImageMouseMotionListener extends MouseMotionAdapter {

        /**
         * This method is called when a mouse button is pressed
         * on the component and then dragged.
         * It is called again until the mouse button is released.
         */
        @Override
        public void mouseDragged(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            Image scaledImage = sprite.getScaledImage(zoom);
            double zoomValue = zoom.getValue();

            switch (state) {

            case NORMAL:
                break;

            case DRAWING_NEW_DIRECTION:

                // compute the selected area
                Point selectionPreviousPoint = draggingCurrentPoint;
                draggingCurrentPoint = mouseEvent.getPoint();

                draggingCurrentPoint.x = (int) (Math.min(Math.max(mouseEvent.getX(), 0),
                        scaledImage.getWidth(SpriteImageView.this)) / zoomValue * 2);
                draggingCurrentPoint.y = (int) (Math.min(Math.max(mouseEvent.getY(), 0),
                        scaledImage.getHeight(SpriteImageView.this)) / zoomValue * 2);

                draggingCurrentPoint.x = (draggingCurrentPoint.x + 8) / 16 * 8;
                draggingCurrentPoint.y = (draggingCurrentPoint.y + 8) / 16 * 8;

                if (!draggingCurrentPoint.equals(selectionPreviousPoint)) {

                    Rectangle newDirectionArea = new Rectangle();

                    // the selected area has changed: recalculate the rectangle
                    if (draggingStartPoint.x < draggingCurrentPoint.x) {
                        newDirectionArea.x = draggingStartPoint.x;
                        newDirectionArea.width = draggingCurrentPoint.x - draggingStartPoint.x;
                    }
                    else {
                        newDirectionArea.x = draggingCurrentPoint.x;
                        newDirectionArea.width = draggingStartPoint.x - draggingCurrentPoint.x;
                    }

                    if (draggingStartPoint.y < draggingCurrentPoint.y) {
                        newDirectionArea.y = draggingStartPoint.y;
                        newDirectionArea.height = draggingCurrentPoint.y - draggingStartPoint.y;
                    }
                    else {
                        newDirectionArea.y = draggingCurrentPoint.y;
                        newDirectionArea.height = draggingStartPoint.y - draggingCurrentPoint.y;
                    }

                    setCurrentArea(newDirectionArea);
                }
                break;

            case MOVING_DIRECTION:
            {
                draggingCurrentPoint = mouseEvent.getPoint();

                draggingCurrentPoint.x = (int) (Math.min(Math.max(mouseEvent.getX(), 0),
                        scaledImage.getWidth(SpriteImageView.this)) / zoomValue * 2);
                draggingCurrentPoint.y = (int) (Math.min(Math.max(mouseEvent.getY(), 0),
                        scaledImage.getHeight(SpriteImageView.this)) / zoomValue * 2);

                Rectangle newDirectionArea = new Rectangle(currentArea);
                newDirectionArea.x = directionBeingMoved.getPosition().x
                        + draggingCurrentPoint.x - draggingStartPoint.x;
                newDirectionArea.y = directionBeingMoved.getPosition().y
                        + draggingCurrentPoint.y - draggingStartPoint.y;

                newDirectionArea.x = (newDirectionArea.x + 8) / 16 * 8;
                newDirectionArea.y = (newDirectionArea.y + 8) / 16 * 8;
                setCurrentArea(newDirectionArea);
            }
            break;
            }
        }
    }

    /**
     * Action listener invoked when the user creates a direction.
     */
    private class ActionCreateDirection implements ActionListener {

        /**
         * Constructor.
         */
        public ActionCreateDirection() {
        }

        /**
         * Method called when the user wants to create a direction.
         */
        @Override
        public void actionPerformed(ActionEvent ev) {
            try {
                if (isDrawingNewDirection()) {
                    sprite.addDirection(currentArea);
                    startStateNormal();
                }
            }
            catch (SpriteException e) {
                JOptionPane.showMessageDialog(null,
                                              "Unable to create the direction: " + e.getMessage(),
                                              "Error",
                                              JOptionPane.ERROR_MESSAGE);
            }
        }
    }
}
