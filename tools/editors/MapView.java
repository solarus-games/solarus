package editors;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

/**
 * This component shows the map image and allows the user to modify it.
 */
public class MapView extends JComponent implements Observer {

    /**
     * The current map.
     */
    private Map map;

    // state of the map view

    // static constants to identify the state
    private static final int STATE_NORMAL = 0;
    private static final int STATE_TILE_SELECTED = 1;
    private static final int STATE_RESIZING_TILE = 2;
    private static final int STATE_ADDING_TILE = 3;

    /**
     * State of the map view: STATE_NORMAL, STATE_TILE_SELECTED,
     * STATE_ADDING_TILE or STATE_RESIZING_TILE.
     */
    private int state;

    /**
     * Coordinates:
     * - of the tile displayed under the pointer in state STATE_ADDING_TILE
     * - of the second point of the tile's rectangle in state STATE_RESIZING_TILE
     */
    private Point cursorLocation;

    /**
     * The current selected tile on the map (in states STATE_TILE_SELECTED and STATE_RESIZING_TILE).
     */
    private TileOnMap currentSelectedTile;

    // popup menu

    /**
     * A popup menu displayed when the user right-clicks on a tile (state STATE_TILE_SELECTED).
     */
    private JPopupMenu popupMenuSelectedTile;

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
	// items: expand, layer, destroy
	popupMenuSelectedTile = new JPopupMenu();

