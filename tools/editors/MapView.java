package editors;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

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

    /**
     * State of the map view.
     */
    private int state;

    /**
     * Location of the point or the rectangle defined by the mouse.
     * - in state STATE_NORMAL: unused
     * - in state STATE_SELECTING_AREA: the rectangle drawn by the mouse
     * - in state STATE_MOVING_TILES: unused
     * - in state STATE_RESIZING_TILE: coordinates of the second point of the rectangle, defined by the cursor
     * - in state STATE_ADDING_TILE: coordinates of the tile displayed under the cursor
     */
    private Rectangle cursorLocation;

    /**
     * Fixed area of the tile that is being resized.
     */
    private Rectangle fixedRectangle;

    /**
     * The popup menu shown when the user right clicks on the selected tiles.
     */
    private MapViewPopupMenu popupMenu;

    /**
     * Constructor.
     */
    public MapView() {
	super();

	this.cursorLocation = new Rectangle();
	this.fixedRectangle = new Rectangle();


	// create the popup menu for the selected tiles
	// items: resize, layer, destroy
	popupMenu = new MapViewPopupMenu(this);

	Configuration.getInstance().addObserver(this);

	addMouseListener(new MapMouseListener());
	addMouseMotionListener(new MapMouseMotionListener());
	addKeyListener(new MapKeyListener());

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
	    width = map.getWidth() * 2;
	    height = map.getHeight() * 2;
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

	// background color
	g.setColor(map.getBackgroundColor());
	g.fillRect(0, 0, map.getWidth() * 2, map.getHeight() * 2);

	Tileset tileset = map.getTileset();
	if (tileset != null) {

	    Image tilesetImage = tileset.getDoubleImage();
	    if (tilesetImage != null) {

		int x, y, width, height;

		// the tiles
		for (int layer = 0; layer < 3; layer++) {
		    TileOnMapVector tiles = map.getTiles(layer);
		    for (int i = 0; i < tiles.size(); i++) {
			
			TileOnMap tile = tiles.get(i);

			// draw the tile
			tile.paint(g, tileset, 2);
			
			// draw the selection rectangle if necessary
			if (map.getTileSelection().isSelected(tile)) {
			    
			    Rectangle positionInMap = tile.getPositionInMap();
			    
			    x = positionInMap.x * 2;
			    y = positionInMap.y * 2;
			    width = positionInMap.width * 2 - 1;
			    height = positionInMap.height * 2 - 1;
			    
			    g.setColor(Color.GREEN);
			    g.drawRect(x, y, width, height);
			    g.drawRect(x + 1, y + 1, width - 2, height - 2);
			}

		    } // for
		} // for

		// special display for each state
		switch (state) {

		    // the selected tile in the tileset is going to be added to the map
		case STATE_ADDING_TILE:

		    Tile selectedTileInTileset = tileset.getSelectedTile();
		    selectedTileInTileset.paint(g, tileset, cursorLocation.x / 2, cursorLocation.y / 2, 2);

		    break;

		    // the user is resizing the selected tile
// 		case STATE_RESIZING_TILE:
		    
// 		    TileOnMap selectedTileInMap = map.getTileSelection().getTile(0);
// 		    Tile originalTile = selectedTileInMap.getTile();
		    
// 		    Rectangle positionInMap = selectedTileInMap.getPositionInMap();

// 		    // A is the initial point corresponding to the top-left corner of the tile
// 		    int xA = positionInMap.x * 2;
// 		    int yA = positionInMap.y * 2;

// 		    // B is the point corresponding to the cursor
// 		    int xB = cursorLocation.x;
// 		    int yB = cursorLocation.y;

// 		    // calculate the rectangle we will draw
// 		    if (xA < xB) {
// 			x = xA;
// 			width = xB - xA - 1;
// 		    }
// 		    else {
// 			x = xB;
// 			width = xA - xB + originalTile.getWidth() - 1;
// 		    }

// 		    if (yA < yB) {
// 			y = yA;
// 			height = yB - yA - 1;
// 		    }
// 		    else {
// 			y = yB;
// 			width = yA - yB + originalTile.getHeight() - 1;
// 		    }

// 		    // draw the tile

// 		    g.setColor(Color.GREEN);
// 		    g.drawRect(x, y, width, height);
// 		    g.drawRect(x + 1, y + 1, width - 2, height - 2);

// 		    break;
		}
	    }
	}
    }

    /**
     * Adds the current tile to the map.
     * The current tile selected in the tileset is placed on the map at the mouse location.
     * @param selectNewTile true to make selected the new tile after its creation
     */
    private void addCurrentTileToMap(boolean selectNewTile) {

	// create the tile
	Tileset tileset = map.getTileset();
	TileOnMap tile = new TileOnMap(tileset.getSelectedTile(), tileset.getSelectedTileIndex(),
				       cursorLocation.x / 2, cursorLocation.y / 2);

	// add it to the map
	map.addTile(tile);

	// make it selected if required
	if (selectNewTile) {
	    map.getTileSelection().select(tile);
	    map.getTileset().setSelectedTileIndex(-1);
	}
    }

    /**
     * Removes the selected tiles from the map.
     */
    public void destroySelectedTiles() {
	map.getTileSelection().removeFromMap();
    }

    /**
     * Lets the user resize the tile selected on the map.
     * There must be exactly one tile selected.
     */
    public void startResizingSelectedTile() {
	
	MapTileSelection tileSelection = map.getTileSelection();

	if (tileSelection.getNbTilesSelected() == 1) {
	    TileOnMap tileOnMap = tileSelection.getTile(0);
	    Rectangle tilePositionInMap = tileOnMap.getPositionInMap();
	    cursorLocation.x = (tilePositionInMap.x + tilePositionInMap.width) * 2;
	    cursorLocation.y = (tilePositionInMap.y + tilePositionInMap.height) * 2;
	    
	    fixedRectangle.x = tilePositionInMap.x;
	    fixedRectangle.y = tilePositionInMap.y;
	    fixedRectangle.width = tileOnMap.getTile().getWidth();
	    fixedRectangle.height = tileOnMap.getTile().getHeight();

	    state = STATE_RESIZING_TILE;
	    repaint();
	}
    }

    /**
     * Validates the new size of the tile that was being resized.
     */
    private void endResizingSelectedTile() {

	state = STATE_NORMAL;
	repaint();
    }

    /**
     * The mouse listener associated to the map image.
     */
    private class MapMouseListener extends MouseAdapter {

	/**
	 * This method is called when the mouse exits the map view.
	 * If a tile is selected in the tileset, it is still displayed at
	 * the last cursor location on the map view, to we have to remove it.
	 */
	public void mouseExited(MouseEvent mouseEvent) {
	    if (state == STATE_ADDING_TILE) {
		state = STATE_NORMAL;
		repaint();
	    }
	}

	/**
	 * This method is called when the mouse is clicked on the map view.
	 * The focus is given to the component and nothing else is done here.
	 */
	public void mouseClicked(MouseEvent mouseEvent) {

	    if (!isImageLoaded()) {
		return;
	    }

	    requestFocusInWindow();
	}

	/**
	 * This method is called when the mouse is pressed on the map view.
	 * If a tile is selected in the tileset, an instance of this tile is added to the map
	 * at the cursor location and this tile can be resized
	 * until the mouse button is released.
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

		// detect whether CTRL or SHIFT is pressed
		boolean enableMultipleSelection = ((mouseEvent.getModifiers() & (InputEvent.CTRL_MASK | InputEvent.SHIFT_MASK)) != 0);
		    
		// find the tile clicked
		int x = mouseEvent.getX();
		int y = mouseEvent.getY();
		
		TileOnMap tileClicked = map.getTileAt(x / 2, y / 2);
		
		// left click
		if (button == MouseEvent.BUTTON1) {

		    // unselect all tiles unless CTRL or SHIFT is pressed
		    if (!enableMultipleSelection && (tileClicked == null || !tileSelection.isSelected(tileClicked))) {
			map.getTileSelection().unSelectAll();
		    }

		    if (tileClicked != null) {
			if (!enableMultipleSelection) {
			    // make the tile selected
			    tileSelection.select(tileClicked);
			}
			else {
			    // CTRL + left click or SHIFT + left click:
			    // switch the selection state of the tile
			    tileSelection.switchSelection(tileClicked);
			}
		    }
		}
		
		// right click
		else if (button == MouseEvent.BUTTON3) {

		    // If a tile is selected and the user right clicks on another tile,
		    // we will select the new one instead of the old one.
		    // Note that if several tiles are selected, the selection is kept.
		    if (tileSelection.getNbTilesSelected() == 1 && tileClicked != null && !tileSelection.isSelected(tileClicked)) {
			map.getTileSelection().unSelectAll();
		    }

		    // select the tile clicked if no previous selection was kept
		    if (tileSelection.isEmpty() && tileClicked != null) {
			tileSelection.select(tileClicked);
		    }
		    
		    // show a popup menu for the tiles selected
		    if (!tileSelection.isEmpty()) {
			popupMenu.show(x, y);
		    }
		}
		
		break;

		// validate the new size
	    case STATE_RESIZING_TILE:

		endResizingSelectedTile();

		break;
		
		// place the new tile
	    case STATE_ADDING_TILE:

		boolean selectNewTile = (mouseEvent.getButton() != MouseEvent.BUTTON3);
		addCurrentTileToMap(selectNewTile);
	    }
	}

	/**
	 * This function is called when the mouse is released on the component.
	 * If a tile was being resized (by dragging the mouse), the new size is validated.
	 */
	public void mouseReleased(MouseEvent mouseEvent) {
	    
	    if (isImageLoaded() && state == STATE_RESIZING_TILE) {

		endResizingSelectedTile();
	    }
	}

	/**
	 * This function is called when the mouse is dragged on the component.
	 */
	public void mouseDragged(MouseEvent mouseEvent) {

	    if (!isImageLoaded()) {
		return;
	    }

	    switch (state) {

	    case STATE_NORMAL:

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

		int x, y;

		switch (state) {
		    
		case STATE_NORMAL:
		    // if a tile is selected in the tileset, display it on the map under the cursor
		    if (map.getTileset().getSelectedTile() != null) {
			state = STATE_ADDING_TILE;
		    }

		    break;
		    
		case STATE_ADDING_TILE:
		    // if we are adding a tile but the mouse is outside the map, 
		    // remove the tile displayed under the cursor

		    x = mouseEvent.getX();
		    y = mouseEvent.getY();

		    if (x >= map.getWidth() * 2 || y >= map.getHeight() * 2) {
			state = STATE_NORMAL;
			repaint();
		    }
		    else {

			// snap the tile to the 16*16 grid
			x -= x % 16;
			y -= y % 16;
		    
			if (x != cursorLocation.x || y != cursorLocation.y) {
			    cursorLocation.x = x;
			    cursorLocation.y = y;
			    repaint();
			}
		    }

		    break;

		case STATE_RESIZING_TILE:
		    // if we are resizing a tile, calculate the coordinates of
		    // the second point of the rectangle formed by the pointer

		    int xA, yA; // A is the original point of the rectangle we are drawing
		    int xB, yB; // B is the second point, defined by the cursor location

		    xB = mouseEvent.getX() / 2;
		    yB = mouseEvent.getY() / 2;

		    if (xB < map.getWidth() && yB < map.getHeight()) {

			TileOnMap selectedTileOnMap = map.getTileSelection().getTile(0);

			Rectangle selectedTilePosition = selectedTileOnMap.getPositionInMap();
			Tile originalTile = selectedTileOnMap.getTile();
			int width = originalTile.getWidth();
			int height = originalTile.getHeight();

			xA = fixedRectangle.x;
			yA = fixedRectangle.y;
			
			// snap the coordinates to the grid
			xB += width / 2;
			yB += height / 2;

			xB -= xB % 8;
			yB -= yB % 8;
			
			// snap the coordinates to the size of the tile
			xB = xB - (Math.abs(xB - xA)) % width;
			yB = yB - (Math.abs(yB - yA)) % width;

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
				selectedTileOnMap.setPositionInMap(xA, yA, xB, yB);
				repaint();
			    }
			    catch (MapException e) {
				// should not happen as long setPositionInMap() checks only
				// the width and the height of the rectangle
				System.out.println("Unexpected error when trying to resize the tile: " + e.getMessage());
			    }
			}
		    }
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
