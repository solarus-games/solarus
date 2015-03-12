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
import java.util.ArrayList;
import javax.swing.*;
import javax.swing.event.*;
import java.util.List;
import java.util.HashMap;
import java.util.Collection;
import java.util.Observable;
import java.util.Observer;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * This component shows the map image and allows the user to modify it.
 */
public class MapView extends JComponent implements Observer {

    /**
     * The parent panel.
     */
    private MapEditorPanel mapEditor;

    /**
     * The current map.
     */
    private Map map;

    /**
     * The tileset of the current map.
     * Store here to track whenever it is changed.
     */
    private Tileset tileset;

    /**
     * Constants to identify the state of the map view.
     */
    private enum State {

        NORMAL,            // the user is not performing any special operation, he can select or unselect entities
        SELECTING_AREA,    // the user is drawing a rectangle to select several entities
        MOVING_ENTITIES,   // drag and drop
        RESIZING_ENTITIES, // the user is resizing the selected entities
        ADDING_ENTITIES,   // entities are being added on the map and displayed under the cursor
    }

    // information about the current state

    /**
     * State of the map view.
     */
    private State state;

    /**
     * Location of the point defined by the mouse.
     * - in state NORMAL: unused
     * - in state SELECTING_AREA: coordinates of the second point of the rectangle, defined by the cursor
     * - in state MOVING_ENTITIES: coordinates of the pointer
     */
    private Point cursorLocation;

    /**
     * Location of the fixed area of the rectangle the user is drawing
     * in state SELECTING_AREA.
     */
    private Rectangle fixedLocation;

    private int total_dx;                          // in state MOVING_ENTITIES: total x and y translation
    private int total_dy;
    private List<MapEntity> initialSelection;      // the entities selected, saved here before drawing a selection rectangle
    private EntityType entityTypeBeingAdded;       // In state ADDING_ENTITIES: type of the only entity that is about to be added
                                                   // (only for a new entity, not used when pasting).
    private List<MapEntity> entitiesBeingAdded;    // In state ADDING_ENTITIES: entities about to be added.
    private MapEntity masterEntity;                // In states ADDING_ENTITIES and RESIZING_ENTITIES:
                                                   // the master entity. This one follows the the cursor position, and an equivalent
                                                   // change is computed for the other ones.
    private HashMap<MapEntity, Rectangle>
        positionsBeforeResizing;                   // in state RESIZING_ENTITIES: the initial bounding box of entities being resized.
    private static List<MapEntity> copiedEntities; // entities cut or copied, ready to be pasted (or null)

    boolean blockTilesetUpdates = false;           // Temporarily ignore tileset changes in this view.

    // headers of the map view

    /**
     * The toolbar to add entities on the map.
     */
    private AddEntitiesToolbar addEntitiesToolbar;

    /**
     * The key listener of the map view.
     */
    private KeyListener keyListener;

    // other map view stuff

    /**
     * Width (or height) of the area displayed around the map.
     */
    private static final int AREA_AROUND_MAP = 48;

    /**
     * Constructor.
     */
    public MapView(MapEditorPanel mapEditor) {
        super();

        this.mapEditor = mapEditor;

        this.cursorLocation = new Point();
        this.fixedLocation = new Rectangle();
        this.positionsBeforeResizing = new HashMap<MapEntity, Rectangle>();
        this.initialSelection = new ArrayList<MapEntity>();

        MouseInputListener mouseListener = new MapMouseInputListener();
        addMouseListener(mouseListener);
        addMouseMotionListener(mouseListener);

        MouseMiddleButtonScrollListener.install(this);

        keyListener = new MapKeyListener();
        addKeyListener(keyListener);
    }

    /**
     * Returns the map.
     * @return the map
     */
    public Map getMap() {
        return map;
    }

    /**
     * Sets the observed map.
     * @param map the current map, or null if no map is loaded
     */
    public void setMap(Map map) {

        if (this.map != null) {
            this.map.getEntitySelection().deleteObserver(this);
            this.map.deleteObserver(this);
            getViewSettings().deleteObserver(this);
        }

        this.map = map;
        setState(State.NORMAL);

        if (map != null) {
            map.addObserver(this);
            map.getEntitySelection().addObserver(this);
            this.tileset = map.getTileset();
            if (this.tileset != null) {
                this.tileset.addObserver(this);
            }
            getViewSettings().addObserver(this);
            update(getViewSettings(), null);
        }

        update(map, null);
    }

    /**
     * Specifies the toolbar to add entities on the map.
     * @param toolbar the toolbar
     */
    public void setAddEntitiesToolbar(AddEntitiesToolbar toolbar) {
        this.addEntitiesToolbar = toolbar;
    }

    /**
     * Returns the map view settings.
     * @return The map view settings.
     */
    public MapViewSettings getViewSettings() {
        return map == null ? null : map.getViewSettings();
    }

    /**
     * Returns the zoom of the map view.
     * @return the zoom
     */
    public Zoom getZoom() {
        return getViewSettings().getZoom();
    }

    /**
     * Returns the number of pixels displayed around the map.
     * @return the number of pixels displayed around the map
     */
    public int getScaledSpaceAroundMap() {
        return (int) (AREA_AROUND_MAP * getZoom().getValue());
    }

    /**
     * Returns the size of the map.
     * @return the size of the map, or a default size
     * if no map is loaded.
     */
    @Override
    public Dimension getPreferredSize() {
        int width, height;

        if (map == null) {
            width = 0;
            height = 0;
        } else {
            width = map.getWidth();
            height = map.getHeight();
        }

        double level = getZoom().getValue();
        width = (int) ((width + 2 * AREA_AROUND_MAP) * level);
        height = (int) ((height + 2 * AREA_AROUND_MAP) * level);

        return new Dimension(width, height);
    }

    /**
     * Returns whether or not the component is focusable.
     * @return true (this allows the keyboard events)
     */
    public boolean isFocusable() {
        return true;
    }