	JMenuItem item = new JMenuItem("Resize");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    state = STATE_RESIZING_TILE;
		}
	    });
	popupMenuSelectedTile.add(item);

	popupMenuSelectedTile.addSeparator();

	itemsLayers = new JRadioButtonMenuItem[3];
	ButtonGroup itemsLayersGroup = new ButtonGroup();
	    
	for (int i = 0; i < 3; i++) {
	    itemsLayers[i] = new JRadioButtonMenuItem(layerNames[i]);
	    itemsLayers[i].addActionListener(new ActionChangeLayer(i));
	    popupMenuSelectedTile.add(itemsLayers[i]);
	    itemsLayersGroup.add(itemsLayers[i]);
	}

	popupMenuSelectedTile.addSeparator();

	item = new JMenuItem("Destroy");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    destroySelectedTile();
		}
	    });
	popupMenuSelectedTile.add(item);
    }

    /**
     * Sets the observed map.
     */
    public void setMap(Map map) {
	if (this.map != null) {
	    this.map.deleteObserver(this);
	}

	this.map = map;
	this.state = STATE_NORMAL;
	map.addObserver(this);
	if (map.getTileset() != null) {
	    map.getTileset().addObserver(this);
	}

	update(map, null);
    }

    /**
     * Removes the selected tile from the map.
     */
    private void destroySelectedTile() {
	state = STATE_NORMAL;
	map.removeSelectedTile();
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

    /**
     * Returns whether or not the component is focusable.
     * @return true
     */
    public boolean isFocusable() {
	return true;
    }
    
    /**
     * This function is called when the map, the tileset or the configuration changes.
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

	    // not necessary?
// 	    else {
// 		TileOnMap newSelectedTile = map.getSelectedTile();
// 		if (newSelectedTile != currentSelectedTile) {
// 		    // observe the new selected tile
		
// 		    if (currentSelectedTile != null) {
// 			currentSelectedTile.deleteObserver(this);
// 		    }
		    
// 		    if (newSelectedTile != null) {
// 			newSelectedTile.addObserver(this);
// 		    }
		
// 		    currentSelectedTile = newSelectedTile;
// 		}
// 	    }
	    
	    // redraw the image
	    repaint();
	}
// 	else if (o instanceof TileOnMap) {
// 	    // the selected tile in the map has changed
	    
	    
// 	}
	else if (o instanceof Configuration && map != null) {
	    // TODO
	}
	else if (o instanceof Tileset) {
	    // the selected tile in the tileset has changed
	    
	    Tileset tileset = map.getTileset();
	    if (tileset.getSelectedTile() == null) {
		// no tile is selected anymore in the tileset
		state = STATE_NORMAL;
		repaint();
	    }
	    else if (map.getSelectedTile() != null) {

		// if a tile was just selected in the tileset whereas there is already
		// a tile selected in the map, unselected the tile in the map
		state = STATE_NORMAL;
		map.setSelectedTile(null);
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

		// the tiles
		for (int layer = 0; layer < 3; layer++) {
		    TileOnMapVector tiles = map.getTiles(layer);
		    for (int i = 0; i < tiles.size(); i++) {
			
			TileOnMap tileOnMap = tiles.get(i);
			Tile tile = tileOnMap.getTile();

			// source image
			Rectangle positionInTileset = tile.getPositionInTileset();
			int sx1 = positionInTileset.x * 2;
			int sx2 = sx1 + positionInTileset.width * 2;
			int sy1 = positionInTileset.y * 2;
			int sy2 = sy1 + positionInTileset.height * 2;
			
			// destination image
			Rectangle positionInMap = tileOnMap.getPositionInMap();
			int dx1 = positionInMap.x * 2;
			int dx2 = dx1 + positionInMap.width * 2;
			int dy1 = positionInMap.y * 2;
			int dy2 = dy1 + positionInMap.height * 2;
			
			g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
		    } // for
		} // for

		// selected tile in the tileset
		if (state == STATE_ADDING_TILE) {
		    Tile selectedTileInTileset = tileset.getSelectedTile();
		    Rectangle positionInTileset = selectedTileInTileset.getPositionInTileset();

		    int width = positionInTileset.width;
		    int height = positionInTileset.height;
			
		    int sx1 = positionInTileset.x * 2;
		    int sx2 = sx1 + width * 2;
		    int sy1 = positionInTileset.y * 2;
		    int sy2 = sy1 + height * 2;
			
		    int dx1 = cursorLocation.x;
		    int dx2 = dx1 + width * 2;
		    int dy1 = cursorLocation.y;
		    int dy2 = dy1 + height * 2;
			
		    g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
		}

		// selected tile in the map
		else if (state == STATE_TILE_SELECTED) {
		    
		    TileOnMap selectedTileInMap = map.getSelectedTile();
		    
		    g.setColor(Color.GREEN);

		    Rectangle selectedRectangle = selectedTileInMap.getPositionInMap();
		    int x1 = selectedRectangle.x * 2;
		    int x2 = (selectedRectangle.x + selectedRectangle.width) * 2 - 1;
		    int y1 = selectedRectangle.y * 2;
		    int y2 = (selectedRectangle.y + selectedRectangle.height) * 2 - 1;
		    
		    g.drawLine(x1, y1, x2, y1);
		    g.drawLine(x2, y1, x2, y2);
		    g.drawLine(x2, y2, x1, y2);
		    g.drawLine(x1, y2, x1, y1);
		    
		    g.drawLine(x1, y1 + 1, x2, y1 + 1);
		    g.drawLine(x2 - 1, y1, x2 - 1, y2);
		    g.drawLine(x2, y2 - 1, x1, y2 - 1);
		    g.drawLine(x1 + 1, y2, x1 + 1, y1);
		}
	    }
	}
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
	 * If a tile is selected in the tileset, an instance of this tile is added to the map
	 * at the cursor location. Otherwise, the tile clicked becomes selected in the map.
	 * A right click on a tile of the map shows a popup menu.
	 */
	public void mouseClicked(MouseEvent mouseEvent) {

	    if (!isImageLoaded()) {
		return;
	    }

	    requestFocusInWindow();

	    switch (state) {

	    case STATE_NORMAL:
	    case STATE_TILE_SELECTED:
		
		// select the tile clicked
		int x = mouseEvent.getX();
		int y = mouseEvent.getY();
		map.setSelectedTile(x / 2, y / 2);

		// a tile has just been selected
		if (map.getSelectedTile() != null) {
		    state = STATE_TILE_SELECTED;

		    // right click: popup menu on the selected tile
		    if (mouseEvent.getButton() == MouseEvent.BUTTON3) {
			
			int layer = map.getSelectedTile().getLayer();
			itemsLayers[layer].setSelected(true);
			popupMenuSelectedTile.show(MapView.this,
						   mouseEvent.getX(),
						   mouseEvent.getY());
		    }
		}
		else {
		    state = STATE_NORMAL;
		}

		break;

	    case STATE_ADDING_TILE:
	    // the user is adding a tile
		
		// left click: place the new tile
		if (mouseEvent.getButton() == MouseEvent.BUTTON1) {

		    // place a new tile
		    Tileset tileset = map.getTileset();
		    map.addTile(new TileOnMap(tileset.getSelectedTile(), tileset.getSelectedTileIndex(),
					      cursorLocation.x / 2, cursorLocation.y / 2));
		}

		// right click: cancel
		else {
		    map.getTileset().setSelectedTileIndex(-1);
		}

		break;

	    case STATE_RESIZING_TILE:
		// TODO
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

		int x = mouseEvent.getX();
		int y = mouseEvent.getY();

		// if we are adding a tile but the mouse is outside the map, 
		// remove the tile displayed under the cursor
		if (state == STATE_ADDING_TILE && (x >= map.getWidth() * 2 || y >= map.getHeight() * 2)) {

		    state = STATE_NORMAL;
		    repaint();
		}
		else {

		    // if a tile is selected in the tileset, make the state STATE_ADDING_TILE
		    Tile selectedTileInTileset = map.getTileset().getSelectedTile();
		    
		    if (selectedTileInTileset != null && state != STATE_ADDING_TILE) {
			state = STATE_ADDING_TILE;
		    }
		}

		// update the cursor location if necessary
		if (state == STATE_ADDING_TILE || state == STATE_RESIZING_TILE) {

		    // snap the tile to the 16*16 grid
		    x -= x % 16;
		    y -= y % 16;
		    
		    if (x != cursorLocation.x || y != cursorLocation.y) {
			cursorLocation.x = x;
			cursorLocation.y = y;
			repaint();
		    }
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

	    if (keyEvent.getKeyCode() == KeyEvent.VK_DELETE && state == STATE_TILE_SELECTED) {
		destroySelectedTile();
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
	 * Method called when the user sets the layer of the selected tile.
	 */
	public void actionPerformed(ActionEvent e) {
	    TileOnMap tileOnMap = map.getSelectedTile();
	    int currentLayer = tileOnMap.getLayer();

	    if (currentLayer != layer) {
		map.selectedTileSetLayer(layer);
	    }
	}
    }
}
