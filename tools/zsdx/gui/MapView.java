package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;
import java.util.List;

import zsdx.*;
import zsdx.Map;
import zsdx.map_editor_actions.*;

/**
 * This component shows the map image and allows the user to modify it.
 */
public class MapView extends JComponent implements Observer, Scrollable {

    /**
     * The current map.
     */
    private Map map;

    // static constants to identify the state

    /**
     * The user is not performing any special operation.
     * He can select or unselect entities.
     */
    private static final int STATE_NORMAL = 0; 

    /**
     * The user is drawing a rectangle to select several entities.
     */
    private static final int STATE_SELECTING_AREA = 1;

    /**
     * The user is moving the selected entities (drag and drop).
     */
    private static final int STATE_MOVING_ENTITIES = 2;

    /**
     * The user is resizing the selected entity.
     */
    private static final int STATE_RESIZING_ENTITY = 3;

    /**
     * An entity is being added on the map.
     * This entity is displayed under the cursor and the user
     * can place the new entity by clicking on the map. 
     */
    private static final int STATE_ADDING_ENTITY = 4;

    // information about the current state

    /**
     * State of the map view.
     */
    private int state;

    /**
     * Location of the point defined by the mouse.
     * - in state STATE_NORMAL: unused
     * - in state STATE_SELECTING_AREA: coordinates of the second point of the rectangle, defined by the cursor
     * - in state STATE_MOVING_ENTITIES: coordinates of the pointer
     * - in state STATE_RESIZING_ENTITY: coordinates of the second point of the rectangle, defined by the cursor
     * - in state STATE_ADDING_ENTITY: coordinates of the entity displayed under the cursor
     */
    private Point cursorLocation;

    /**
     * Location of the fixed area of the rectangle the user is drawing.
     * - in state STATE_SELECTING_AREA: coordinates of the initial point (width and height are not used)
     * - in state STATE_RESIZING_ENTITY: top-left rectangle of the entity before being resized
     */
    private Rectangle fixedLocation;

    /**
     * In state STATE_MOVING_ENTITIES: total x translation
     */
    private int total_dx;

    /**
     * In state STATE_MOVING_ENTITIES: total y translation
     */
    private int total_dy;

    /**
     * The entities selected, saved here before drawing a selection rectangle.
     */
    private List<MapEntity> initialSelection;

    /**
     * True if the last entity on which the mouse was pressed was not already selected.
     */
    private boolean entityJustSelected;

    // other map view stuff

    /**
     * The popup menu shown when the user right clicks on the selected entities.
     */
    private MapViewPopupMenu popupMenu;

    /**
     * The display options (what layers are displayed, etc.).
     */
    private MapViewRenderingOptions renderingOptions;

    /**
     * Zoom of the map view.
     */
    private double zoom;

    /**
     * Constructor.
     */
    public MapView() {
	super();

	this.cursorLocation = new Point();
	this.fixedLocation = new Rectangle();
	this.initialSelection = new LinkedList<MapEntity>();
	this.renderingOptions = new MapViewRenderingOptions(this);
	this.zoom = 2;

	// create the popup menu for the selected entities
	// items: resize, layer, destroy
	popupMenu = new MapViewPopupMenu(this);

	MouseInputListener mouseListener = new MapMouseInputListener();
	addMouseListener(mouseListener);
	addMouseMotionListener(mouseListener);
	addKeyListener(new MapKeyListener());
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
	}

	this.map = map;
	this.state = STATE_NORMAL;

	if (map != null) {
	    map.addObserver(this);
	    map.getEntitySelection().addObserver(this);
	    if (map.getTileset() != null) {
		map.getTileset().addObserver(this);
	    }
	}

	popupMenu.setMap(map);