    /**
     * This function is called when the map, the selected entities, the tileset
     * or the view settings changes.
     * @param o The object changed.
     * @param parameter Parameter about what has changed, or null.
     */
    public void update(Observable o, Object parameter) {

        if (map == null) { // the map has just been closed
            repaint();
            return;
        }

        if (o instanceof Map) {
            // the map has been modified

            if (parameter instanceof Tileset) {
                // The tileset is now another one.
                Tileset tileset = map.getTileset();

                if (tileset != this.tileset) {
                    Tileset oldTileset = this.tileset;
                    if (oldTileset != null) {
                        oldTileset.deleteObserver(this);
                    }
                    tileset.addObserver(this);
                    this.tileset = tileset;
                    if (state == State.ADDING_ENTITIES) {
                        // Notify entities that are about to be added
                        // but don't belong to the map yet.
                        for (MapEntity entity: entitiesBeingAdded) {
                            try {
                                entity.notifyTilesetChanged(oldTileset, tileset);
                            }
                            catch (MapException ex) {
                                // Entity invalid with the new tileset.
                            }
                        }
                    }
                }

                update(tileset, null);
            }

            // redraw the image
            repaint();
        } else if (o instanceof MapEntitySelection) {
            // The entity selection has changed.

            MapEntitySelection selection = (MapEntitySelection) o;
            Tileset tileset = map.getTileset();
            blockTilesetUpdates = true;  // To avoid adding a new tile by reentrant update.
            if (selection.getNbEntitiesSelected() > 1) {
                // Several entities are selected: unhighlight any tile pattern
                tileset.unselectTilePattern();
            }
            else if (selection.getNbEntitiesSelected() == 1
                    && selection.getEntity() instanceof Tile) {
                // A single tile is selected: highlight its pattern in the tileset view.
                String tilePatternId = ((Tile) selection.getEntity()).getTilePatternId();
                tileset.setSelectedTilePatternId(tilePatternId);
            }
            blockTilesetUpdates = false;

            // Redraw the map.
            repaint();
        } else if (o instanceof Tileset
                && !blockTilesetUpdates) {
            // The selected tile pattern in the tileset has changed.

            Tileset tileset = map.getTileset();
            TilePattern pattern = tileset.getSelectedTilePattern();
            if (pattern == null) {
                // No tile pattern is selected anymore in the tileset.
                if (state == State.ADDING_ENTITIES &&
                        entitiesBeingAdded.size() == 1 &&
                        entityTypeBeingAdded == EntityType.TILE) {
                    returnToNormalState();
                }
            }
            else {
                // Just picked a tile pattern.
                if (state == State.NORMAL ||
                        (state == State.ADDING_ENTITIES && entityTypeBeingAdded != null)
                   ) {
                    // If we are doing nothing or creating another type of entity,
                    // create the tile.
                    if (!map.getEntitySelection().isEmpty()) {
                        // If a tile pattern was just selected in the tileset whereas there is already
                        // some entities selected in the map, unselected the entities in the map.
                        setState(State.NORMAL);
                        map.getEntitySelection().unselectAll();
                    }

                    switchAddingNewEntity(EntityType.TILE, null);
                }
            }
        }
        else if (o instanceof MapViewSettings) {

            // Map view settings have changed: unselect hidden entities.
            ArrayList<MapEntity> hiddenEntities =
                new ArrayList<MapEntity>();
            for (MapEntity entity: map.getEntitySelection()) {
                if (!map.getViewSettings().isEntityShown(entity)) {
                    hiddenEntities.add(entity);
                }
            }
            map.getEntitySelection().unselect(hiddenEntities);

            if (parameter instanceof MapViewSettings.ChangeInfo) {

                MapViewSettings.ChangeInfo info = (MapViewSettings.ChangeInfo) parameter;
                if (info.setting.equals("zoom")) {
                    // The zoom has changed.
                    setSize(getPreferredSize());  // Make sure the scroller knows our now size.
                    if (getParent() instanceof JViewport) {
                        JViewport viewport = (JViewport) getParent();

                        double oldZoom = ((Zoom) info.oldValue).getValue();
                        double newZoom = ((Zoom) info.newValue).getValue();
                        Rectangle viewRegion = viewport.getViewRect();

                        int oldX = 0;
                        int oldY = 0;
                        if (getMousePosition() != null) {
                            // The mouse is in the map view: center the zoom on
                            // the mouse.
                            oldX = (int) getMousePosition().getX();
                            oldY = (int) getMousePosition().getY();
                        }
                        else {
                            // Otherwise use the center of the viewport as
                            // reference.
                            oldX = viewRegion.x + viewRegion.width / 2;
                            oldY = viewRegion.y + viewRegion.height / 2;
                        }
                        int offsetX = oldX - viewRegion.x;
                        int offsetY = oldY - viewRegion.y;
                        int x = (int) (oldX / oldZoom * newZoom) - offsetX;
                        int y = (int) (oldY / oldZoom * newZoom) - offsetY;

                        viewport.setViewPosition(new Point(x, y));
                    }
                }
            }
        } else if (o == null && parameter instanceof String) {

            String event = (String) parameter;
            if (event.equals(ViewScroller.ZOOM_IN)) {
                getViewSettings().zoomIn();
            } else if (event.equals(ViewScroller.ZOOM_OUT)) {
                getViewSettings().zoomOut();
            }
        }
    }

    /**
     * Returns true if the image exists.
     * @return true if the map image exists
     */
    public boolean isImageLoaded() {
        return map != null && map.getTileset() != null && map.getTileset().getImage() != null;
    }

