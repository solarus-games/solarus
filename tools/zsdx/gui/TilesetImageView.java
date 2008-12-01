package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import zsdx.entities.*;

/**
 * This component shows the tileset image and allow the user to select the tiles.
 */
public class TilesetImageView extends JComponent implements Observer, Scrollable {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * True if the tileset is editable, false if the user can only pick a tile.
     */
    private boolean editable;

    /**
     * The current selected tile.
     */
    private Tile currentSelectedTile;

    // information about the selection

    /**
     * Point where the selection started,
     * or null if there in no new tile selection.
     */
    private Point selectionStartPoint;

    /**
     * Point where the selection ends currently,
     * or null if there in no new tile selection.
     */
    private Point selectionCurrentPoint;

    /**
     * A popup menu with an item "Create", displayed when the user selects a new tile.
     */
    private JPopupMenu popupMenuCreate;

    /**
     * A popup menu with an item "Obstacle" and an item "Delete", displayed
     * when the user right-clicks on a tile.
     */
    private JPopupMenu popupMenuSelectedTile;

    /**
     * Items in the popup menu to set the type of obstacle of the selected tile.
     */
    private JRadioButtonMenuItem[] itemsObstacle;

    /**
     * Constructor.
     * @param editable true to make the tileset editable, false otherwise
     * (if so, the user can only pick a tile)
     */
    public TilesetImageView(boolean editable) {
	super();

	this.editable = editable;

	addMouseListener(new TilesetImageMouseListener());
	addMouseMotionListener(new TilesetImageMouseMotionListener());

	if (editable) {
	    JMenuItem item;
	    JMenuItem itemCancelCreate = new JMenuItem("Cancel");
	    
	    // popup menu to create a tile
	    popupMenuCreate = new JPopupMenu();

	    item = new JMenuItem("Create (no obstacle)", ObstacleIcons.getIcon(MapEntity.OBSTACLE_NONE));
	    item.addActionListener(new ActionCreateTile(MapEntity.OBSTACLE_NONE));
	    popupMenuCreate.add(item);

	    item = new JMenuItem("Create (obstacle)", ObstacleIcons.getIcon(MapEntity.OBSTACLE));
	    item.addActionListener(new ActionCreateTile(MapEntity.OBSTACLE));
	    popupMenuCreate.add(item);

	    itemCancelCreate.addActionListener(new ActionListener() {
		    public void actionPerformed(ActionEvent e) {
			tileset.unselectTile();
		    }
		});
	    popupMenuCreate.addSeparator();
	    popupMenuCreate.add(itemCancelCreate);
	    
	    // popup menu for a selectedTile
	    popupMenuSelectedTile = new JPopupMenu();
	    itemsObstacle = new JRadioButtonMenuItem[6];
	    ButtonGroup itemsObstacleGroup = new ButtonGroup();
	    
	    for (int i = 0; i < 6; i++) {
		itemsObstacle[i] = new JRadioButtonMenuItem(ObstacleIcons.getName(i),
							    ObstacleIcons.getIcon(i));
		itemsObstacle[i].addActionListener(new ActionChangeObstacle(i));
		popupMenuSelectedTile.add(itemsObstacle[i]);
		itemsObstacleGroup.add(itemsObstacle[i]);
	    }
	    
	    popupMenuSelectedTile.addSeparator();
	    
	    item = new JMenuItem("Delete");
	    item.addActionListener(new ActionListener() {
		    public void actionPerformed(ActionEvent e) {
			tileset.removeTile();
		    }
		});
	    popupMenuSelectedTile.add(item);
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
	    update(tileset.getSelectedTile(), null);
	    
	    // load the tileset's image
	    tileset.reloadImage();
	    update(tileset, null);
	}

	setSize(getPreferredSize());
    }

