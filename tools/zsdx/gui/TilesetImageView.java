/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import zsdx.entities.*;

/**
 * This component shows the tileset image and allow the user to select the tile patterns.
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
    private TilePattern currentSelectedTilePattern;

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
     * A popup menu with an item "Create", displayed when the user selects a new tile pattern.
     */
    private JPopupMenu popupMenuCreate;

    /**
     * A popup menu with an item "Obstacle" and an item "Delete", displayed
     * when the user right-clicks on a tile pattern.
     */
    private JPopupMenu popupMenuSelectedTilePattern;

    /**
     * Items in the popup menu to set the type of obstacle of the selected tile pattern.
     */
    private JRadioButtonMenuItem[] itemsObstacle;

    /**
     * Constructor.
     * @param editable true to make the tileset editable, false otherwise
     * (if so, the user can only pick a tile pattern)
     */
    public TilesetImageView(boolean editable) {
	super();

	this.editable = editable;

	addMouseListener(new TilesetImageMouseListener());
	addMouseMotionListener(new TilesetImageMouseMotionListener());

	if (editable) {
	    JMenuItem item;
	    JMenuItem itemCancelCreate = new JMenuItem("Cancel");

	    // popup menu to create a tile pattern
	    popupMenuCreate = new JPopupMenu();

	    item = new JMenuItem("Create (no obstacle)", ObstacleIcons.getIcon(MapEntity.OBSTACLE_NONE));
	    item.addActionListener(new ActionCreateTilePattern(MapEntity.OBSTACLE_NONE));
	    popupMenuCreate.add(item);

	    item = new JMenuItem("Create (obstacle)", ObstacleIcons.getIcon(MapEntity.OBSTACLE));
	    item.addActionListener(new ActionCreateTilePattern(MapEntity.OBSTACLE));
	    popupMenuCreate.add(item);

	    itemCancelCreate.addActionListener(new ActionListener() {
		    public void actionPerformed(ActionEvent e) {
			tileset.unselectTilePattern();
		    }
		});
	    popupMenuCreate.addSeparator();
	    popupMenuCreate.add(itemCancelCreate);

	    // popup menu for a selected tile pattern
	    popupMenuSelectedTilePattern = new JPopupMenu();
	    itemsObstacle = new JRadioButtonMenuItem[ObstacleIcons.NB_ICONS];
	    ButtonGroup itemsObstacleGroup = new ButtonGroup();

	    for (int i = 0; i < ObstacleIcons.NB_ICONS; i++) {
		itemsObstacle[i] = new JRadioButtonMenuItem(ObstacleIcons.getName(i),
							    ObstacleIcons.getIcon(i));
		itemsObstacle[i].addActionListener(new ActionChangeObstacle(i));
		popupMenuSelectedTilePattern.add(itemsObstacle[i]);
		itemsObstacleGroup.add(itemsObstacle[i]);
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
	    // the selected tile pattern has changed
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

	    TilePattern selectedTilePattern = null;
	    Image scaledImage = tileset.getDoubleImage();

	    // draw the image
// 	    g.drawImage(scaledImage, 0, 0, scaledImage.getWidth(this) * 2, scaledImage.getHeight(this) * 2, this);
	    g.drawImage(scaledImage, 0, 0, null);

	    // determine the selected area
	    Rectangle selectedRectangle = null;
	    if (tileset.isSelectingNewTilePattern()) {

		// the selected tile doesn't exist yet
		selectedRectangle = tileset.getNewTilePatternArea();

		if (selectedRectangle.width == 0
		    || selectedRectangle.height == 0
		    || tileset.isNewTilePatternAreaOverlapping()) {
		    // invalid area
		    g.setColor(Color.RED);
		}
		else {
		    // valid area
		    g.setColor(Color.GREEN);
		}
	    }
	    else {
		selectedTilePattern = tileset.getSelectedTilePattern();

		if (selectedTilePattern != null) {
		    // an existing tile pattern is selected
		    selectedRectangle = selectedTilePattern.getPositionInTileset();
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

		// for an animated tile pattern, also draw the separation between the 3 frames
		if (selectedTilePattern != null && selectedTilePattern.isAnimated()) {

		    if (selectedTilePattern.getAnimationSeparation() == TilePattern.ANIMATION_SEPARATION_HORIZONTAL) {
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
		
		int x = Math.min(Math.max(mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this)) / 2;
		int y = Math.min(Math.max(mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this)) / 2;
		
		// search the tile pattern clicked
		int clickedTileId = tileset.getTilePatternIdAt(x, y);

		// a tile pattern was just clicked
		if (clickedTileId > 0) {

		    // select the tile pattern
		    tileset.setSelectedTilePatternId(clickedTileId);
		    
		    // right click: if the tileset is editable, we show a popup menu
		    if (mouseEvent.getButton() == MouseEvent.BUTTON3 && editable) {
			int obstacle = tileset.getSelectedTilePattern().getObstacle();
			itemsObstacle[obstacle].setSelected(true);
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

	    // only consider left clicks
	    if (isImageLoaded() && mouseEvent.getButton() == MouseEvent.BUTTON1) {
		
		Image scaledImage = tileset.getDoubleImage();

		int x = Math.min(Math.max(mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this)) / 2;
		int y = Math.min(Math.max(mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this)) / 2;

		TilePattern selectedTilePattern = tileset.getSelectedTilePattern();

		if (selectedTilePattern != null && !selectedTilePattern.getPositionInTileset().contains(x,y)) {
		    // an existing tile pattern was selected and the user pressed the mouse button outside: unselect it
		    tileset.unselectTilePattern();
		}

		else if (tileset.isSelectingNewTilePattern()) {
		    // a new tile pattern was selected: unselect it
		    tileset.unselectTilePattern();
		}

		// begin a selection
		if (!tileset.isSelectingNewTilePattern() && editable) {
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

		// keep the new selected tile pattern only if it really exists
		Rectangle newTilePatternArea = tileset.getNewTilePatternArea();
		if (tileset.isSelectingNewTilePattern()
		    && newTilePatternArea != null
		    && newTilePatternArea.width > 0
		    && newTilePatternArea.height > 0
		    && !tileset.isNewTilePatternAreaOverlapping()) {

		    // the area is valid: show a popup menu with an item "Create"
		    popupMenuCreate.show(TilesetImageView.this,
					 mouseEvent.getX(),
					 mouseEvent.getY());
		}
		else {
		    // the area doesn't exist or is not valid: we cancel the selection
		    selectionStartPoint = null;
		    selectionCurrentPoint = null;
		    tileset.unselectTilePattern();
		    newTilePatternArea = null;
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

		    Rectangle newTilePatternArea = new Rectangle();

		    // the selected area has changed: recalculate the rectangle
		    if (selectionStartPoint.x < selectionCurrentPoint.x) {
			newTilePatternArea.x = selectionStartPoint.x;
			newTilePatternArea.width = selectionCurrentPoint.x - selectionStartPoint.x;
		    }
		    else {
			newTilePatternArea.x = selectionCurrentPoint.x;
			newTilePatternArea.width = selectionStartPoint.x - selectionCurrentPoint.x;
		    }
		    
		    if (selectionStartPoint.y < selectionCurrentPoint.y) {
			newTilePatternArea.y = selectionStartPoint.y;
			newTilePatternArea.height = selectionCurrentPoint.y - selectionStartPoint.y;
		    }
		    else {
			newTilePatternArea.y = selectionCurrentPoint.y;
			newTilePatternArea.height = selectionStartPoint.y - selectionCurrentPoint.y;
		    }
		    
		    tileset.startSelectingNewTilePattern();
		    tileset.setNewTilePatternArea(newTilePatternArea);
		}
	    }
	}
    }

    /**
     * Action listener invoked when the user creates a tile pattern.
     */
    private class ActionCreateTilePattern implements ActionListener {

	/**
	 * Type of obstacle to set when the action is invoked.
	 */
	private int obstacle;

	/**
	 * Constructor.
	 * @param obstacle type of obstacle of the tile pattern to create
	 */
	public ActionCreateTilePattern(int obstacle) {
	    this.obstacle = obstacle;
	}

	/**
	 * Method called when the user wants to create a tile pattern.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		tileset.addTilePattern(obstacle);
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
     * Action listener invoked when the user changes the type of obstacle of a tile
     * pattern from the popup menu after a right click.
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
	 * Method called when the user sets the type of obstacle of the selected tile pattern.
	 */
	public void actionPerformed(ActionEvent ev) {
	    TilePattern tilePattern = tileset.getSelectedTilePattern();
	    int currentObstacle = tilePattern.getObstacle();

	    if (currentObstacle != obstacle) {

		try {
		    tilePattern.setObstacle(obstacle);
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