    /**
     * This function is called to display the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {

        if (map == null) {
            return;
        }

        Tileset tileset = map.getTileset();

        // outside the map
        double zoom = getZoom().getValue();
        int scaledSpaceAroundMap = getScaledSpaceAroundMap();

        g.setColor(Color.lightGray);
        g.fillRect(0, 0, (int) (map.getWidth() * zoom) + 2 * scaledSpaceAroundMap,
                (int) (map.getHeight() * zoom) + 2 * scaledSpaceAroundMap);
        g.translate(scaledSpaceAroundMap, scaledSpaceAroundMap);

        // background color
        if (getViewSettings().getShowLayer(Layer.LOW) && tileset != null) {
            g.setColor(tileset.getBackgroundColor());
        } else {
            g.setColor(Color.black);
        }
        g.fillRect(0, 0, (int) (map.getWidth() * zoom), (int) (map.getHeight() * zoom));

        if (tileset != null && tileset.getImage() != null) {

            int x, y, width, height;

            // the entities
            for (Layer layer : Layer.values()) {

                // nothing to do if this layer is not shown
                if (getViewSettings().getShowLayer(layer)) {

                    MapEntities entities = map.getEntities(layer);

                    for (MapEntity entity: entities) {

                        // should we draw this entity?
                        if (getViewSettings().isEntityShown(entity)) {

                            // draw the entity
                            entity.paint(g, zoom, getViewSettings().getShowTransparency());

                            // draw the selection rectangle if the entity is selected
                            if (map.getEntitySelection().isSelected(entity)) {

                                Rectangle positionInMap = entity.getPositionInMap();

                                x = (int) (positionInMap.x * zoom);
                                y = (int) (positionInMap.y * zoom);
                                width = (int) (positionInMap.width * zoom - 1);
                                height = (int) (positionInMap.height * zoom - 1);

                                g.setColor(Color.GREEN);
                                g.drawRect(x, y, width, height);
                                g.drawRect(x + 1, y + 1, width - 2, height - 2);
                            }
                        }
                    }
                } // for
            } // for

            // draw the grid if needed
            if (getViewSettings().getShowGrid()) {
                g.setColor(Color.GRAY);
                // draw the vertical lines
                for (int i = 0; i < (map.getWidth() / getViewSettings().getGridSize()); i++) {
                    g.fillRect((int)(i * getViewSettings().getGridSize() * zoom), 0 ,1, (int)(map.getHeight() * zoom));
                }
                // draw the horizontal lines
                for (int i = 0; i < (map.getHeight() / getViewSettings().getGridSize() ); i++) {
                    g.fillRect(0, (int)(i * getViewSettings().getGridSize() * zoom), (int)(map.getWidth() * zoom), 1);
                }
            }

            // special display for some states
            switch (state) {

                case ADDING_ENTITIES:

                    if (getMousePosition() != null) {
                        // The mouse is over the view.
                        // Display the entities on the map, under the pointer.
                        for (MapEntity entity: entitiesBeingAdded) {
                            entity.paint(g, zoom, getViewSettings().getShowTransparency());
                        }
                    }
                    break;

                case SELECTING_AREA:
                    // draw the selection rectangle

                    x = (int) (Math.min(fixedLocation.x, cursorLocation.x) * zoom);
                    y = (int) (Math.min(fixedLocation.y, cursorLocation.y) * zoom);
                    width = (int) (Math.abs(cursorLocation.x - fixedLocation.x) * zoom - 1);
                    height = (int) (Math.abs(cursorLocation.y - fixedLocation.y) * zoom - 1);

                    g.setColor(Color.YELLOW);
                    g.drawRect(x, y, width, height);
                    g.drawRect(x + 1, y + 1, width - 2, height - 2);

                    break;

                default:
                    break;
            }
        }
    }

    /**
     * Removes the selected entities from the map.
     */
    public void destroySelectedEntities() {
        try {
            map.getEntitySelection().removeFromMap();
            map.getEntitySelection().unselectAll();
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot remove the entities: " + ex.getMessage());
        }
    }

    /**
     * Copies the selected entities to the clipboard and removes them from the map.
     */
    public void cutSelectedEntities() {
        copySelectedEntities();
        destroySelectedEntities();
    }

    /**
     * Copies the selected entities to the clipboard.
     */
    public void copySelectedEntities() {
        Collection<MapEntity> selectedEntities = map.getEntitySelection().getEntities();
        copiedEntities = new ArrayList<MapEntity>();
        selectedEntities = map.getSortedEntities(selectedEntities);

        try {
            for (MapEntity entity: selectedEntities) {
                MapEntity copy = MapEntity.createCopy(entity);
                copiedEntities.add(copy);
            }
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog(ex.getMessage());
            ex.printStackTrace();
            copiedEntities = null;
        }

        // Update copy/paste menu items.
        mapEditor.getMainWindow().update(null, null);
    }

    /**
     * Paste the copied entities.
     */
    public void paste() {

        if (!canPaste()) {
            return;
        }

        List<MapEntity> successfullyCopiedEntities = new ArrayList<MapEntity>();

        for (MapEntity original: copiedEntities) {
            try {
                // The entities may come from another map.
                MapEntity copy = MapEntity.createCopy(original);
                copy.setMap(map);
                successfullyCopiedEntities.add(copy);
            }
            catch (QuestEditorException ex) {
                // This entity cannot exist in the new map, typically a tile
                // that does not exist in the tileset of the new map.
                // Don't paste it in this case.
            }
        }

        // Unselect previously selected entities so better show we are pasting.
        map.getEntitySelection().unselectAll();

        // Place the entities under the pointer.
        startAddingEntities(successfullyCopiedEntities);
    }

    /**
     * Returns whether there is something in the clipboard.
     */
    public boolean canPaste() {
        return copiedEntities != null;
    }

    /**
     * Shows the context menu at the coordinates specified by a mouse event.
     * @param mouseEvent a mouse event
     */
    private void showPopupMenu(MouseEvent mouseEvent) {
        MapViewPopupMenu popupMenu = new MapViewPopupMenu(this);
        popupMenu.display(mouseEvent.getX(), mouseEvent.getY());
    }

    /**
     * Changes the current state of the map view.
     * If the state was State.RESIZING_ENTITIES or State.MOVING_ENTITIES,
     * the endResizingEntities() or endMovingEntities() function is called.
     * If you don't want these functions to be called, just change the state variable by hand.
     * @param state the new state
     */
    private void setState(State state) {

        if (this.state == State.RESIZING_ENTITIES) {
            endResizingEntities();
        } else if (this.state == State.MOVING_ENTITIES) {
            endMovingEntities();
        }

        this.state = state;
        addEntitiesToolbar.repaint();
    }

    /**
     * Returns to the normal state.
     * This function is called when the user exits another state.
     */
    private void returnToNormalState() {
        setState(State.NORMAL);
        repaint();
    }

    /**
     * Starts or stops adding an entity.
     * If the user was already adding a new entity of the same kind,
     * we get back to the normal state.
     * Otherwise, we move to State.ADDING_ENTITIES.
     * @param entityType Type of entity to create.
     * @param entitySubtype Subtype of entity to create.
     */
    public void switchAddingNewEntity(EntityType entityType, EntitySubtype entitySubtype) {

        if (state == State.ADDING_ENTITIES
                && entityType == entityTypeBeingAdded) {
            entityTypeBeingAdded = null;
            returnToNormalState();
        }
        else {
            try {
                entityTypeBeingAdded = entityType;
                MapEntity entity = MapEntity.create(map, entityType, entitySubtype);

                if (entity == null) {
                    // Cannot happen: MapEntity.create() returns an entity
                    // or throws a MapException.
                    throw new NullPointerException();
                }

                if (entityType == EntityType.TILE) {
                    String patternId = map.getTileset().getSelectedTilePatternId();
                    entity.setStringProperty("pattern", patternId);
                }

                List<MapEntity> entities = new ArrayList<MapEntity>();
                entities.add(entity);
                startAddingEntities(entities);
            } catch (MapException ex) {
                GuiTools.errorDialog("Cannot create the entity: " + ex.getMessage());
            }
        }
    }