    /**
     * This function is called when the tileset or the selected tile changes.
     */
    public void update(Observable o, Object obj) {

	if (o instanceof Tileset) {
	    // the tileset has changed

	    Tile newSelectedTile = tileset.getSelectedTile();
	    if (newSelectedTile != currentSelectedTile) {
		// observe the new selected tile
		
		if (currentSelectedTile != null) {
		    currentSelectedTile.deleteObserver(this);
		}
		
		if (newSelectedTile != null) {
		    newSelectedTile.addObserver(this);
		}
		
		currentSelectedTile = newSelectedTile;
	    }

	    // redraw the image
	    repaint();
	}
	else if (o instanceof Tile) {
	    // the selected tile has changed
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
     * This function is called to display the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {

	if (isImageLoaded()) { // the image exists

	    Tile selectedTile = null;
	    Image scaledImage = tileset.getDoubleImage();

	    // draw the image
// 	    g.drawImage(scaledImage, 0, 0, scaledImage.getWidth(this) * 2, scaledImage.getHeight(this) * 2, this);
	    g.drawImage(scaledImage, 0, 0, null);

	    // determine the selected area
	    Rectangle selectedRectangle = null;
	    if (tileset.isSelectingNewTile()) {

		// the selected tile doesn't exist yet
		selectedRectangle = tileset.getNewTileArea();

		if (selectedRectangle.width == 0
		    || selectedRectangle.height == 0
		    || tileset.isNewTileAreaOverlapping()) {
		    // invalid area
		    g.setColor(Color.RED);
		}
		else {
		    // valid area
		    g.setColor(Color.GREEN);
		}
	    }
	    else {
		selectedTile = tileset.getSelectedTile();

		if (selectedTile != null) {
		    // an existing tile is selected
		    selectedRectangle = selectedTile.getPositionInTileset();
		    g.setColor(Color.BLUE);
		}
	    }
	    
	    // draw the selected rectangle
	    if (selectedRectangle != null) {

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

		// for an animated tile, also draw the separation between the 3 frames
		if (selectedTile != null && selectedTile.isAnimated()) {

		    if (selectedTile.getAnimationSeparation() == Tile.ANIMATION_SEPARATION_HORIZONTAL) {
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
	 * If no tile was selected, then the tile clicked (if any) is selected.
	 * For a right click: a popup menu allows to remove the tile clicked
	 * (if the tileset is editable).
	 * @param mouseEvent information about the click
	 */
	public void mouseClicked(MouseEvent mouseEvent) {
	    if (isImageLoaded()) {

		Image scaledImage = tileset.getDoubleImage();
		
		int x = Math.min(Math.max(mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this)) / 2;
		int y = Math.min(Math.max(mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this)) / 2;
		
		// search the tile clicked
		int clickedTileId = tileset.getIdOfTileAt(x, y);

		// a tile was just clicked
		if (clickedTileId > 0) {

		    // select the tile
		    tileset.setSelectedTileId(clickedTileId);
		    
		    // right click: if the tileset is editable, we show a popup menu
		    if (mouseEvent.getButton() == MouseEvent.BUTTON3 && editable) {
			int obstacle = tileset.getSelectedTile().getObstacle();
			itemsObstacle[obstacle].setSelected(true);
			popupMenuSelectedTile.show(TilesetImageView.this,
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
	 * If a tile was selected, it becomes unselected. Otherwise, the selection for a new tile
	 * begins (only if the tileset is editable).
	 * @param mouseEvent information about the click
	 */
	public void mousePressed(MouseEvent mouseEvent) {

	    // only consider left clicks
	    if (isImageLoaded() && mouseEvent.getButton() == MouseEvent.BUTTON1) {
		
		Image scaledImage = tileset.getDoubleImage();

		int x = Math.min(Math.max(mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this)) / 2;
		int y = Math.min(Math.max(mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this)) / 2;

		Tile selectedTile = tileset.getSelectedTile();

		if (selectedTile != null && !selectedTile.getPositionInTileset().contains(x,y)) {
		    // an existing tile was selected and the user pressed the mouse button outside: unselect it
		    tileset.unselectTile();
		}

		else if (tileset.isSelectingNewTile()) {
		    // a new tile was selected: unselect it
		    tileset.unselectTile();
		}

		// begin a selection
		if (!tileset.isSelectingNewTile() && editable) {
		    selectionStartPoint = mouseEvent.getPoint();

		    selectionStartPoint.x = (x + 4) / 8 * 8;
		    selectionStartPoint.y = (y + 4) / 8 * 8;
		}
	    }
	}

	/**
	 * This function is called when the mouse button is released on the tileset image.
	 * Only left clicks are considered.
	 * @param mouseEvent information about the click
	 */
	public void mouseReleased(MouseEvent mouseEvent) {

	    // only consider left clicks
	    if (isImageLoaded() && mouseEvent.getButton() == MouseEvent.BUTTON1) {

		// keep the new selected tile only if it really exists
		Rectangle newTileArea = tileset.getNewTileArea();
		if (tileset.isSelectingNewTile()
		    && newTileArea != null
		    && newTileArea.width > 0
		    && newTileArea.height > 0
		    && !tileset.isNewTileAreaOverlapping()) {

		    // the area is valid: show a popup menu with an item "Create"
		    popupMenuCreate.show(TilesetImageView.this,
					 mouseEvent.getX(),
					 mouseEvent.getY());
		}
		else {
		    // the area doesn't exist or is not valid: we cancel the selection
		    selectionStartPoint = null;
		    selectionCurrentPoint = null;
		    tileset.unselectTile();
		    newTileArea = null;
		}
	    }
	}
    }

    /**
     * The mouse motion listener associated to the tileset image.
     */
    private class TilesetImageMouseMotionListener extends MouseMotionAdapter {
	
	/**
	 * This method is called when a mouse button is pressed on the component and then dragged.
	 * It is called again until the mouse button is released.
	 */
	public void mouseDragged(MouseEvent mouseEvent) {

	    if (isImageLoaded() && selectionStartPoint != null) {

		Image scaledImage = tileset.getDoubleImage();

		// compute the selected area
		Point selectionPreviousPoint = selectionCurrentPoint;
		selectionCurrentPoint = mouseEvent.getPoint();

		selectionCurrentPoint.x = Math.min(Math.max(mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this));
		selectionCurrentPoint.y = Math.min(Math.max(mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this));

		selectionCurrentPoint.x = (selectionCurrentPoint.x + 8) / 16 * 8;
		selectionCurrentPoint.y = (selectionCurrentPoint.y + 8) / 16 * 8;
		
		if (!selectionCurrentPoint.equals(selectionPreviousPoint)) {

		    Rectangle newTileArea = new Rectangle();

		    // the selected area has changed: recalculate the rectangle
		    if (selectionStartPoint.x < selectionCurrentPoint.x) {
			newTileArea.x = selectionStartPoint.x;
			newTileArea.width = selectionCurrentPoint.x - selectionStartPoint.x;
		    }
		    else {
			newTileArea.x = selectionCurrentPoint.x;
			newTileArea.width = selectionStartPoint.x - selectionCurrentPoint.x;
		    }
		    
		    if (selectionStartPoint.y < selectionCurrentPoint.y) {
			newTileArea.y = selectionStartPoint.y;
			newTileArea.height = selectionCurrentPoint.y - selectionStartPoint.y;
		    }
		    else {
			newTileArea.y = selectionCurrentPoint.y;
			newTileArea.height = selectionStartPoint.y - selectionCurrentPoint.y;
		    }
		    
		    tileset.startSelectingNewTile();
		    tileset.setNewTileArea(newTileArea);
		}
	    }
	}
    }

    /**
     * Action listener invoked when the user creates a tile.
     */
    private class ActionCreateTile implements ActionListener {

	/**
	 * Type of obstacle to set when the action is invoked.
	 */
	private int obstacle;

	/**
	 * Constructor.
	 * @param obstacle type of obstacle of the tile to create
	 */
	public ActionCreateTile(int obstacle) {
	    this.obstacle = obstacle;
	}

	/**
	 * Method called when the user wants to create a tile.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		tileset.addTile(obstacle);
	    }
	    catch (TilesetException e) {
		JOptionPane.showMessageDialog(null,
					      "Unable to create the tile: " + e.getMessage(),
					      "Error",
					      JOptionPane.ERROR_MESSAGE);
	    }
	}
    }

    /**
     * Action listener invoked when the user changes the type of obstacle of a tile
     * from the popup menu after a right click.
     */
    private class ActionChangeObstacle implements ActionListener {

	/**
	 * Type of obstacle to set when the action is invoked.
	 */
	private int obstacle;

	/**
	 * Constructor.
	 * @param obstacle type of obstacle to set when the action is invoked.
	 */
	public ActionChangeObstacle(int obstacle) {
	    this.obstacle = obstacle;
	}

	/**
	 * Method called when the user sets the type of obstacle of the selected tile.
	 */
	public void actionPerformed(ActionEvent ev) {
	    Tile tile = tileset.getSelectedTile();
	    int currentObstacle = tile.getObstacle();

	    if (currentObstacle != obstacle) {

		try {
		    tile.setObstacle(obstacle);
		}
		catch (TilesetException e) {
		    JOptionPane.showMessageDialog(null,
						  "Unable to set this type of obstacle: " + e.getMessage(),
						  "Error",
						  JOptionPane.ERROR_MESSAGE);
		}
	    }
	}
    }
}
