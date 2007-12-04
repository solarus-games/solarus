package editors;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import editors.map_editor_actions.*;

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
     * He can select or unselect tiles.
     */
    private static final int STATE_NORMAL = 0; 

    /**
     * The user is drawing a rectangle to select several tiles.
     */
    private static final int STATE_SELECTING_AREA = 1;

    /**
     * The user is moving the selected tiles (drag and drop).
     */
    private static final int STATE_MOVING_TILES = 2;

    /**
     * The user is resizing the selected tile.
     */
    private static final int STATE_RESIZING_TILE = 3;

    /**
     * A tile is selected in the tileset.
     * This tile is displayed under the cursor and the user
     * can place the new tile by clicking on the map. 
     */
    private static final int STATE_ADDING_TILE = 4;

    // information about the current state

    /**
     * State of the map view.
     */
    private int state;

    /**
     * Location of the point defined by the mouse.
     * - in state STATE_NORMAL: unused
     * - in state STATE_SELECTING_AREA: coordinates of the second point of the rectangle, defined by the cursor
     * - in state STATE_MOVING_TILES: coordinates of the pointer
     * - in state STATE_RESIZING_TILE: coordinates of the second point of the rectangle, defined by the cursor
     * - in state STATE_ADDING_TILE: coordinates of the tile displayed under the cursor
     */
    private Point cursorLocation;

    /**
     * Location of the fixed area of the rectangle the user is drawing.
     * - in state STATE_SELECTING_AREA: coordinates of the initial point (width and height are not used)
     * - in state STATE_RESIZING_TILE: top-left rectangle of the tile before being resized
     */
    private Rectangle fixedLocation;

    /**
     * In state STATE_MOVING_TILES: total x translation
     */
    private int total_dx;

    /**
     * In state STATE_MOVING_TILES: total y translation
     */
    private int total_dy;

    /**
     * The tiles selected, saved here before drawing a selection rectangle.
     */
    private TileOnMapList initialSelection;

    /**
     * True if the last tile on which the mouse was pressed was not already selected.
     */
    private boolean tileJustSelected;

    // other map view stuff

    /**
     * The popup menu shown when the user right clicks on the selected tiles.
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
	this.initialSelection = new TileOnMapList();
	this.renderingOptions = new MapViewRenderingOptions(this);
	this.zoom = 2;

	// create the popup menu for the selected tiles
	// items: resize, layer, destroy
	popupMenu = new MapViewPopupMenu(this);

	Configuration.getInstance().addObserver(this);

	addMouseListener(new MapMouseListener());
	addMouseMotionListener(new MapMouseMotionListener());
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
     */
    public void setMap(Map map) {
	if (this.map != null) {
	    this.map.getTileSelection().deleteObserver(this);
	    this.map.deleteObserver(this);
	}

	this.map = map;
	this.state = STATE_NORMAL;

	map.addObserver(this);
	map.getTileSelection().addObserver(this);
	if (map.getTileset() != null) {
	    map.getTileset().addObserver(this);
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
     * Returns the rendering options of the map view, i.e. how the tiles are displayed.
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
     * This function is called when the map, the selected tiles, the tileset or the configuration changes.
     * @param o the object changed
     * @param obj parameters
     */
    public void update(Observable o, Object obj) {

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

	else if (o instanceof MapTileSelection) {
	    // the tile selection has changed

	    // redraw the map
	    repaint();
	}

	else if (o instanceof Configuration && map != null) {
	    // TODO?
	}

	else if (o instanceof Tileset) {
	    // the selected tile in the tileset has changed
	    
	    Tileset tileset = map.getTileset();
	    if (tileset.getSelectedTile() == null) {
		// no tile is selected anymore in the tileset
		state = STATE_NORMAL;
		repaint();
	    }
	    else if (!map.getTileSelection().isEmpty()) {

		// if a tile was just selected in the tileset whereas there is already
		// a tile selected in the map, unselected the tile in the map
		state = STATE_NORMAL;
		map.getTileSelection().unSelectAll();
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
	if (renderingOptions.getShowLayer(Tile.LAYER_LOW) && tileset != null) {
	    g.setColor(tileset.getBackgroundColor());
	    g.fillRect(0, 0, (int) (map.getWidth() * zoom), (int) (map.getHeight() * zoom));
	}

	if (tileset != null) {

	    Image tilesetImage = tileset.getDoubleImage();
	    if (tilesetImage != null) {

		int x, y, width, height;

		// the tiles
		for (int layer = 0; layer < Tile.LAYER_NB; layer++) {

		    // nothing to do if this layer is not shown
		    if (renderingOptions.getShowLayer(layer)) {

			TileOnMapList tiles = map.getTiles(layer);
			for (TileOnMap tile: tiles) {
			    
			    // should we draw this tile?
			    if (renderingOptions.isTileShown(tileset, tile)) {
			    
				// draw the tile
				tile.paint(g, 2, renderingOptions.getShowTransparency());
				
				// draw the selection rectangle if the tile is selected
				if (map.getTileSelection().isSelected(tile)) {
				    
				    Rectangle positionInMap = tile.getPositionInMap();
				    
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

		case STATE_ADDING_TILE:
		    // display on the map, under the cursor, the tile selected in the tileset

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
     * Removes the selected tiles from the map.
     */
    public void destroySelectedTiles() {
	try {
	    map.getTileSelection().removeFromMap();
	}
	catch (MapException e) {
	    WindowTools.errorDialog("Cannot remove the tiles: " + e.getMessage());
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
     * Move to the state STATE_ADDING_TILE.
     * Allows the user to place on the map the current tile
     *(i.e. the tile selected in the tileset).
     * This tile is displayed under the cursor and the user
     * can place it by pressing the mouse
     * at the location he wants.
     */
    private void startAddingTile() {

	state = STATE_ADDING_TILE;
    }

    /**
     * In state STATE_ADDING_TILE, updates the position of the tile
     * to add with the new mouse coordinates.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateAddingTile(int x, int y) {

	x -= x % 8;
	y -= y % 8;
	
	if (x != cursorLocation.x || y != cursorLocation.y) {
	    cursorLocation.x = x;
	    cursorLocation.y = y;
	    repaint();
	}
    }

    /**
     * In state STATE_ADDING_TILE, adds the current tile to the map.
     * The current tile selected in the tileset is placed on the map at the mouse location.
     * @param selectTileCreated true to make selected the new tile after its creation
     */
    private void endAddingTile() {

	// create the tile
	Tileset tileset = map.getTileset();
	TileOnMap tile = new TileOnMap(tileset, tileset.getSelectedTileId(),
				       cursorLocation.x, cursorLocation.y);

	// add it to the map
	try {
	    map.getHistory().doAction(new ActionAddTile(map, tile));

	    // make it selected
	    map.getTileSelection().select(tile);
	    map.getTileset().unSelectTile();
	    startResizingTile(); // let the user resize the tile until the mouse is released
	    repaint();
	}
	catch (MapException e) {
	    // should never happen (adding a tile doesn't throw any exception)
	    WindowTools.errorDialog("Cannot add the tile: " + e.getMessage());
	}
    }

    /**
     * Moves to the state STATE_SELECTING_AREA.
     * Lets the user draw a rectangle to select some tiles.
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

	// save the tiles already selected (useful for a multiple selection)
	TileOnMapList currentSelection = map.getTileSelection().getTiles();
	initialSelection.clear();
	for (TileOnMap tile: currentSelection) {
	    initialSelection.add(tile);
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
	    MapTileSelection tileSelection = map.getTileSelection();
	    tileSelection.unSelectAll();
	    
	    // get the rectangle's content
	    TileOnMapList tilesInRectangle = map.getTilesInRectangle(fixedLocation.x, fixedLocation.y,
								     cursorLocation.x, cursorLocation.y);

	    // select the tiles in the rectangle, except the hidden ones
	    Tileset tileset = map.getTileset();
	    for (TileOnMap tile: tilesInRectangle) {
		if (renderingOptions.isTileShown(tileset, tile)) {
		    tileSelection.select(tile);
		}
	    }

	    // also restore the initial selection (for a multiple selection)
	    for (TileOnMap tile: initialSelection) {
		tileSelection.select(tile);
	    }
	}
    }

    /**
     * Moves to the state STATE_RESIZING_TILE.
     * Lets the user resize the tile selected on the map.
     * There must be exactly one tile selected, otherwise nothing is done.
     */
    public void startResizingTile() {

	MapTileSelection tileSelection = map.getTileSelection();

	if (tileSelection.getNbTilesSelected() == 1) {
	    TileOnMap tileOnMap = tileSelection.getTile(0);
	    Rectangle tilePositionInMap = tileOnMap.getPositionInMap();
	    Tile originalTile = map.getTileset().getTile(tileOnMap.getTileId());
	    
	    fixedLocation.x = tilePositionInMap.x;
	    fixedLocation.y = tilePositionInMap.y;
	    fixedLocation.width = tilePositionInMap.width;
	    fixedLocation.height = tilePositionInMap.height;

	    cursorLocation.x = tilePositionInMap.x + tilePositionInMap.width;
	    cursorLocation.y = tilePositionInMap.y + tilePositionInMap.height;

	    state = STATE_RESIZING_TILE;
	    repaint();
	}
    }

    /**
     * In state STATE_RESIZING_TILE, updates with the new mouse coordinates
     * the rectangle of the tile that is being resized.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateResizingTile(int x, int y) {
    
	int xA, yA; // A is the original point of the rectangle we are drawing
	int xB, yB; // B is the second point, defined by the cursor location
	
	xB = x;
	yB = y;
	
	if (xB < map.getWidth() && yB < map.getHeight()) {
	    
	    TileOnMap selectedTileOnMap = map.getTileSelection().getTile(0);
	    
	    Rectangle selectedTilePosition = selectedTileOnMap.getPositionInMap();
	    Tile originalTile = map.getTileset().getTile(selectedTileOnMap.getTileId());
	    int width = originalTile.getWidth();
	    int height = originalTile.getHeight();
	    
	    xA = fixedLocation.x;
	    yA = fixedLocation.y;
	    
	    // snap the coordinates to the grid
	    xB += width / 2;
	    yB += height / 2;
	    
	    xB -= xB % 8;
	    yB -= yB % 8;
	    
	    // snap the coordinates to the size of the tile
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
		
		// now let's update the tile
		try {
		    map.setTilePosition(selectedTileOnMap, xA, yA, xB, yB);
		}
		catch (MapException e) {
		    // should not happen as long as setPositionInMap() checks only
		    // the width and the height of the rectangle
		    WindowTools.errorDialog("Cannot resize the tile: " + e.getMessage());
		}
	    }
	}
    }

    /**
     * In state STATE_RESIZING_TILE, stops the resizing and saves it into the undo/redo history.
     */
    private void endResizingTile() {

	TileOnMap tile = map.getTileSelection().getTile(0);

	// get a copy of the final rectangle before we restore the initial one
	Rectangle finalPosition = new Rectangle(tile.getPositionInMap());
	
	if (!finalPosition.equals(fixedLocation)) { // if the tile's rectangle has changed

	    /**
	     * While dragging the mouse, the tile's rectangle has followed the mouse, being
	     * resized with small steps. Now we want to consider the whole resizing process
	     * as one step only, so that it can be undone or redone directly later.
	     */
	    
	    try {
		// we restore the tile at its initial size
		map.setTilePosition(tile, fixedLocation);
		
		// we make the resizing in one step,  this time saving itinto the undo/redo history
		map.getHistory().doAction(new ActionResizeTile(map, tile, finalPosition));
	    }
	    catch (MapException e) {
		WindowTools.errorDialog("Cannot resize the tile: " + e.getMessage());
	    }
	}
	returnToNormalState();
    }

    /**
     * Moves to the state STATE_MOVING_TILES.
     * Lets the user move the tiles selected on the map.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    public void startMovingTiles(int x, int y) {

	x -= x % 8;
	y -= y % 8;

	cursorLocation.x = x;
	cursorLocation.y = y;
	
	total_dx = 0;
	total_dy = 0;

	state = STATE_MOVING_TILES;
    }

    /**
     * In state STATE_MOVING_TILES, updates with the new mouse coordinates
     * the position of the selected tiles.
     * @param x x coordinate of the pointer
     * @param y y coordinate of the pointer
     */
    private void updateMovingTiles(int x, int y) {

	x -= x % 8;
	y -= y % 8;

	x = Math.min(Math.max(0, x), map.getWidth());
	y = Math.min(Math.max(0, y), map.getHeight());

	if (x != cursorLocation.x || y != cursorLocation.y) {
	    
	    int dx = x - cursorLocation.x;
	    int dy = y - cursorLocation.y;

	    // we move the tiles during the dragging, to make them follow the mouse while dragging
	    TileOnMapList tiles = map.getTileSelection().getTiles();
	    map.moveTiles(tiles, dx, dy);

	    // but we also save the total move because only the total move will be undoable
	    this.total_dx += dx;
	    this.total_dy += dy;

	    cursorLocation.x = x;
	    cursorLocation.y = y;
	}
    }

    /**
     * In state STATE_MOVING_TILES, stops the move and saves it into the undo/redo history.
     */
    private void endMovingTiles() {

	if (total_dx != 0 || total_dy != 0) {

	    /**
	     * While dragging the mouse, the selected tiles have followed the mouse, moving with
	     * small 8-pixel steps. Now we want to consider the whole move as one step only, so that
	     * it can be undone or redone directly later.
	     */
	    
	    TileOnMapList tiles = map.getTileSelection().getTiles();
	    
	    // we restore the tiles at their initial position
	    map.moveTiles(tiles, -total_dx, -total_dy);
	    
	    // we make the whole move in one step, this time saving it into the undo/redo history
	    try {
		map.getHistory().doAction(new ActionMoveTiles(map, tiles, total_dx, total_dy));
	    }
	    catch (MapException e) {
		WindowTools.errorDialog("Cannot move the tiles: " + e.getMessage());
	    }
	}
	returnToNormalState();
    }

    /**
     * The mouse listener associated to the map image.
     */
    private class MapMouseListener extends MouseAdapter {

	/**
	 * This method is called when the mouse exits the map view.
	 * If a tile is selected in the tileset, it is still displayed at
	 * the last cursor location on the map view, so we have to remove it.
	 */
	public void mouseExited(MouseEvent mouseEvent) {

	    if (state == STATE_ADDING_TILE) {

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
		
		// detect whether CTRL or SHIFT is pressed
		if (mouseEvent.isControlDown() || mouseEvent.isShiftDown()) {

		    MapTileSelection tileSelection = map.getTileSelection();
		
		    // find the tile clicked
		    int x = (int) (mouseEvent.getX() / zoom);
		    int y = (int) (mouseEvent.getY() / zoom);

		    TileOnMap tileClicked = map.getTileAt(x, y);

		    if (tileClicked != null && !tileJustSelected
			&& tileSelection.isSelected(tileClicked)) {

			// CTRL + left click or SHIFT + left click:
			// unselect the tile clicked
			tileSelection.unSelect(tileClicked);
		    }
		}
	    }
	}

	/**
	 * This method is called when the mouse is pressed on the map view.
	 * If a tile is selected in the tileset (i.e. the state is STATE_ADDING_TILE),
	 * an instance of this tile is added to the map
	 * at the cursor location.
	 * Otherwise, the tile clicked becomes selected in the map.
	 * A right click on a tile of the map shows a popup menu.
	 */
	public void mousePressed(MouseEvent mouseEvent) {

	    if (!isImageLoaded()) {
		return;
	    }

	    requestFocusInWindow();
	    MapTileSelection tileSelection = map.getTileSelection();

	    switch (state) {

		// select or unselect a tile
	    case STATE_NORMAL:

		// detect the mouse button
		int button = mouseEvent.getButton();

		// find the tile clicked
		int x = (int) (mouseEvent.getX() / zoom);
		int y = (int) (mouseEvent.getY() / zoom);

		TileOnMap tileClicked = null;
		for (int layer = Tile.LAYER_HIGH; layer >= Tile.LAYER_LOW && tileClicked == null; layer--) {
		    
		    if (renderingOptions.getShowLayer(layer)) {
			tileClicked = map.getTileAt(layer, x, y);
		    }
		}

		boolean alreadySelected = tileSelection.isSelected(tileClicked);
		
		// left click
		if (button == MouseEvent.BUTTON1) {

		    // unselect all tiles unless CTRL or SHIFT is pressed
		    if (!mouseEvent.isControlDown() && !mouseEvent.isShiftDown() &&
			(tileClicked == null || !alreadySelected)) {

			tileSelection.unSelectAll();
		    }

		    // the user may want to select tiles
		    if (tileClicked == null) {
			startSelectingArea(x, y);
		    }
		    else {
			// make the tile selected
			tileSelection.select(tileClicked);
			tileJustSelected = !alreadySelected;
		    
			// the user may want to move the selected tiles
			startMovingTiles(x, y);
		    }
		}
		
		// right click
		else if (button == MouseEvent.BUTTON3) {

		    // If a tile is selected and the user right clicks on another tile,
		    // we will select the new one instead of the old one.
		    // Note that if several tiles are selected, the selection is kept.
		    if (tileSelection.getNbTilesSelected() == 1 && tileClicked != null
			&& !tileSelection.isSelected(tileClicked)) {

			map.getTileSelection().unSelectAll();
		    }
		    
		    // select the tile clicked if no previous selection was kept
		    if (tileSelection.isEmpty() && tileClicked != null) {
			tileSelection.select(tileClicked);
		    }
		    
		    // show a popup menu for the tiles selected
		    if (!tileSelection.isEmpty()) {
			popupMenu.show(mouseEvent.getX(), mouseEvent.getY());
		    }
		}
		
		break;

		// validate the new size
	    case STATE_RESIZING_TILE:

		endResizingTile();
		break;
		
		// place the new tile
	    case STATE_ADDING_TILE:

		endAddingTile(); // add the tile to the map
		break;
	    }
	}

	/**
	 * This function is called when the mouse is released on the component.
	 * If a tile was being resized (by dragging the mouse), the new size is validated.
	 */
	public void mouseReleased(MouseEvent mouseEvent) {
	    
	    if (!isImageLoaded()) {
		return;
	    }

	    switch (state) {

	    case STATE_RESIZING_TILE:
		endResizingTile();

		if (mouseEvent.getButton() == MouseEvent.BUTTON3) {
		    
		    int x = (int) (mouseEvent.getX() / zoom);
		    int y = (int) (mouseEvent.getY() / zoom);

		    int tileId = map.getTileSelection().getTile(0).getTileId();
		    map.getTileset().setSelectedTileId(tileId);
 		    startAddingTile();
		    updateAddingTile(x, y);
		}
		break;

	    case STATE_SELECTING_AREA:
		returnToNormalState();

		if (mouseEvent.getButton() == MouseEvent.BUTTON3) {
		    popupMenu.show(mouseEvent.getX(), mouseEvent.getY());
		}
		break;

	    case STATE_MOVING_TILES:
		endMovingTiles();
		break;
	    }
	}
    }

    /**
     * The mouse motion listener associated to the map image.
     */
    private class MapMouseMotionListener extends MouseMotionAdapter {

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
			startAddingTile();
		    }

		    break;
		    
		case STATE_ADDING_TILE:
		    // if we are adding a tile but the mouse is outside the map, 
		    // remove the tile displayed under the cursor

		    if (x >= map.getWidth() || y >= map.getHeight()) {

			returnToNormalState();
		    }
		    else {

			// update the tile position
			updateAddingTile(x, y);
		    }

		    break;

		case STATE_RESIZING_TILE:
		    // if we are resizing a tile, calculate the coordinates of
		    // the second point of the rectangle formed by the pointer

		    updateResizingTile(x, y);
		    break;
		}
	    }
	}

	/**
	 * This function is called when the mouse is dragged on the component.
	 * In STATE_NORMAL, when to tile was clicked, a selection rectangle appears.
	 * In STATE_SELECTING_AREA, the selection rectangle is updated.
	 * In STATE_RESIZING_TILE, the rectangle of the tile is updated.
	 */
	public void mouseDragged(MouseEvent mouseEvent) {

	    if (!isImageLoaded()) {
		return;
	    }

	    boolean leftClick = (mouseEvent.getModifiersEx() & InputEvent.BUTTON1_DOWN_MASK) != 0;

	    MapTileSelection tileSelection = map.getTileSelection();

	    int x = (int) (mouseEvent.getX() / zoom);
	    int y = (int) (mouseEvent.getY() / zoom);

	    switch (state) {

	    case STATE_SELECTING_AREA:

		// update the selection rectangle
		if (leftClick) {
		    updateSelectingArea(x, y);
		}
		break;

	    case STATE_RESIZING_TILE:
		// if we are resizing a tile, calculate the coordinates of
		// the second point of the rectangle formed by the pointer
		updateResizingTile(x, y);
		break;

	    case STATE_MOVING_TILES:
		// if we are moving tiles, update their position
		
		if (leftClick) {
		    updateMovingTiles(x, y);
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
	 * If the user presses Delete, the selected tiles are removed from the map.
	 */
	public void keyPressed(KeyEvent keyEvent) {

	    if (state == STATE_NORMAL) {

		switch (keyEvent.getKeyCode()) {
		    
		case KeyEvent.VK_DELETE:
		    destroySelectedTiles();
		    break;
		}
	    }
	}
    }
}