    /**
     * Moves to the state State.ADDING_ENTITIES.
     * Allows the user to place on the map some new entities.
     * These entities are displayed under the cursor, keeping their relative position.
     * The user can place them by pressing the mouse at the location he wants.
     * If the user was already adding an entity of the same kind,
     * we get back to the normal state.
     * @param entities The entities to add (the list will be copied).
     * Nothing happens if this list is empty.
     */
    public void startAddingEntities(List<MapEntity> entities) {

        if (entities.isEmpty()) {
            return;
        }

        setState(State.ADDING_ENTITIES);
        entitiesBeingAdded = new ArrayList<MapEntity>(entities);

        // Set as master entity the most centered one.
        // To do this, we need to compute the center of our entities.
        int minX = Integer.MAX_VALUE;
        int minY = Integer.MAX_VALUE;
        int maxX = -Integer.MAX_VALUE;
        int maxY = -Integer.MAX_VALUE;

        for (MapEntity entity: entitiesBeingAdded) {
            Rectangle position = entity.getPositionInMap();
            if (position.x < minX) {
                minX = position.x;
            }
            if (position.x + position.width > maxX) {
                maxX = position.x + position.width;
            }
            if (position.y < minY) {
                minY = position.y;
            }
            if (position.y + position.height > maxY) {
                maxY = position.y + position.height;
            }
        }
        int centerX = (minX + maxX) / 2;
        int centerY = (minY + maxY) / 2;

        // We have the center. Find the closest one to the center.
        masterEntity = null;
        int minDistance2 = Integer.MAX_VALUE;
        for (MapEntity entity: entitiesBeingAdded) {
            int dx = (entity.getXTopLeft() + entity.getWidth() / 2) - centerX;
            int dy = (entity.getYTopLeft() + entity.getHeight() / 2) - centerY;
            int distance2 = dx * dx + dy * dy;
            if (distance2 < minDistance2) {
                minDistance2 = distance2;
                masterEntity = entity;
            }
        }

        if (masterEntity == null) {
            // Cannot happen.
            new NullPointerException().printStackTrace();
            return;
        }

        updateAddingEntities();
    }

    /**
     * In state State.ADDING_ENTITIES, updates the position of the entities
     * to add with the new mouse coordinates.
     */
    private void updateAddingEntities() {

        if (masterEntity == null) {
            new NullPointerException().printStackTrace();
            entityTypeBeingAdded = null;
            entitiesBeingAdded = null;
            returnToNormalState();
            return;
        }

        // Get the coordinates of the mouse relative to the map.
        Point mousePosition = getMousePosition();
        if (mousePosition == null) {
            // The mouse is not in this component.
            return;
        }
        int x = getMouseInMapX(mousePosition.x);
        int y = getMouseInMapY(mousePosition.y);

        // The master entity will be centered around x,y
        // and the other ones will keep their relative position to master.

        int width = masterEntity.getWidth();
        int height = masterEntity.getHeight();
        x = GuiTools.round8(x - width / 2);  // Center the master entity around the cursor.
        y = GuiTools.round8(y - height / 2);

        int dx = x - masterEntity.getXTopLeft();
        int dy = y - masterEntity.getYTopLeft();
        if (dx != 0 || dy != 0) {
            // There is a change.
            try {
                for (MapEntity entity: entitiesBeingAdded) {
                    entity.setPositionTopLeft(
                            entity.getXTopLeft() + dx,
                            entity.getYTopLeft() + dy
                    );
                }
            } catch (MapException ex) {
                GuiTools.errorDialog("Unexpected error: " + ex.getMessage());
            }
            repaint();
        }
    }

    /**
     * In state State.ADDING_ENTITIES, adds the current entities to the map.
     * If there was only one entity added and if it resizable,
     * the resulting state is State.RESIZING_ENTITY.
     * Otherwise it is State.NORMAL.
     * @returns The entities added or null if there was a problem.
     */
    private List<MapEntity> endAddingEntities() {

        List<MapEntity> entitiesAdded = null;
        EditEntityDialog dialog = null;
        try {

            boolean valid = true;
            if (entitiesBeingAdded.size() == 1 && entityTypeBeingAdded != null) {
                // New entity just created.
                MapEntity entityBeingAdded = entitiesBeingAdded.get(0);

                // First choose the layer.
                Layer layerUnderPointer = map.getLayerInRectangle(entityBeingAdded.getPositionInMap());
                if (!entityBeingAdded.hasInitialLayer()
                    || layerUnderPointer.getId() > entityBeingAdded.getLayer().getId()) {
                    // The entity has no preferred layer, or there is something above its preferred layer.
                    map.setEntityLayer(entityBeingAdded, layerUnderPointer);
                }

                // make sure the entity is valid
                valid = entityBeingAdded.isValid();

                if (!valid) {
                    // The entity is not valid yet, this is because some information is missing.
                    // Show a dialog box to let the user edit the entity.

                    dialog = new EditEntityDialog(map, entityBeingAdded);
                    dialog.setLocationRelativeTo(MapView.this);
                    if (dialog.display()) { // if the user filled the dialog box
                        valid = true;
                    } else { // the user cancelled the dialog box
                        state = State.NORMAL;
                    }
                }
            }

            if (valid) {
                // Add the entities to the map.
                map.getHistory().doAction(new ActionAddEntities(map, entitiesBeingAdded));

                // make then selected
                MapEntitySelection selection = map.getEntitySelection();
                selection.unselectAll();
                selection.select(entitiesBeingAdded);

                // Unselect the tile in the tileset (if any).
                if (map.getTileset().getSelectedTilePattern() != null) {
                    map.getTileset().unselectTilePattern();
                }

                // If there is only one entity, let the user resize it
                // until the mouse is released
                // (unless the dialog box was shown).
                if (selection.isResizable()
                        && selection.getNbEntitiesSelected() == 1
                        && dialog == null) {
                    startResizingEntities();
                } else {
                    entityTypeBeingAdded = null;
                    state = State.NORMAL;
                }

                entitiesAdded = entitiesBeingAdded;
                entitiesBeingAdded = null;
            }
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot add entities: " + ex.getMessage());
        }

        repaint();
        addEntitiesToolbar.repaint();

        return entitiesAdded;
    }

    /**
     * Returns the type of the entity that is being created if any.
     * If the state is not State.ADDING_ENTITIES, or if the entities being added
     * are not a new one (i.e. were pasted), null is returned.
     * @return The type of the entity being created, or null if the user is
     * not creating an entity.
     */
    public EntityType getEntityTypeBeingAdded() {

        if (state != State.ADDING_ENTITIES) {
            return null;
        }

        return entityTypeBeingAdded;
    }

