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
    private static final int STATE_NORMAL = 0;
    private static final int STATE_RESIZING_TILE = 1;
    private static final int STATE_ADDING_TILE = 2;

    // state of the map view

    /**
     * State of the map view: STATE_NORMAL, STATE_ADDING_TILE or STATE_RESIZING_TILE.
     */
    private int state;

    /**
     * Coordinates:
     * - of the tile displayed under the pointer in state STATE_ADDING_TILE
     * - of the second point of the tile's rectangle in state STATE_RESIZING_TILE
     */
    private Point cursorLocation;

    // popup menu

    /**
     * A popup menu displayed when the user right-clicks on a tile.
     */
    private JPopupMenu popupMenuSelectedTiles;

    /**
     * Item in the popup menu to resize the selected tile.
     */
    private JMenuItem itemResize;

    /**
     * Items for the layers in the popup menu.
     */
    private JRadioButtonMenuItem[] itemsLayers;

    /**
     * Name of the layers, for the items in the popup menu.
     */
    private static final String[] layerNames = {"Low layer", "Intermediate layer", "High layer"};

    /**
     * Constructor.
     */
    public MapView() {
	super();

	this.cursorLocation = new Point(0, 0);

	Configuration.getInstance().addObserver(this);

	addMouseListener(new MapMouseListener());
	addMouseMotionListener(new MapMouseMotionListener());
	addKeyListener(new MapKeyListener());

	// create the popup menu for a selectedTile
	// items: resize, layer, destroy
	popupMenuSelectedTiles = new JPopupMenu();

	itemResize = new JMenuItem("Resize");
	itemResize.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    startResizingSelectedTile();
		}
	    });
	popupMenuSelectedTiles.add(itemResize);

	popupMenuSelectedTiles.addSeparator();

	itemsLayers = new JRadioButtonMenuItem[3];
	ButtonGroup itemsLayersGroup = new ButtonGroup();
	    
	for (int i = 0; i < 3; i++) {
	    itemsLayers[i] = new JRadioButtonMenuItem(layerNames[i]);
	    itemsLayers[i].addActionListener(new ActionChangeLayer(i));
	    popupMenuSelectedTiles.add(itemsLayers[i]);
	    itemsLayersGroup.add(itemsLayers[i]);
	}

	popupMenuSelectedTiles.addSeparator();

	JMenuItem item = new JMenuItem("Destroy");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    destroySelectedTiles();
		}
	    });
	popupMenuSelectedTiles.add(item);
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
		// the tileset has changed
		Tileset tileset = map.getTileset();
		tileset.addObserver(this);
		update(tileset, null);
	    }

	    // redraw the image
	    repaint();
	}

	else if (o instanceof MapTileSelection) {
	    // the tile selection has changed

	    // update the popup menu to enable or not the item "Resize"
	    boolean enable = (map.getTileSelection().getNbTilesSelected() == 1);
	    itemResize.setEnabled(enable);

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
		case STATE_RESIZING_TILE:
		    
		    TileOnMap selectedTileInMap = map.getTileSelection().getTile(0);
		    
		    Rectangle positionInMap = selectedTileInMap.getPositionInMap();

		    // A is the initial point corresponding to the top-left corner of the tile
		    int xA = positionInMap.x * 2;
		    int yA = positionInMap.y * 2;

		    // B is the point corresponding to the cursor
		    int xB = cursorLocation.x;
		    int yB = cursorLocation.y;

		    x = Math.min(xA, xB);
		    width = Math.abs(xB - xA) - 1;

		    y = Math.min(yA, yB);
		    height = Math.abs(yB - yA) - 1;

		    g.setColor(Color.GREEN);
		    g.drawRect(x, y, width, height);
		    g.drawRect(x + 1, y + 1, width - 2, height - 2);

		    break;
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
    private void destroySelectedTiles() {
	map.getTileSelection().removeFromMap();
    }

    /**
     * Lets the user resize the tile selected on the map.
     * There must be exactly one tile selected.
     */
    private void startResizingSelectedTile() {
	
	MapTileSelection tileSelection = map.getTileSelection();

	if (tileSelection.getNbTilesSelected() == 1) {
	    Rectangle tilePositionInMap = tileSelection.getTile(0).getPositionInMap();
	    cursorLocation.x = (tilePositionInMap.x + tilePositionInMap.width) * 2;
	    cursorLocation.y = (tilePositionInMap.y + tilePositionInMap.height) * 2;
	    state = STATE_RESIZING_TILE;
	    repaint();
	}
    }

    /**
     * Validates the new size of the tile that was being resized.
     */
    private void endResizingSelectedTile() {

	TileOnMap tileOnMap = map.getTileSelection().getTile(0);
	Rectangle positionInMap = tileOnMap.getPositionInMap();
	Rectangle positionInTileset = tileOnMap.getTile().getPositionInTileset();

	// first point: the initial point, the top-left corner of the tile
	int x1 = positionInMap.x;
	int y1 = positionInMap.y;
		
	// second point: the point corresponding to the cursor
	int x2 = cursorLocation.x / 2;
	int y2 = cursorLocation.y / 2;
	
	try {
	    tileOnMap.setPositionInMap(x1, y1, x2, y2);
	    state = STATE_NORMAL;
	    repaint();
	}
	catch (MapException e) {
	    // nothing to do, the modification has just been cancelled if it was impossible
	}
    }

    /**
     * Shows the popup menu associated to the selected tiles.
     * @param x x coordinate of where the popup menu has to be shown
     * @param y y coordinate of where the popup menu has to be shown
     */
    private void showPopupMenu(int x, int y) {

	itemsLayers[Tile.LAYER_BELOW].setSelected(false);
	itemsLayers[Tile.LAYER_INTERMEDIATE].setSelected(false);
	itemsLayers[Tile.LAYER_ABOVE].setSelected(false);

	int layer = map.getTileSelection().getLayer();

	System.out.println("layer = " + layer);

	// if all the selected tiles have the same layer, we check its item
	if (layer != -1) {
	    itemsLayers[layer].setSelected(true);
	}

	popupMenuSelectedTiles.show(this, x, y);
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

		switch (mouseEvent.getButton()) {

		case MouseEvent.BUTTON3:
		    // right click: show a popup menu for the tiles selected

		    if (!tileSelection.isEmpty()) {
			
			showPopupMenu(mouseEvent.getX(), mouseEvent.getY());
		    }
		    
		    break;

		case MouseEvent.BUTTON1:
		    // left click: select or unselect

		    // detect whether CTRL or SHIFT is pressed
		    boolean enableMultipleSelection = ((mouseEvent.getModifiers() & (InputEvent.CTRL_MASK | InputEvent.SHIFT_MASK)) != 0);
		    
		    // unselect all tiles, unless CTRL or SHIFT is pressed
		    if (!enableMultipleSelection) {
			map.getTileSelection().unSelectAll();
		    }
		    
		    // find the tile clicked
		    int x = mouseEvent.getX();
		    int y = mouseEvent.getY();
		    
		    TileOnMap tileClicked = map.getTileAt(x / 2, y / 2);
		    
		    // select or unselect the tile clicked
		    if (tileClicked != null) {
			if (!enableMultipleSelection) {
			    // make the tile selected
			    tileSelection.select(tileClicked);
			}
			else {
			    // switch the selection state of the tile
			    tileSelection.switchSelection(tileClicked);
			}
		    }
		    
		    break;
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

		    // snap the tile to the 16*16 grid
		    x -= x % 16;
		    y -= y % 16;
		    
		    if (x != cursorLocation.x || y != cursorLocation.y) {
			cursorLocation.x = x;
			cursorLocation.y = y;
			repaint();
		    }

		    break;

		case STATE_RESIZING_TILE:
		    // if we are resizing a tile, calculate the coordinates of
		    // the second point of the rectangle formed by the pointer

		    x = mouseEvent.getX() / 2;
		    y = mouseEvent.getY() / 2;

		    if (x < map.getWidth() && y < map.getHeight()) {

			TileOnMap selectedTileOnMap = map.getTileSelection().getTile(0);

			Rectangle selectedTilePosition = selectedTileOnMap.getPositionInMap();
			Tile originalTile = selectedTileOnMap.getTile();
			int width = originalTile.getWidth();
			int height = originalTile.getHeight();
			
			// snap the coordinates to the grid
			x += width / 2;
			y += height / 2;

			x -= x % 8;
			y -= y % 8;
			
			// snap the coordinates to the size of the tile
			x = x - (Math.abs(x - selectedTilePosition.x)) % width;
			y = y - (Math.abs(y - selectedTilePosition.y)) % width;
			
			x *= 2;
			y *= 2;
			
			if (x != cursorLocation.x || y != cursorLocation.y) {
			    cursorLocation.x = x;
			    cursorLocation.y = y;
			    repaint();
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

    /**
     * Action listener invoked when the user changes the layer of a tile
     * from the popup menu after a right click.
     */
    private class ActionChangeLayer implements ActionListener {

	/**
	 * Layer to set when the action is invoked.
	 */
	private int layer;

	/**
	 * Constructor.
	 * @param layer layer to set when the action is invoked.
	 */
	public ActionChangeLayer(int layer) {
	    this.layer = layer;
	}

	/**
	 * Method called when the user sets the layer of the selected tiles.
	 */
	public void actionPerformed(ActionEvent e) {
	    map.getTileSelection().setLayer(layer);
	}
    }
}
