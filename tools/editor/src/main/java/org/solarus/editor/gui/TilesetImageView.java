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
import org.solarus.editor.entities.*;

/**
 * This component shows the tileset image and allows the user to select
 * tile patterns.
 */
public class TilesetImageView extends JComponent implements Observer, Scrollable {

    /**
     * Possible action the user is doing on this view.
     */
    private enum State {
        NORMAL,                   /**< Can select a tile. */
        DRAWING_NEW_PATTERN,      /**< Drawing a rectangle for a new tile. */
        MOVING_PATTERN            /**< Moving an existing tile to another place
                                   * in the PNG image. */
    }

    /**
     * Current action of the user.
     */
    private State state;

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * True if the tileset is editable, false if the user can only pick a tile.
     */
    private boolean editable;

    /**
     * The currently selected tile in the tileset.
     */
    private TilePattern currentSelectedTilePattern;

    /**
     * In states DRAWING_NEW_PATTERN and MOVING_PATTERN:
     * point where the dragging started.
     */
    private Point draggingStartPoint;

    /**
     * In states DRAWING_NEW_PATTERN and MOVING_PATTERN:
     * point where the dragging is currently.
     */
    private Point draggingCurrentPoint;

    /**
     * In states DRAWING_NEW_PATTERN and MOVING_PATTERN:
     * position of the tile pattern the user is creating
     * or moving.
     */
    private Rectangle currentArea;

    /**
     * In states DRAWING_NEW_PATTERN and MOVING_PATTERN:
     * true if the area is overlapping an existing tile pattern.
     * Is so, the tile pattern cannot be created or moved.
     */
    private boolean isCurrentAreaOverlapping;

    /**
     * In state MOVING_PATTERN:
     * the tile pattern being moved by the user.
     */
    private TilePattern patternBeingMoved;

    // Components.

    /**
     * A popup menu with an item "Create", displayed when the user selects a new tile pattern.
     */
    private JPopupMenu popupMenuCreate;

    /**
     * A popup menu with an item "Move here", displayed when the user moves a tile pattern.
     */
    private JPopupMenu popupMenuMove;

    /**
     * A popup menu with an item "Obstacle" and an item "Delete", displayed
     * when the user right-clicks on a tile pattern.
     */
    private JPopupMenu popupMenuSelectedTilePattern;

    /**
     * Items in the popup menu to set the type of ground of the selected tile pattern.
     */
    private JRadioButtonMenuItem[] itemsGround;