    /**
     * Moves to the state State.SELECTING_AREA.
     * Lets the user draw a rectangle to select some entities.
     * @param x x coordinate of where the selection starts
     * @param y y coordinate of where the selection starts
     */
    private void startSelectingArea(int x, int y) {

        // initial point of the rectangle
        fixedLocation.x = GuiTools.round8(x);
        fixedLocation.y = GuiTools.round8(y);

        // second point of the rectangle
        cursorLocation.x = fixedLocation.x;
        cursorLocation.y = fixedLocation.y;

        // save the entities already selected (useful for a multiple selection)
        Collection<MapEntity> currentSelection = map.getEntitySelection().getEntities();
        initialSelection.clear();
        for (MapEntity entity : currentSelection) {
            initialSelection.add(entity);
        }

        setState(State.SELECTING_AREA);
        repaint();
    }

    /**
     * In state State.SELECING_AREA, updates the selection area
     * with the new mouse coordinates.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateSelectingArea(int x, int y) {

        x = GuiTools.round8(x);
        y = GuiTools.round8(y);

        x = Math.min(Math.max(-AREA_AROUND_MAP, x), map.getWidth() + 2 * AREA_AROUND_MAP);
        y = Math.min(Math.max(-AREA_AROUND_MAP, y), map.getHeight() + 2 * AREA_AROUND_MAP);

        // update the second point of the rectangle if necessary
        if (x != cursorLocation.x || y != cursorLocation.y) {
            cursorLocation.x = x;
            cursorLocation.y = y;

            // update the selection
            MapEntitySelection entitySelection = map.getEntitySelection();
            entitySelection.unselectAll();

            // get the rectangle's content
            List<MapEntity> entitiesInRectangle =
                    map.getEntitiesInRectangle(fixedLocation.x, fixedLocation.y,
                    cursorLocation.x, cursorLocation.y);

            // select the entities in the rectangle
            entitySelection.select(entitiesInRectangle);

            // also restore the initial selection (for a multiple selection)
            for (MapEntity entity : initialSelection) {
                entitySelection.select(entity);
            }
        }
    }

    /**
     * Moves to the state State.RESIZING_ENTITIES.
     * Lets the user resize the entities selected on the map.
     * All selected entities must be resizable, otherwise nothing is done.
     */
    public void startResizingEntities() {

        MapEntitySelection entitySelection = map.getEntitySelection();

        if (entitySelection.isEmpty() || !entitySelection.isResizable()) {
            return;
        }

        // Store the initial position of all entities to resize
        // and chose the leader.
        // The leader will be the closest entity to the mouse.
        // Arbitrary choice of the leader
        // in case the mouse is outside the view.
        Point mousePosition = getMousePosition();

        masterEntity = entitySelection.getEntity();
        int minDistanceToMouse2 = Integer.MAX_VALUE;
        for (MapEntity entity: entitySelection) {
            Rectangle positionInMap = entity.getPositionInMap();
            positionsBeforeResizing.put(entity, new Rectangle(positionInMap));

            if (mousePosition != null) {
                int mouseX = getMouseInMapX(mousePosition.x);
                int mouseY = getMouseInMapY(mousePosition.y);
                int dx = (positionInMap.x + positionInMap.width) - mouseX;
                int dy = (positionInMap.y + positionInMap.height) - mouseY;
                int distanceToMouse2 = dx * dx + dy * dy;
                if (distanceToMouse2 < minDistanceToMouse2) {
                    masterEntity = entity;
                    minDistanceToMouse2 = distanceToMouse2;
                }
            }
        }

        state = State.RESIZING_ENTITIES;
        repaint();
    }

    /**
     * In state State.RESIZING_ENTITIES, updates with the mouse coordinates
     * the rectangle of the entities that are being resized.
     * @param x X coordinate of the pointer relative to the map.
     * @param y Y coordinate of the pointer relative to the map.
     */
    private void updateResizingEntities(int x, int y) {

        // Only resize if the cursor is inside the drawn area.
        if (x < map.getWidth() + AREA_AROUND_MAP
                && y < map.getHeight() + 2 * AREA_AROUND_MAP) {

            Rectangle oldMasterPosition = positionsBeforeResizing.get(masterEntity);

            for (MapEntity entity: map.getEntitySelection()) {
                Rectangle oldEntityPosition = positionsBeforeResizing.get(entity);
                int masterOffsetX = oldEntityPosition.x + oldEntityPosition.width
                        - (oldMasterPosition.x + oldMasterPosition.width);
                int masterOffsetY = oldEntityPosition.y + oldEntityPosition.height
                        - (oldMasterPosition.y + oldMasterPosition.height);
                updateResizingEntity(entity, x + masterOffsetX, y + masterOffsetY);
            }
        }
    }

    /**
     * In state State.RESIZING_ENTITIES, updates with new coordinates
     * the rectangle of one entity.
     * @param entity The entity to resize.
     * @param x X coordinate of the second point for this entity.
     * @param y X coordinate of the second point for this entity.
     */
    private void updateResizingEntity(MapEntity entity, int x, int y) {

        int xA, yA; // A is the original point of the rectangle we are drawing
        int xB, yB; // B is the second point, defined by the cursor location

        xB = x;
        yB = y;

        int width = entity.getUnitarySize().width;
        int height = entity.getUnitarySize().height;

        xA = positionsBeforeResizing.get(entity).x;
        yA = positionsBeforeResizing.get(entity).y;
        // we have to extend the entity's rectangle with units of size (width,height) from point A to point B

        // trust me: this awful formula calculates the coordinates of point B such
        // that the size of the rectangle from A to B is a multiple of (width,height)
        int diffX = xB - xA;
        int diffY = yB - yA;
        int signX = (diffX >= 0) ? 1 : -1;
        int signY = (diffY >= 0) ? 1 : -1;
        xB = xB + signX * (width - ((Math.abs(diffX) + width) % width));
        yB = yB + signY * (height - ((Math.abs(diffY) + height) % height));

        // if the entity is constrained to be square, set the position of point B accordingly
        if (entity.mustBeSquare()) {
            diffX = Math.abs(xB - xA);
            diffY = Math.abs(yB - yA);
            int length = Math.max(diffX, diffY); // length of the square
            xB = xA + signX * length;
            yB = yA + signY * length;
        }
        // point A or B may have to be updated so that the rectangle is extended
        // only in allowed directions, making sure its size is never zero
        else {
            if (entity.isExtensible(0)) {
                if (xB <= xA) {
                    xA += width;
                }
            } else {
                xB = xA + width;
            }

            if (entity.isExtensible(1)) {
                if (yB <= yA) {
                    yA += height;
                }
            } else {
                yB = yA + height;
            }
        }

        // now let's update the entity
        try {
            // note that A is not necessarily the top-left corner of the rectangle
            map.setEntityPosition(entity, xA, yA, xB, yB);
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot resize the entity: " + ex.getMessage());
        }
    }