	update(map, null);
    }

    /**
     * Returns the zoom of the map view.
     * @return the zoom
     */
    public double getZoom() {
	return zoom;
    }

    /**
     * Returns the rendering options of the map view, i.e. how the entities are displayed.
     * @return the rendering options
     */
    public MapViewRenderingOptions getRenderingOptions() {
	return renderingOptions;
    }

    /**
     * Returns the size of the map.
     * @return the size of the map, or a default size
     * if no map is loaded.
     */
    public Dimension getPreferredSize() {
	int width, height;

	if (map == null) {
	    width = 640;
	    height = 480;
	}
	else {
	    width = (int) (map.getWidth() * zoom);
	    height = (int) (map.getHeight() * zoom);
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
     * Returns whether or not the component is focusable.
     * @return true
     */
    public boolean isFocusable() {
	return true;
    }
    
    /**
     * This function is called when the map, the selected entities or the tileset changes.
     * @param o the object changed
     * @param obj parameters
     */
    public void update(Observable o, Object obj) {
	
	if (map == null) { // the map has just been closed
	    repaint();
	    return;
	}

	if (o instanceof Map) {
	    // the map has been modified

	    if (obj instanceof Tileset) {
		// the tileset has been changed
		Tileset tileset = map.getTileset();
		tileset.addObserver(this);
		update(tileset, null);
	    }

	    // redraw the image
	    
	    repaint();
	    setSize(getPreferredSize());
	}

	else if (o instanceof MapEntitySelection) {
	    // the tile selection has changed

	    // redraw the map
	    repaint();
	}

	else if (o instanceof Tileset) {
	    // the selected tile in the tileset has changed
	    
	    Tileset tileset = map.getTileset();
	    if (tileset.getSelectedTile() == null) {
		// no tile is selected anymore in the tileset
		state = STATE_NORMAL;
		repaint();
	    }
	    else if (!map.getEntitySelection().isEmpty()) {

		// if a tile was just selected in the tileset whereas there is already
		// entities selected in the map, unselected the entities in the map
		state = STATE_NORMAL;
		map.getEntitySelection().unSelectAll();
	    }
	}
    }

    /**
     * Returns true if the image exists.
     * @return true if the map image exists
     */
    public boolean isImageLoaded() {
	return map != null && map.getTileset() != null && map.getTileset().getDoubleImage() != null;
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

	// background color
	if (renderingOptions.getShowLayer(MapEntity.LAYER_LOW) && tileset != null) {
	    g.setColor(tileset.getBackgroundColor());
	    g.fillRect(0, 0, (int) (map.getWidth() * zoom), (int) (map.getHeight() * zoom));
	}

	if (tileset != null) {

	    Image tilesetImage = tileset.getDoubleImage();
	    if (tilesetImage != null) {

		int x, y, width, height;

		// the entities
		for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {

		    // nothing to do if this layer is not shown
		    if (renderingOptions.getShowLayer(layer)) {

			MapEntities entities = map.getEntities(layer);
			for (MapEntity entity: entities) {

			    // should we draw this entity?
			    if (renderingOptions.isEntityShown(entity)) {

				// draw the entity
				entity.paint(g, zoom, renderingOptions.getShowTransparency());
				
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

		// special display for some states
		switch (state) {

		case STATE_ADDING_ENTITY:
		    // display on the map, under the cursor, the entity selected in the tileset

		    // if it is a tile (only case for now)
		    Tile selectedTileInTileset = tileset.getSelectedTile();
		    selectedTileInTileset.paint(g, tileset, cursorLocation.x, cursorLocation.y, 2);

		    break;

		case STATE_SELECTING_AREA:
		    // draw the selection rectangle

		    x = (int) (Math.min(fixedLocation.x, cursorLocation.x) * zoom);
		    y = (int) (Math.min(fixedLocation.y, cursorLocation.y) * zoom);
		    width = (int) (Math.abs(cursorLocation.x - fixedLocation.x) * zoom - 1);
		    height = (int) (Math.abs(cursorLocation.y - fixedLocation.y) * zoom - 1);

		    g.setColor(Color.YELLOW);
		    g.drawRect(x, y, width, height);
		    g.drawRect(x + 1, y + 1, width - 2, height - 2);
		    
		    break;
		}
	    }
	}
    }

    /**
     * Removes the selected entities from the map.
     */
    public void destroySelectedEntities() {
	try {
	    map.getEntitySelection().removeFromMap();
	}
	catch (ZSDXException e) {
	    WindowTools.errorDialog("Cannot remove the entities: " + e.getMessage());
	}
    }

    /**
     * Returns to the normal state.
     * This function is called when the user exits another state.
     */
    private void returnToNormalState() {
	state = STATE_NORMAL;
	repaint();
    }

    /**
     * Move to the state STATE_ADDING_ENTITY.
     * Allows the user to place on the map an entity
     * This entity is displayed under the cursor and the user
     * can place it by pressing the mouse
     * at the location he wants.
     */
    private void startAddingEntity() {
	// TODO: parameter entity type?
	state = STATE_ADDING_ENTITY;
    }

    /**
     * In state STATE_ADDING_ENTITY, updates the position of the entity
     * to add with the new mouse coordinates.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateAddingEntity(int x, int y) {

	x -= x % 8;
	y -= y % 8;
	
	if (x != cursorLocation.x || y != cursorLocation.y) {
	    cursorLocation.x = x;
	    cursorLocation.y = y;
	    repaint();
	}
    }

    /**
     * In state STATE_ADDING_ENTITY, adds the current entity to the map.
     * The current entity selected in the tileset is placed on the map at the mouse location.
     */
    private void endAddingEntity() {

	try {
	    // create the entity
	    Tileset tileset = map.getTileset();

	    // if it is a tile (only case for now)
	    MapEntity entity = new TileOnMap(map, tileset.getSelectedTileId(),
					     cursorLocation.x, cursorLocation.y);

	    // add it to the map
	    map.getHistory().doAction(new ActionAddEntity(map, entity));

	    // make it selected
	    map.getEntitySelection().select(entity);
	    tileset.unSelectTile();
	    startResizingEntity(); // let the user resize the entity until the mouse is released
	    repaint();
	}
	catch (ZSDXException ex) {
	    WindowTools.errorDialog("Cannot add the entity: " + ex.getMessage());
	}
    }

    /**
     * Moves to the state STATE_SELECTING_AREA.
     * Lets the user draw a rectangle to select some entities.
     * @param x x coordinate of where the selection starts
     * @param y y coordinate of where the selection starts
     */
    private void startSelectingArea(int x, int y) {

	// initial point of the rectangle
	fixedLocation.x = (x + 4) / 8 * 8;
	fixedLocation.y = (y + 4) / 8 * 8;

	// second point of the rectangle
	cursorLocation.x = fixedLocation.x;
	cursorLocation.y = fixedLocation.y;

	// save the entities already selected (useful for a multiple selection)
	List<MapEntity> currentSelection = map.getEntitySelection().getEntities();
	initialSelection.clear();
	for (MapEntity entity: currentSelection) {
	    initialSelection.add(entity);
	}

	state = STATE_SELECTING_AREA;
	repaint();
    }

    /**
     * In state STATE_SELECING_AREA, updates the selection area
     * with the new mouse coordinates.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateSelectingArea(int x, int y) {

	x = (x + 4) / 8 * 8;
	y = (y + 4) / 8 * 8;

	x = Math.min(Math.max(0, x), map.getWidth());
	y = Math.min(Math.max(0, y), map.getHeight());

	// update the second point of the rectangle if necessary
	if (x != cursorLocation.x || y != cursorLocation.y) {
	    cursorLocation.x = x;
	    cursorLocation.y = y;
	    
	    // update the selection
	    MapEntitySelection entitySelection = map.getEntitySelection();
	    entitySelection.unSelectAll();
	    
	    // get the rectangle's content
	    List<MapEntity> entitiesInRectangle =
		map.getEntitiesInRectangle(fixedLocation.x, fixedLocation.y,
					   cursorLocation.x, cursorLocation.y);

	    // select the entities in the rectangle, except the hidden ones
	    for (MapEntity entity: entitiesInRectangle) {
		if (renderingOptions.isEntityShown(entity)) {
		    entitySelection.select(entity);
		}
	    }

	    // also restore the initial selection (for a multiple selection)
	    for (MapEntity entity: initialSelection) {
		entitySelection.select(entity);
	    }
	}
    }

    /**
     * Moves to the state STATE_RESIZING_ENTITY.
     * Lets the user resize the entity selected on the map.
     * There must be exactly one entity selected, and this entity must be resizable,
     * otherwise nothing is done.
     */
    public void startResizingEntity() {

	MapEntitySelection entitySelection = map.getEntitySelection();

	if (entitySelection.isResizable()) {
	    MapEntity entity = entitySelection.getEntity(0);
	    Rectangle tilePositionInMap = entity.getPositionInMap();
	    
	    fixedLocation.x = tilePositionInMap.x;
	    fixedLocation.y = tilePositionInMap.y;
	    fixedLocation.width = tilePositionInMap.width;
	    fixedLocation.height = tilePositionInMap.height;

	    cursorLocation.x = tilePositionInMap.x + tilePositionInMap.width;
	    cursorLocation.y = tilePositionInMap.y + tilePositionInMap.height;

	    state = STATE_RESIZING_ENTITY;
	    repaint();
	}
    }

    /**
     * In state STATE_RESIZING_ENTITY, updates with the new mouse coordinates
     * the rectangle of the tile that is being resized.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateResizingEntity(int x, int y) {
    
	int xA, yA; // A is the original point of the rectangle we are drawing
	int xB, yB; // B is the second point, defined by the cursor location
	
	xB = x;
	yB = y;
	
	if (xB < map.getWidth() && yB < map.getHeight()) {
	    
	    MapEntity selectedEntity = map.getEntitySelection().getEntity(0);
	    
	    int width = selectedEntity.getUnitWidth();
	    int height = selectedEntity.getUnitHeight();
	    
	    xA = fixedLocation.x;
	    yA = fixedLocation.y;
	    
	    // snap the coordinates to the grid
	    xB += width / 2;
	    yB += height / 2;
	    
	    xB -= xB % 8;
	    yB -= yB % 8;
	    
	    // snap the coordinates to the size of the entity
	    xB = xB - (xB - xA) % width;
	    yB = yB - (yB - yA) % height;
	    
	    if (xB != cursorLocation.x || yB != cursorLocation.y) {
		
		// the rectangle has changed
		cursorLocation.x = xB;
		cursorLocation.y = yB;
		
		// (xA,yA) depends on the rectangle direction:
		// they may have to be updated so that the rectangle size is not zero
		if (xB <= xA) {
		    xA += width;
		}
		
		if (yB <= yA) {
		    yA += height;
		}
		
		// now let's update the entity
		try {
		    map.setEntityPosition(selectedEntity, xA, yA, xB, yB);
		}
		catch (ZSDXException e) {
		    WindowTools.errorDialog("Cannot resize the entity: " + e.getMessage());
		}
	    }
	}
    }

    /**
     * In state STATE_RESIZING_ENTITY, stops the resizing and saves it into the undo/redo history.
     */
    private void endResizingEntity() {

	MapEntity entity = map.getEntitySelection().getEntity(0);

	// get a copy of the final rectangle before we restore the initial one
	Rectangle finalPosition = new Rectangle(entity.getPositionInMap());
	
	if (!finalPosition.equals(fixedLocation)) { // if the tile's rectangle has changed

	    /**
	     * While dragging the mouse, the entity's rectangle has followed the mouse, being
	     * resized with small steps. Now we want to consider the whole resizing process
	     * as one step only, so that it can be undone or redone directly later.
	     */
	    
	    try {
		// we restore the entity at its initial size
		map.setEntityPosition(entity, fixedLocation);
		
		// we make the resizing in one step, this time saving it into the undo/redo history
		map.getHistory().doAction(new ActionResizeEntity(map, entity, finalPosition));
	    }
	    catch (ZSDXException e) {
		WindowTools.errorDialog("Cannot resize the entity: " + e.getMessage());
	    }
	}
	returnToNormalState();
    }

    /**
     * Moves to the state STATE_MOVING_ENTITIES.
     * Lets the user move the entities selected on the map.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    public void startMovingEntities(int x, int y) {

	x -= x % 8;
	y -= y % 8;

	cursorLocation.x = x;
	cursorLocation.y = y;
	
	total_dx = 0;
	total_dy = 0;

	state = STATE_MOVING_ENTITIES;
    }

    /**
     * In state STATE_MOVING_ENTITIES, updates with the new mouse coordinates
     * the position of the selected entities.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateMovingEntities(int x, int y) {

	x -= x % 8;
	y -= y % 8;

	x = Math.min(Math.max(0, x), map.getWidth());
	y = Math.min(Math.max(0, y), map.getHeight());

	if (x != cursorLocation.x || y != cursorLocation.y) {
	    
	    int dx = x - cursorLocation.x;
	    int dy = y - cursorLocation.y;

	    // we move the entities during the dragging, to make them follow the mouse while dragging
	    List<MapEntity> entities = map.getEntitySelection().getEntities();
	    
	    try {
		map.moveEntities(entities, dx, dy);
	    }
	    catch (MapException ex) {
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
     * In state STATE_MOVING_ENTITIES, stops the move and saves it into the undo/redo history.
     */
    private void endMovingEntities() {

	if (total_dx != 0 || total_dy != 0) {

	    /**
	     * While dragging the mouse, the selected entities have followed the mouse, moving with
	     * small 8-pixel steps. Now we want to consider the whole move as one step only, so that
	     * it can be undone or redone directly later.
	     */
	    
	    List<MapEntity> entities = map.getEntitySelection().getEntities();
	    
	    // we restore the entities at their initial position
	    try {
		map.moveEntities(entities, -total_dx, -total_dy);
	    }
	    catch (MapException ex) {
		System.err.println("Unexpected error: " + ex.getMessage());
		ex.printStackTrace();
		System.exit(1);
	    }
	    
	    // we make the whole move in one step, this time saving it into the undo/redo history
	    try {
		map.getHistory().doAction(new ActionMoveEntities(map, entities, total_dx, total_dy));
	    }
	    catch (ZSDXException e) {
		WindowTools.errorDialog("Cannot move the entities: " + e.getMessage());
	    }
	}
	returnToNormalState();
    }

    /**
     * The mouse listener associated to the map image.
     */
    private class MapMouseInputListener extends MouseInputAdapter {

	/**
	 * This method is called when the mouse exits the map view.
	 * If a tile is selected in the tileset, it is still displayed at
	 * the last cursor location on the map view, so we have to remove it.
	 */
	public void mouseExited(MouseEvent mouseEvent) {

	    if (state == STATE_ADDING_ENTITY) {

		returnToNormalState();
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

	    if (state == STATE_NORMAL && mouseEvent.getButton() == MouseEvent.BUTTON1) {

		// find the entity clicked
		int x = (int) (mouseEvent.getX() / zoom);
		int y = (int) (mouseEvent.getY() / zoom);

		MapEntity entityClicked = map.getEntityAt(x, y);

		// detect whether CTRL or SHIFT is pressed
		if (mouseEvent.isControlDown() || mouseEvent.isShiftDown()) {

		    MapEntitySelection entitySelection = map.getEntitySelection();
		
		    if (entityClicked != null && !entityJustSelected
			&& entitySelection.isSelected(entityClicked)) {

			// CTRL + left click or SHIFT + left click:
			// unselect the tile clicked
			entitySelection.unSelect(entityClicked);
		    }
		}
		else if (mouseEvent.getClickCount() == 2) {
		    // double-click on an entity: show the edit dialog

		    if (entityClicked != null) {
			EditEntityDialog dialog = new EditEntityDialog(map, entityClicked);
		    	dialog.setLocationRelativeTo(null);
		    	dialog.pack();
		    	dialog.setVisible(true);
		    }
		}
	    }
	}

	/**
	 * This method is called when the mouse is pressed on the map view.
	 * If the state is STATE_ADDING_ENTITY, an instance of the entity is
	 * added to the map at the cursor location.
	 * Otherwise, the entity clicked becomes selected in the map.
	 * A right click on an entity of the map shows a popup menu.
	 */
	public void mousePressed(MouseEvent mouseEvent) {

	    if (!isImageLoaded()) {
		return;
	    }

	    requestFocusInWindow();
	    MapEntitySelection entitySelection = map.getEntitySelection();

	    switch (state) {

		// select or unselect an entity
	    case STATE_NORMAL:

		// detect the mouse button
		int button = mouseEvent.getButton();

		// find the entity clicked
		int x = (int) (mouseEvent.getX() / zoom);
		int y = (int) (mouseEvent.getY() / zoom);

		MapEntity entityClicked = null;
		for (int layer = MapEntity.LAYER_HIGH;
		     layer >= MapEntity.LAYER_LOW && entityClicked == null;
		     layer--) {
		    
		    if (renderingOptions.getShowLayer(layer)) {
			entityClicked = map.getEntityAt(layer, x, y);
		    }
		}

		boolean alreadySelected = entitySelection.isSelected(entityClicked);
		
		// left click
		if (button == MouseEvent.BUTTON1) {

		    // unselect all entities unless CTRL or SHIFT is pressed
		    if (!mouseEvent.isControlDown() && !mouseEvent.isShiftDown() &&
			(entityClicked == null || !alreadySelected)) {

			entitySelection.unSelectAll();
		    }

		    // the user may want to select entities
		    if (entityClicked == null) {
			startSelectingArea(x, y);
		    }
		    else {
			// make the tile selected
			entitySelection.select(entityClicked);
			entityJustSelected = !alreadySelected;
		    
			// the user may want to move the selected entities
			startMovingEntities(x, y);
		    }
		}
		
		// right click
		else if (button == MouseEvent.BUTTON3) {

		    // If an entity is selected and the user right clicks on another tile,
		    // we will select the new one instead of the old one.
		    // Note that if several entities are selected, the selection is kept.
		    if (entitySelection.getNbEntitiesSelected() == 1 && entityClicked != null
			&& !entitySelection.isSelected(entityClicked)) {

			map.getEntitySelection().unSelectAll();
		    }
		    
		    // select the tile clicked if no previous selection was kept
		    if (entitySelection.isEmpty() && entityClicked != null) {
			entitySelection.select(entityClicked);
		    }
		    
		    // show a popup menu for the entities selected
		    if (!entitySelection.isEmpty()) {
			popupMenu.show(mouseEvent.getX(), mouseEvent.getY());
		    }
		}
		
		break;

		// validate the new size
	    case STATE_RESIZING_ENTITY:

		endResizingEntity();
		break;
		
		// place the new tile
	    case STATE_ADDING_ENTITY:

		endAddingEntity(); // add the entity to the map
		break;
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

	    switch (state) {

	    case STATE_RESIZING_ENTITY:
		MapEntity entity = map.getEntitySelection().getEntity(0);
		endResizingEntity();

		if (mouseEvent.getButton() == MouseEvent.BUTTON3) {
		    
		    int x = (int) (mouseEvent.getX() / zoom);
		    int y = (int) (mouseEvent.getY() / zoom);

		    // move to the state STATE_ADDING_ENTITY

		    // if it is a tile (only case for now):
		    int tileId = ((TileOnMap) entity).getTileId();
		    map.getTileset().setSelectedTileId(tileId);
 		    startAddingEntity();
		    updateAddingEntity(x, y);
		}
		break;

	    case STATE_SELECTING_AREA:
		returnToNormalState();

		if (mouseEvent.getButton() == MouseEvent.BUTTON3) {
		    popupMenu.show(mouseEvent.getX(), mouseEvent.getY());
		}
		break;

	    case STATE_MOVING_ENTITIES:
		endMovingEntities();
		break;
	    }
	}
	
	/**
	 * This method is called when the cursor is moved onto the map view.
	 * If a tile is selected in the tileset, it is displayed under the cursor.
	 */
	public void mouseMoved(MouseEvent mouseEvent) {

	    if (isImageLoaded()) {
		
		int x = (int) (mouseEvent.getX() / zoom);
		int y = (int) (mouseEvent.getY() / zoom);

		switch (state) {
		    
		case STATE_NORMAL:
		    // if a tile is selected in the tileset,
		    // display it on the map under the cursor
		    if (map.getTileset().getSelectedTile() != null) {
			startAddingEntity();
		    }

		    break;
		    
		case STATE_ADDING_ENTITY:
		    // if we are adding an entity but the mouse is outside the map, 
		    // remove the entity displayed under the cursor

		    if (x >= map.getWidth() || y >= map.getHeight()) {

			returnToNormalState();
		    }
		    else {
			// update the entity position
			updateAddingEntity(x, y);
		    }

		    break;

		case STATE_RESIZING_ENTITY:
		    // if we are resizing an entity, calculate the coordinates of
		    // the second point of the rectangle formed by the pointer

		    updateResizingEntity(x, y);
		    break;
		}
	    }
	}

	/**
	 * This function is called when the mouse is dragged on the component.
	 * In STATE_NORMAL, when no entity was clicked, a selection rectangle appears.
	 * In STATE_SELECTING_AREA, the selection rectangle is updated.
	 * In STATE_RESIZING_ENTITY, the rectangle of the tile is updated.
	 */
	public void mouseDragged(MouseEvent mouseEvent) {

	    if (!isImageLoaded()) {
		return;
	    }

	    boolean leftClick = (mouseEvent.getModifiersEx() & InputEvent.BUTTON1_DOWN_MASK) != 0;

	    int x = (int) (mouseEvent.getX() / zoom);
	    int y = (int) (mouseEvent.getY() / zoom);

	    switch (state) {

	    case STATE_SELECTING_AREA:

		// update the selection rectangle
		if (leftClick) {
		    updateSelectingArea(x, y);
		}
		break;

	    case STATE_RESIZING_ENTITY:
		// if we are resizing a tile, calculate the coordinates of
		// the second point of the rectangle formed by the pointer
		updateResizingEntity(x, y);
		break;

	    case STATE_MOVING_ENTITIES:
		// if we are moving entities, update their position
		
		if (leftClick) {
		    updateMovingEntities(x, y);
		}
		break;
	    }
	}
    }
    
    /**
     * The key listener associated to the map image.
     */
    private class MapKeyListener extends KeyAdapter {
	
	/**
	 * This method is invoked when a key is pressed on the map image.
	 * If the user presses Delete, the selected entities are removed from the map.
	 */
	public void keyPressed(KeyEvent keyEvent) {

	    if (state == STATE_NORMAL) {

		switch (keyEvent.getKeyCode()) {
		    
		case KeyEvent.VK_DELETE:
		    destroySelectedEntities();
		    break;
		}
	    }
	}
    }
}