    /**
     * Constructor.
     * @param editable true to make the tileset editable, false otherwise
     * (if so, the user can only pick a tile pattern)
     */
    public TilesetImageView(boolean editable) {
        super();

        this.state = State.NORMAL;
        this.editable = editable;

        addMouseListener(new TilesetImageMouseListener());
        addMouseMotionListener(new TilesetImageMouseMotionListener());

        if (editable) {
            JMenuItem item;

            // popup menu to create a tile pattern
            popupMenuCreate = new JPopupMenu();

            item = new JMenuItem("Create (traversable)", Ground.TRAVERSABLE.getIcon());
            item.addActionListener(new ActionCreateTilePattern(Ground.TRAVERSABLE));
            popupMenuCreate.add(item);

            item = new JMenuItem("Create (wall)", Ground.WALL.getIcon());
            item.addActionListener(new ActionCreateTilePattern(Ground.WALL));
            popupMenuCreate.add(item);

            JMenuItem itemCancelCreate = new JMenuItem("Cancel");
            itemCancelCreate.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        tileset.unselectTilePattern();
                        startStateNormal();
                    }
                });
            popupMenuCreate.addSeparator();
            popupMenuCreate.add(itemCancelCreate);

            // Popup menu to move a tile pattern.
            popupMenuMove = new JPopupMenu();

            item = new JMenuItem("Move here");
            item.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    // Actually move the pattern.
                    try {
                        patternBeingMoved.setPositionInTileset(currentArea);
                    }
                    catch (TilesetException ex) {
                        // Should not happen: this view is supposed to
                        // draw a valid rectangle.
                        ex.printStackTrace();
                    }
                    startStateNormal();
                }
            });
            popupMenuMove.add(item);

            JMenuItem itemCancelMove = new JMenuItem("Cancel");
            itemCancelMove.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        startStateNormal();
                    }
                });
            popupMenuMove.addSeparator();
            popupMenuMove.add(itemCancelMove);

            // popup menu for a selected tile pattern
            popupMenuSelectedTilePattern = new JPopupMenu();
            itemsGround = new JRadioButtonMenuItem[Ground.values().length];
            ButtonGroup itemsObstacleGroup = new ButtonGroup();

            int i = 0;
            for (Ground ground: Ground.values()) {
                itemsGround[i] = new JRadioButtonMenuItem(ground.getHumanName(), ground.getIcon());
                itemsGround[i].addActionListener(new ActionChangeGround(ground));
                popupMenuSelectedTilePattern.add(itemsGround[i]);
                itemsObstacleGroup.add(itemsGround[i]);
                i++;
            }

            popupMenuSelectedTilePattern.addSeparator();

            item = new JMenuItem("Delete");
            item.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        tileset.removeTilePattern();
                    }
                });
            popupMenuSelectedTilePattern.add(item);
        }
    }

    /**
     * Returns the tileset's image size.
     * If no image is loaded, returns a default size.
     * @return the image size or a default size.
     */
    public Dimension getPreferredSize() {
        int width, height;

        if (!isImageLoaded()) {
            width = 0;
            height = 0;
        }
        else {
            Image scaledImage = tileset.getDoubleImage();
            width = scaledImage.getWidth(null);
            height = scaledImage.getHeight(null);
        }

        return new Dimension(width, height);
    }

    // interface Scrollable
    public Dimension getPreferredScrollableViewportSize() {
        return getPreferredSize();
    }

    public int getScrollableUnitIncrement(Rectangle visibleRect, int orientation, int direction) {
        return 16;
    }

    public int getScrollableBlockIncrement(Rectangle visibleRect, int orientation, int direction) {
        return 160;
    }

    public boolean getScrollableTracksViewportWidth() {
        return false;
    }

    public boolean getScrollableTracksViewportHeight() {
        return false;
    }

    /**
     * Sets the observed tileset.
     * @param tileset the current tileset, or null if there is no tileset loaded
     */
    public void setTileset(Tileset tileset) {

        if (tileset == null) {

            if (this.tileset != null) {
                // the tileset has just been closed

                this.tileset.deleteObserver(this);
                this.tileset = null;
                repaint();
            }
        }
        else if (tileset != this.tileset) {
            // a tileset has just been loaded

            if (this.tileset != null) {
                this.tileset.deleteObserver(this);
            }

            this.tileset = tileset;
            tileset.addObserver(this);

            // initialize the popup menu of the selected tile
            update(tileset.getSelectedTilePattern(), null);

            // load the tileset's image
            tileset.reloadImage();
            update(tileset, null);
        }

        setSize(getPreferredSize());
    }

    /**
     * This function is called when the tileset or the selected tile pattern changes.
     */
    public void update(Observable o, Object obj) {

        if (o instanceof Tileset) {
            // the tileset has changed

            TilePattern newSelectedTilePattern = tileset.getSelectedTilePattern();
            if (newSelectedTilePattern != currentSelectedTilePattern) {
                // observe the new selected tile

                if (currentSelectedTilePattern != null) {
                    currentSelectedTilePattern.deleteObserver(this);
                }

                if (newSelectedTilePattern != null) {
                    newSelectedTilePattern.addObserver(this);
                }

                currentSelectedTilePattern = newSelectedTilePattern;
            }

            // redraw the image
            repaint();
        }
        else if (o instanceof TilePattern) {
            // A tile pattern has changed or has been added.
            repaint();
        }
    }

    /**
     * Returns true if the image exists.
     * @return true if the tileset image exists
     */
    public boolean isImageLoaded() {
        return tileset != null && tileset.getDoubleImage() != null;
    }

    /**
     * Sets the state to normal.
     */
    private void startStateNormal() {

        state = State.NORMAL;
        draggingStartPoint = null;
        draggingCurrentPoint = null;
        currentArea = null;
        patternBeingMoved = null;
        repaint();
    }

    /**
     * Sets the state to drawing a new tile pattern area.
     * @param startPoint Where the user starts drawing the rectangle.
     */
    private void startStateDrawingNewArea(Point startPoint) {

        state = State.DRAWING_NEW_PATTERN;
        draggingStartPoint = startPoint;
        repaint();
    }

    /**
     * Sets the state to moving an existing tile pattern.
     * @param pattern The pattern to move.
     * @param startPoint Where the user starts dragging the pattern.
     */
    private void startStateMovingPattern(TilePattern pattern, Point startPoint) {

        state = State.MOVING_PATTERN;
        patternBeingMoved = pattern;
        draggingStartPoint = new Point(startPoint);
        currentArea = new Rectangle(pattern.getPositionInTileset());
        repaint();
    }

    /**
     * Changes the position of the tile pattern the user is creating or moving
     * If the specified area is the same than before, nothing is done.
     * @param newArea new position of the pattern.
     */
    public void setCurrentArea(Rectangle newArea) {

        if (!newArea.equals(this.currentArea)) {

            this.currentArea = new Rectangle(newArea);

            // Determine whether or not the new area is overlapping
            // an existing tile pattern.
            isCurrentAreaOverlapping = false;
            for (TilePattern pattern: tileset.getTilePatterns()) {

                if (pattern.getPositionInTileset().intersects(newArea)
                        && pattern != patternBeingMoved) {
                    isCurrentAreaOverlapping = true;
                    break;
                }
            }
            repaint();
        }
    }

    /**
     * Returns whether or not the user is selecting a new tile pattern.
     * @return true if the user is selecting a new tile pattern.
     */
    public boolean isDrawingNewTilePattern() {
        return state == State.DRAWING_NEW_PATTERN;
    }

    /**
     * This function is called to display the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {

        if (isImageLoaded()) { // the image exists

            TilePattern selectedTilePattern = null;
            Image scaledImage = tileset.getDoubleImage();

            // Draw the image of the tileset.
            g.drawImage(scaledImage, 0, 0, null);

            // Draw a rectangle depending on the current state.
            Rectangle rectangleToDraw = null;

            switch (state) {

            case DRAWING_NEW_PATTERN:
            case MOVING_PATTERN:

                // Draw the rectangle the user is dragging.
                rectangleToDraw = currentArea;

                if (rectangleToDraw == null
                    || rectangleToDraw.width == 0
                    || rectangleToDraw.height == 0
                    || isCurrentAreaOverlapping) {
                    // Invalid area.
                    g.setColor(Color.RED);
                }
                else {
                    // Valid area.
                    g.setColor(Color.GREEN);
                }
                break;

            case NORMAL:
                // Normal state: draw the selected pattern if any.
                selectedTilePattern = tileset.getSelectedTilePattern();

                if (selectedTilePattern != null) {
                    // An existing tile pattern is selected.
                    rectangleToDraw = selectedTilePattern.getPositionInTileset();
                    g.setColor(Color.BLUE);
                }
                break;

            }

            // Draw the additional rectangle.
            if (rectangleToDraw != null) {

                int x1 = rectangleToDraw.x * 2;
                int x2 = (rectangleToDraw.x + rectangleToDraw.width) * 2 - 1;
                int y1 = rectangleToDraw.y * 2;
                int y2 = (rectangleToDraw.y + rectangleToDraw.height) * 2 - 1;

                g.drawLine(x1, y1, x2, y1);
                g.drawLine(x2, y1, x2, y2);
                g.drawLine(x2, y2, x1, y2);
                g.drawLine(x1, y2, x1, y1);

                g.drawLine(x1, y1 + 1, x2, y1 + 1);
                g.drawLine(x2 - 1, y1, x2 - 1, y2);
                g.drawLine(x2, y2 - 1, x1, y2 - 1);
                g.drawLine(x1 + 1, y2, x1 + 1, y1);

                // for an animated tile pattern, also draw the separation between the 3 frames
                if (selectedTilePattern != null && selectedTilePattern.isMultiFrame()) {

                    if (selectedTilePattern.getAnimationSeparation() ==
                            TilePattern.AnimationSeparation.HORIZONTAL) {
                        int frame_width = (x2 - x1 + 1) / 3;
                        x1 += frame_width;
                        x2 -= frame_width;

                        g.drawLine(x2, y1, x2, y2);
                        g.drawLine(x1, y2, x1, y1);
                        g.drawLine(x2 - 1, y1, x2 - 1, y2);
                        g.drawLine(x1 + 1, y2, x1 + 1, y1);
                    }
                    else {
                        int frame_height = (y2 - y1 + 1) / 3;
                        y1 += frame_height;
                        y2 -= frame_height;

                        g.drawLine(x1, y1, x2, y1);
                        g.drawLine(x2, y2, x1, y2);
                        g.drawLine(x1, y1 + 1, x2, y1 + 1);
                        g.drawLine(x2, y2 - 1, x1, y2 - 1);
                    }
                }
            }

        }
        else if (tileset != null) {

            // a tileset is open but the image doesn't exist: draw an error message
            g.drawString("Please create a tileset image named:", 10, 20);
            g.drawString(tileset.getImageFile().getAbsolutePath() + "", 10, 40);
            g.drawString("Then click on this area to show the tileset image.", 10, 60);
        }
    }

    /**
     * The mouse listener associated to the tileset image.
     */
    private class TilesetImageMouseListener extends MouseAdapter {

        /**
         * This function is called when the user clicks on the tileset image.
         * If no tile pattern was selected, then the tile pattern clicked (if any) is selected.
         * For a right click: a popup menu allows to remove the tile pattern clicked
         * (if the tileset is editable).
         * @param mouseEvent information about the click
         */
        public void mouseClicked(MouseEvent mouseEvent) {
            if (isImageLoaded()) {

                Image scaledImage = tileset.getDoubleImage();

                int x = Math.min(Math.max(mouseEvent.getX(), 0),
                        scaledImage.getWidth(TilesetImageView.this)) / 2;
                int y = Math.min(Math.max(mouseEvent.getY(), 0),
                        scaledImage.getHeight(TilesetImageView.this)) / 2;

                // search the tile pattern clicked
                int clickedTileId = tileset.getTilePatternIdAt(x, y);

                // a tile pattern was just clicked
                if (clickedTileId > 0) {

                    // select the tile pattern
                    startStateNormal();
                    tileset.setSelectedTilePatternId(clickedTileId);

                    // right click: if the tileset is editable, we show a popup menu
                    if (mouseEvent.getButton() == MouseEvent.BUTTON3 && editable) {
                        Ground ground = tileset.getSelectedTilePattern().getGround();
                        itemsGround[ground.ordinal()].setSelected(true);
                        popupMenuSelectedTilePattern.show(TilesetImageView.this,
                                mouseEvent.getX(),
                                mouseEvent.getY());
                    }
                }
            }
            else if (tileset != null) {
                tileset.reloadImage();
            }
        }

        /**
         * This function is called when the mouse button is pressed on the tileset image.
         * Only left clicks are considered.
         * If a tile pattern was selected, it becomes unselected. Otherwise, the selection
         * for a new tile pattern begins (only if the tileset is editable).
         * @param mouseEvent information about the click
         */
        public void mousePressed(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            // Only consider left clicks.
            if (mouseEvent.getButton() == MouseEvent.BUTTON1) {

                Image scaledImage = tileset.getDoubleImage();

                int x = Math.min(Math.max(mouseEvent.getX(), 0),
                        scaledImage.getWidth(TilesetImageView.this)) / 2;
                int y = Math.min(Math.max(mouseEvent.getY(), 0),
                        scaledImage.getHeight(TilesetImageView.this)) / 2;

                switch (state) {

                case NORMAL:
                {
                    TilePattern selectedTilePattern = tileset.getSelectedTilePattern();

                    if (selectedTilePattern != null) {
                        // An existing tile pattern is selected.
                        if (selectedTilePattern.getPositionInTileset().contains(x,y)) {
                            // The user pressed the button inside: move it.
                            startStateMovingPattern(selectedTilePattern, new Point(x, y));
                        }
                        else {
                            // The user pressed the button outside: unselect it.
                            tileset.unselectTilePattern();
                        }
                    }

                    // Begin a selection.
                    if (state == State.NORMAL && editable) {
                        Point startPoint = new Point(
                                (x + 4) / 8 * 8,
                                (y + 4) / 8 * 8
                        );
                        startStateDrawingNewArea(startPoint);
                    }
                }
                    break;

                case DRAWING_NEW_PATTERN:
                    // A new tile pattern was being drawn: unselect it.
                    tileset.unselectTilePattern();
                    startStateNormal();

                    break;

                case MOVING_PATTERN:
                    startStateNormal();
                    break;
                }
            }
        }

        /**
         * This function is called when the mouse button is released on the tileset image.
         * Only left clicks are considered.
         * @param mouseEvent information about the click
         */
        public void mouseReleased(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            // only consider left clicks
            if (mouseEvent.getButton() == MouseEvent.BUTTON1) {

                switch (state) {

                case NORMAL:
                    break;

                case DRAWING_NEW_PATTERN:
                    // Keep the new area only if it exists.
                    if (currentArea != null
                            && currentArea.width > 0
                            && currentArea.height > 0
                            && !isCurrentAreaOverlapping) {

                        // The area is valid: show a popup menu with an item "Create".
                        popupMenuCreate.show(TilesetImageView.this,
                                mouseEvent.getX(),
                                mouseEvent.getY());
                    }
                    else {
                        // The area doesn't exist or is not valid: we cancel
                        // the selection.
                        tileset.unselectTilePattern();
                        startStateNormal();
                    }
                    break;

                case MOVING_PATTERN:
                    if (currentArea != null
                            && currentArea.width > 0
                            && currentArea.height > 0
                            && !isCurrentAreaOverlapping
                            && !currentArea.equals(patternBeingMoved.getPositionInTileset())) {
                        // The pattern was dropped in a valid area: show a popup menu
                        // with an item "Move here".
                        popupMenuMove.show(TilesetImageView.this,
                                mouseEvent.getX(),
                                mouseEvent.getY());
                    }
                    else {
                        // The pattern was dropped at its original place
                        // or at an invalid place.
                        startStateNormal();
                    }
                    break;
                }
            }
        }
    }

    /**
     * The mouse motion listener associated to the tileset image.
     */
    private class TilesetImageMouseMotionListener extends MouseMotionAdapter {

        /**
         * This method is called when a mouse button is pressed
         * on the component and then dragged.
         * It is called again until the mouse button is released.
         */
        public void mouseDragged(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            Image scaledImage = tileset.getDoubleImage();
            switch (state) {

            case NORMAL:
                break;

            case DRAWING_NEW_PATTERN:

                // compute the selected area
                Point selectionPreviousPoint = draggingCurrentPoint;
                draggingCurrentPoint = mouseEvent.getPoint();

                draggingCurrentPoint.x = Math.min(Math.max(
                        mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this));
                draggingCurrentPoint.y = Math.min(Math.max(
                        mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this));

                draggingCurrentPoint.x = (draggingCurrentPoint.x + 8) / 16 * 8;
                draggingCurrentPoint.y = (draggingCurrentPoint.y + 8) / 16 * 8;

                if (!draggingCurrentPoint.equals(selectionPreviousPoint)) {

                    Rectangle newTilePatternArea = new Rectangle();

                    // the selected area has changed: recalculate the rectangle
                    if (draggingStartPoint.x < draggingCurrentPoint.x) {
                        newTilePatternArea.x = draggingStartPoint.x;
                        newTilePatternArea.width = draggingCurrentPoint.x - draggingStartPoint.x;
                    }
                    else {
                        newTilePatternArea.x = draggingCurrentPoint.x;
                        newTilePatternArea.width = draggingStartPoint.x - draggingCurrentPoint.x;
                    }

                    if (draggingStartPoint.y < draggingCurrentPoint.y) {
                        newTilePatternArea.y = draggingStartPoint.y;
                        newTilePatternArea.height = draggingCurrentPoint.y - draggingStartPoint.y;
                    }
                    else {
                        newTilePatternArea.y = draggingCurrentPoint.y;
                        newTilePatternArea.height = draggingStartPoint.y - draggingCurrentPoint.y;
                    }

                    setCurrentArea(newTilePatternArea);
                }
                break;

            case MOVING_PATTERN:
            {
                draggingCurrentPoint = mouseEvent.getPoint();

                draggingCurrentPoint.x = Math.min(Math.max(
                        mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this));
                draggingCurrentPoint.y = Math.min(Math.max(
                        mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this));

                Rectangle newTilePatternArea = new Rectangle(currentArea);
                newTilePatternArea.x = patternBeingMoved.getX()
                        + draggingCurrentPoint.x - draggingStartPoint.x;
                newTilePatternArea.y = patternBeingMoved.getY()
                        + draggingCurrentPoint.y - draggingStartPoint.y;

                newTilePatternArea.x = (newTilePatternArea.x + 8) / 16 * 8;
                newTilePatternArea.y = (newTilePatternArea.y + 8) / 16 * 8;
                setCurrentArea(newTilePatternArea);
            }
            break;
            }
        }
    }

    /**
     * Action listener invoked when the user creates a tile pattern.
     */
    private class ActionCreateTilePattern implements ActionListener {

        /**
         * Ground property to set when the action is invoked.
         */
        private Ground ground;

        /**
         * Constructor.
         * @param ground Ground property of the tile pattern to create.
         */
        public ActionCreateTilePattern(Ground ground) {
            this.ground = ground;
        }

        /**
         * Method called when the user wants to create a tile pattern.
         */
        public void actionPerformed(ActionEvent ev) {
            try {
                if (isDrawingNewTilePattern() && !isCurrentAreaOverlapping) {
                    tileset.addTilePattern(currentArea, ground);
                    startStateNormal();
                }
            }
            catch (TilesetException e) {
                JOptionPane.showMessageDialog(null,
                                              "Unable to create the tile pattern: " + e.getMessage(),
                                              "Error",
                                              JOptionPane.ERROR_MESSAGE);
            }
        }
    }

    /**
     * Action listener invoked when the user changes the type of ground of a tile
     * pattern from the popup menu after a right click.
     */
    private class ActionChangeGround implements ActionListener {

        /**
         * Ground property to set when the action is invoked.
         */
        private Ground ground;

        /**
         * Constructor.
         * @param ground Ground property to set when the action is invoked.
         */
        public ActionChangeGround(Ground ground) {
            this.ground = ground;
        }

        /**
         * Method called when the user sets the type of obstacle of the selected tile pattern.
         */
        public void actionPerformed(ActionEvent ev) {
            TilePattern tilePattern = tileset.getSelectedTilePattern();
            Ground currentGround = tilePattern.getGround();

            if (currentGround != ground) {

                try {
                    tilePattern.setGround(ground);
                }
                catch (TilesetException e) {
                    JOptionPane.showMessageDialog(null,
                                                  "Unable to set this type of ground: " + e.getMessage(),
                                                  "Error",
                                                  JOptionPane.ERROR_MESSAGE);
                }
            }
        }
    }
}