    /**
     * In state State.RESIZING_ENTITY, stops the resizing and saves it into the undo/redo history.
     */
    private void endResizingEntities() {

        try {
            boolean changed = false;
            HashMap<MapEntity, Rectangle> finalPositions = new HashMap<MapEntity, Rectangle>();
            for (MapEntity entity: map.getEntitySelection()) {

                // Get a copy of the final rectangle before we restore the initial one
                Rectangle initialPosition = positionsBeforeResizing.get(entity);
                Rectangle finalPosition = new Rectangle(entity.getPositionInMap());

                if (!finalPosition.equals(initialPosition)) {  // If the entity's rectangle has changed.

                    // While dragging the mouse, the entity's rectangle has followed the mouse, being
                    // resized with many small steps and with individual entities.
                    // Now we want to consider the whole resizing process
                    // as one step only, so that it can be undone or redone directly later.
                    // So first, restore entities with their initial shape.
                    finalPositions.put(entity, finalPosition);
                    map.setEntityPositionUnchecked(entity, initialPosition);  // The initial position could be invalid.
                    changed = true;
                }
            }

            if (changed) {
                // Make the resizing in one step, this time saving it into the undo/redo history.
                map.getHistory().doAction(new ActionResizeEntities(map, finalPositions));
            }
        } catch (QuestEditorException e) {
            GuiTools.errorDialog("Cannot resize entities: " + e.getMessage());
        }
        positionsBeforeResizing.clear();
        masterEntity = null;
        state = State.NORMAL;
        repaint();
    }

    /**
     * Moves to the state State.MOVING_ENTITIES.
     * Lets the user move the entities selected on the map.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    public void startMovingEntities(int x, int y) {

        x = GuiTools.round8(x);
        y = GuiTools.round8(y);

        cursorLocation.x = x;
        cursorLocation.y = y;

        total_dx = 0;
        total_dy = 0;

        setState(State.MOVING_ENTITIES);
    }

    /**
     * In state State.MOVING_ENTITIES, updates with the new mouse coordinates
     * the position of the selected entities.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateMovingEntities(int x, int y) {

        x = GuiTools.round8(x);
        y = GuiTools.round8(y);

        x = Math.min(Math.max(-AREA_AROUND_MAP, x), map.getWidth() + 2 * AREA_AROUND_MAP);
        y = Math.min(Math.max(-AREA_AROUND_MAP, y), map.getHeight() + 2 * AREA_AROUND_MAP);

        if (x != cursorLocation.x || y != cursorLocation.y) {

            int dx = x - cursorLocation.x;
            int dy = y - cursorLocation.y;

            // we move the entities during the dragging, to make them follow the mouse while dragging
            Collection<MapEntity> entities = map.getEntitySelection().getEntities();

            try {
                map.moveEntities(entities, dx, dy);
            } catch (MapException ex) {
                System.err.println("Unexpected error: " + ex.getMessage());
                ex.printStackTrace();
                System.exit(1);
            }

            // but we also save the total move because only the total move will be undoable
            this.total_dx += dx;
            this.total_dy += dy;

            cursorLocation.x = x;
            cursorLocation.y = y;
        }
    }

    /**
     * In state State.MOVING_ENTITIES, stops the move and saves it into the undo/redo history.
     */
    private void endMovingEntities() {

        if (total_dx != 0 || total_dy != 0) {

            /**
             * While dragging the mouse, the selected entities have followed the mouse, moving with
             * small 8-pixel steps. Now we want to consider the whole move as one step only, so that
             * it can be undone or redone directly later.
             */
            Collection<MapEntity> entities = map.getEntitySelection().getEntities();

            // we restore the entities at their initial position
            try {
                map.moveEntities(entities, -total_dx, -total_dy);
            } catch (MapException ex) {
                System.err.println("Unexpected error: " + ex.getMessage());
                ex.printStackTrace();
                System.exit(1);
            }

            // we make the whole move in one step, this time saving it into the undo/redo history
            try {
                map.getHistory().doAction(new ActionMoveEntities(map, entities, total_dx, total_dy));
            } catch (QuestEditorException e) {
                GuiTools.errorDialog("Cannot move the entities: " + e.getMessage());
            }
        }
        state = State.NORMAL;
        repaint();
    }

    /**
     * Converts an x value from the view coordinates to the map coordinate system.
     * The map coordinate system differs from the map view coordinate system
     * because of the zoom and the area displayed around the map.
     * @param x The x value relative to the map view.
     * @return The x value relative to the map.
     */
    public int getMouseInMapX(int x) {
        return (int) ((x - getScaledSpaceAroundMap()) / getZoom().getValue());
    }

    /**
     * Converts an y value from the view coordinates to the map coordinate system.
     * The map coordinate system differs from the map view coordinate system
     * because of the zoom and the area displayed around the map.
     * @param y The y value relative to the map view.
     * @return The y value relative to the map.
     */
    public int getMouseInMapY(int y) {
        return (int) ((y - getScaledSpaceAroundMap()) / getZoom().getValue());
    }

    /**
     * Returns the x coordinate of a mouse event in the map coordinate system.
     * The map coordinate system differs from the map view coordinate system
     * because of the zoom and the area displayed around the map.
     * @param mouseEvent the mouse event
     * @return the x coordinate of the mouse event in the map coordinate system
     */
    public int getMouseInMapX(MouseEvent mouseEvent) {
        return getMouseInMapX(mouseEvent.getX());
    }

    /**
     * Returns the y coordinate of a mouse event in the map coordinate system.
     * The map coordinate system differs from the map view coordinate system
     * because of the zoom and the area displayed around the map.
     * @param mouseEvent the mouse event
     * @return the y coordinate of the mouse event in the map coordinate system
     */
    public int getMouseInMapY(MouseEvent mouseEvent) {
        return getMouseInMapY(mouseEvent.getY());
    }

    /**
     * Returns the visible entity located under the mouse.
     * @param mouseEvent the mouse event
     * @return the entity clicked
     */
    public MapEntity getEntityClicked(MouseEvent mouseEvent) {

        MapEntity entityClicked = null;

        int x = getMouseInMapX(mouseEvent);
        int y = getMouseInMapY(mouseEvent);

        for (int id = Layer.values().length - 1;
                id >= 0 && entityClicked == null;
                id--) {

            Layer layer = Layer.get(id);
            entityClicked = map.getEntityAt(layer, x, y);
        }

        return entityClicked;
    }

    /**
     * The mouse listener associated to the map image.
     */
    private class MapMouseInputListener extends MouseInputAdapter {

        /**
         * This method is called when the mouse exits the map view.
         */
        public void mouseExited(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            if (state == State.ADDING_ENTITIES) {
                repaint(); // useful when adding an entity
            }
        }

        /**
         * This method is called when the mouse is clicked on the map view.
         */
        public void mouseClicked(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            requestFocusInWindow();

            if (state == State.NORMAL && mouseEvent.getButton() == MouseEvent.BUTTON1) {

                // find the entity clicked
                MapEntity entityClicked = getEntityClicked(mouseEvent);

                // detect whether CTRL or SHIFT is pressed
                if (!mouseEvent.isControlDown() && !mouseEvent.isShiftDown()
                    && mouseEvent.getClickCount() == 2
                    && entityClicked != null) {
                    // double-click on an entity: show the edit dialog

                    EditEntityDialog dialog = new EditEntityDialog(map, entityClicked);
                    dialog.setLocationRelativeTo(MapView.this);
                    dialog.display();
                }
            }
        }

        /**
         * This method is called when the mouse is pressed on the map view.
         */
        public void mousePressed(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            requestFocusInWindow();
            MapEntitySelection entitySelection = map.getEntitySelection();

            // detect the mouse button
            int button = mouseEvent.getButton();
            int x = getMouseInMapX(mouseEvent);
            int y = getMouseInMapY(mouseEvent);

            if (button != MouseEvent.BUTTON2) {

                switch (state) {

                // select or unselect an entity
                case NORMAL:

                    // find the entity clicked
                    MapEntity entityClicked = getEntityClicked(mouseEvent);

                    boolean alreadySelected = entitySelection.isSelected(entityClicked);

                    // left click
                    if (button == MouseEvent.BUTTON1) {

                        // unselect all entities unless CTRL or SHIFT is pressed
                        if (!mouseEvent.isControlDown() && !mouseEvent.isShiftDown()
                                && (entityClicked == null || !alreadySelected)) {

                            entitySelection.unselectAll();
                        }

                        // The user may want to select multiple entities.
                        if (entityClicked == null ||
                                mouseEvent.isControlDown() ||
                                mouseEvent.isShiftDown()
                                ) {
                            startSelectingArea(x, y);
                        } else {
                            // Make the entity clicked selected.
                            entitySelection.select(entityClicked);

                            // The user may want to move it.
                            startMovingEntities(x, y);
                        }
                    } // right click
                    else if (button == MouseEvent.BUTTON3) {

                        // If an entity is selected and the user right clicks on another tile,
                        // we will select the new one instead of the old one.
                        // Note that if several entities are selected, the selection is kept.
                        if (entitySelection.getNbEntitiesSelected() == 1 && entityClicked != null
                                && !entitySelection.isSelected(entityClicked)) {

                            map.getEntitySelection().unselectAll();
                        }

                        // select the entity clicked if no previous selection was kept
                        if (entitySelection.isEmpty() && entityClicked != null) {
                            entitySelection.select(entityClicked);
                        }

                        // show a popup menu for the entities selected
                        showPopupMenu(mouseEvent);
                    }

                    break;

                    // validate the new size
                case RESIZING_ENTITIES:

                    endResizingEntities();
                    break;

                    // place the new entity
                case ADDING_ENTITIES:

                    List<MapEntity> entitiesAdded = endAddingEntities();  // Add the entities to the map.

                    // Copy the entities just added for the next paste.
                    if (state == State.NORMAL
                            && entitiesAdded != null) {

                        if (entitiesAdded.size() == 1
                                && entitiesAdded.get(0) instanceof Tile) {
                            String patternId = ((Tile) entitiesAdded.get(0)).getTilePatternId();
                            map.getTileset().setSelectedTilePatternId(patternId);
                        }

                        copiedEntities = new ArrayList<MapEntity>();
                        try {
                            for (MapEntity entity: entitiesAdded) {
                                MapEntity copy = MapEntity.createCopy(entity);
                                copiedEntities.add(copy);
                            }
                        } catch (QuestEditorException ex) {
                            GuiTools.errorDialog(ex.getMessage());
                            ex.printStackTrace();
                        }

                        // If the entities were added with a right click and are not being
                        // resized, we propose to add another copy of these entities now.
                        if (button == MouseEvent.BUTTON3) {
                            startAddingEntities(copiedEntities);
                        }
                    }

                    break;

                default:
                    break;
                }
            }
        }

        /**
         * This function is called when the mouse is released on the component.
         * If an entity was being resized (by dragging the mouse), the new size is validated.
         */
        public void mouseReleased(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            int button = mouseEvent.getButton();

            MapEntitySelection entitySelection = map.getEntitySelection();

            switch (state) {

                case NORMAL:

                    if (button == MouseEvent.BUTTON1 &&
                        mouseEvent.isControlDown() || mouseEvent.isShiftDown()) {

                        // Find the entity clicked.
                        MapEntity entityClicked = getEntityClicked(mouseEvent);

                        if (entityClicked != null) {
                            // Toggle the selected state of this entity.
                            if (entitySelection.isSelected(entityClicked)) {
                                entitySelection.unselect(entityClicked);
                            }
                            else {
                                entitySelection.select(entityClicked);
                            }
                        }
                    }
                    break;

                case RESIZING_ENTITIES:

                    if (button == MouseEvent.BUTTON1 ||
                            button == MouseEvent.BUTTON3) {
                        // Resizing while the mouse was pressed means we were just adding entities.
                        // FIXME: this is not entirely true, we should remember the information explicitly.
                        Collection<MapEntity> entitiesResized = entitySelection.getEntities();
                        entitiesResized = map.getSortedEntities(entitiesResized);

                        endResizingEntities();
                        // At this point no entity is selected anymore.

                        if (button != MouseEvent.BUTTON3) {
                            entityTypeBeingAdded = null;
                        }
                        else {
                            // Add copies of these entities again if the right button was released.

                            if (entitiesResized.size() == 1
                                    && entityTypeBeingAdded == EntityType.TILE) {
                                // If it is a single tile just created, update the tileset view.
                                Tile tile = (Tile) entitySelection.getEntity();
                                String patternId = tile.getTilePatternId();
                                map.getTileset().setSelectedTilePatternId(patternId);
                            }

                            // Move to the state State.ADDING_ENTITIES.
                            try {
                                List<MapEntity> copiedEntities = new ArrayList<MapEntity>();
                                for (MapEntity entity: entitiesResized) {
                                    MapEntity copy = MapEntity.createCopy(entity);
                                    copiedEntities.add(copy);
                                }

                                // If this was a new entity creation, restore the unitary size for next addings.
                                if (copiedEntities.size() == 1 && entityTypeBeingAdded != null) {
                                    MapEntity entity = copiedEntities.get(0);
                                    entity.setSize(entity.getUnitarySize());
                                }

                                startAddingEntities(copiedEntities);
                            } catch (QuestEditorException ex) {
                                GuiTools.errorDialog(ex.getMessage());
                                ex.printStackTrace();
                                returnToNormalState();
                            }
                        }
                    }
                    break;

                case SELECTING_AREA:
                    returnToNormalState();

                    if (button == MouseEvent.BUTTON3) {
                        showPopupMenu(mouseEvent);
                    }
                    else if (button == MouseEvent.BUTTON1 &&
                            mouseEvent.isControlDown() || mouseEvent.isShiftDown()) {

                        // If the rectangle is empty, this was actually a single selection,
                        // that ends when releasing the mouse because ctrl or shift was pressed.
                        // Usually, a single selection ends when pressing the mouse, but when
                        // ctrl and shift are pressed, a selection rectangle is started instead.
                        if (fixedLocation.x == cursorLocation.x &&
                                fixedLocation.y == cursorLocation.y) {

                            // Find the entity clicked.
                            MapEntity entityClicked = getEntityClicked(mouseEvent);

                            if (entityClicked != null) {
                                // Toggle the selected state of this entity.
                                if (entitySelection.isSelected(entityClicked)) {
                                    entitySelection.unselect(entityClicked);
                                }
                                else {
                                    entitySelection.select(entityClicked);
                                }
                            }
                        }
                    }
                    break;

                case MOVING_ENTITIES:
                    endMovingEntities();
                    break;

                default:
                    break;
            }
        }

        /**
         * This method is called when the cursor is moved onto the map view.
         */
        public void mouseMoved(MouseEvent mouseEvent) {

            if (isImageLoaded()) {

                int x = getMouseInMapX(mouseEvent);
                int y = getMouseInMapY(mouseEvent);

                switch (state) {

                    case ADDING_ENTITIES:
                        // update the entities position
                        updateAddingEntities();
                        break;

                    case RESIZING_ENTITIES:
                        // if we are resizing an entity, calculate the coordinates of
                        // the second point of the rectangle formed by the pointer
                        updateResizingEntities(x, y);
                        break;

                    default:
                        break;
                }
            }
        }

        /**
         * This function is called when the mouse is dragged on the component.
         * In State.NORMAL, when no entity was clicked, a selection rectangle appears.
         * In State.SELECTING_AREA, the selection rectangle is updated.
         * In State.RESIZING_ENTITY, the rectangle of the tile is updated.
         */
        public void mouseDragged(MouseEvent mouseEvent) {

            if (!isImageLoaded()) {
                return;
            }

            boolean leftDrag = (mouseEvent.getModifiersEx() & InputEvent.BUTTON1_DOWN_MASK) != 0;
            boolean rightDrag = (mouseEvent.getModifiersEx() & InputEvent.BUTTON3_DOWN_MASK) != 0;

            if (leftDrag || rightDrag) {
                // Left or right button.
                int x = getMouseInMapX(mouseEvent);
                int y = getMouseInMapY(mouseEvent);

                switch (state) {

                case SELECTING_AREA:

                    // update the selection rectangle
                    if (leftDrag) {
                        updateSelectingArea(x, y);
                    }
                    break;

                case RESIZING_ENTITIES:
                    // if we are resizing entities, calculate the coordinates of
                    // the second point of the rectangle formed by the pointer
                    updateResizingEntities(x, y);
                    break;

                case MOVING_ENTITIES:
                    // if we are moving entities, update their position

                    if (leftDrag) {
                        updateMovingEntities(x, y);
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    /**
     * The key listener associated to the map image.
     */
    private class MapKeyListener extends KeyAdapter {

        /**
         * This method is invoked when a key is pressed on the map image.
         */
        public void keyPressed(KeyEvent keyEvent) {

            if (state != State.NORMAL) {
                return;
            }

            MapEntitySelection selectedEntities = map.getEntitySelection();

            int key = keyEvent.getKeyCode();
            switch (key) {

                case KeyEvent.VK_DELETE:
                    destroySelectedEntities();
                    break;

                case KeyEvent.VK_ENTER:
                    if (selectedEntities.getNbEntitiesSelected() == 1) {
                        MapEntity entitySelected = selectedEntities.getEntity();
                        EditEntityDialog dialog = new EditEntityDialog(map, entitySelected);
                        dialog.setLocationRelativeTo(MapView.this);
                        dialog.display();
                    }
                    break;

                case KeyEvent.VK_R:
                    startResizingEntities();
                    break;

                case KeyEvent.VK_T:
                    try {
                        Collection<MapEntity> entities = map.getEntitySelection().getEntities();
                        map.getHistory().doAction(new ActionBringToFront(map, entities));
                    } catch (QuestEditorException e) {
                        GuiTools.errorDialog("Cannot bring the entities to front: " + e.getMessage());
                    }
                    break;

                case KeyEvent.VK_B:
                    try {
                        Collection<MapEntity> entities = map.getEntitySelection().getEntities();
                        map.getHistory().doAction(new ActionBringToBack(map, entities));
                    } catch (QuestEditorException e) {
                        GuiTools.errorDialog("Cannot bring the entities to front: " + e.getMessage());
                    }
                    break;

                case KeyEvent.VK_PLUS:
                case KeyEvent.VK_ADD:

                    try {
                        Collection<MapEntity> entities = map.getEntitySelection().getEntities();
                        map.getHistory().doAction(new ActionUpDownLayer(map, entities, true));
                    } catch (QuestEditorException e) {
                        GuiTools.errorDialog("Cannot change the layer: " + e.getMessage());
                    }
                    break;

                case KeyEvent.VK_MINUS:
                case KeyEvent.VK_SUBTRACT:

                    try {
                        Collection<MapEntity> entities = map.getEntitySelection().getEntities();
                        map.getHistory().doAction(new ActionUpDownLayer(map, entities, false));
                    } catch (QuestEditorException e) {
                        GuiTools.errorDialog("Cannot change the layer: " + e.getMessage());
                    }
                    break;
            }
        }
    }
}
