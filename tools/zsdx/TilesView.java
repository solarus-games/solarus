package zsdx;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

/**
 * This component shows a list of tiles and allows the user to select them.
 * The list is displayed as a JList with an icon for each tile.
 * The id of the tile in the tileset is different from it index in the list items,
 * because in a tileset, when a tile is removed, the other tiles keep their ids.
 */
public class TilesView extends JPanel {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * Component with the properties of the tileset.
     */
    private TilesetPropertiesView tilesetPropertiesView;

    /**
     * The tile list.
     */
    private JList tileList;

    /**
     * The tile list model.
     */
    private TileListModel tileListModel;

    /**
     * The icon associated to each tile.
     */
    private ArrayList<TileIcon> tileIcons;

    /**
     * View of the selected tile.
     */
    private SelectedTileView tileView;

    /**
     * Constructor.
     */
    public TilesView() {
	super();

	tileIcons = new ArrayList<TileIcon>();

	setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
	setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

	// tileset properties
	// the tile list
	// view of the current tile

	// tileset properties
	tilesetPropertiesView = new TilesetPropertiesView();
	tilesetPropertiesView.setMaximumSize(new Dimension(Integer.MAX_VALUE, 120));
	tilesetPropertiesView.setAlignmentX(Component.LEFT_ALIGNMENT);

	// list
	tileListModel = new TileListModel();
	tileList = new JList(tileListModel);
	tileList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	tileList.setLayoutOrientation(JList.HORIZONTAL_WRAP);
	tileList.setVisibleRowCount(-1); // make the rows as wide as possible
 	tileList.getSelectionModel().addListSelectionListener(new TileListSelectionListener());
	tileList.setCellRenderer(new TileListRenderer());

	tileList.addKeyListener(new KeyAdapter() {
		public void keyPressed(KeyEvent keyEvent) {
		    if (keyEvent.getKeyCode() == KeyEvent.VK_DELETE) {
			if (tileset != null && tileset.getSelectedTile() != null) {
			    tileset.removeTile();
			}
		    }
		}
	    });

	JScrollPane listScroller = new JScrollPane(tileList);
 	listScroller.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));
	listScroller.setAlignmentX(Component.LEFT_ALIGNMENT);

	// tile view
	tileView = new SelectedTileView();
 	tileView.setMaximumSize(new Dimension(Integer.MAX_VALUE, 300));
	tileView.setAlignmentX(Component.LEFT_ALIGNMENT);

	add(tilesetPropertiesView);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(listScroller);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(tileView);
    }

    /**
     * Sets the tileset observed.
     * @param tileset the tileset
     */
    public void setTileset(Tileset tileset) {
	if (tileset != this.tileset) {

	    if (this.tileset != null) {
		tileset.deleteObserver(tileListModel);
	    }

	    this.tileset = tileset;

	    tileset.addObserver(tileListModel);
	    tilesetPropertiesView.setTileset(tileset);
	    tileView.setTileset(tileset);

	    loadIcons();
	    tileListModel.update(tileset, null);
	}
    }

    /**
     * Loads the icons for the tile list.
     */
    private void loadIcons() {

	tileIcons.clear();

	for (Tile tile: tileset.getTiles()) {
	    tileIcons.add(new TileIcon(tile, tileset));
	}
    }

    /**
     * List selection listener associated to the tile table.
     * When the user selects a row, the corresponding tile becomes selected in the tileset.
     */
    private class TileListSelectionListener implements ListSelectionListener {

	/**
	 * This function is called when the selection is changed.
	 */
	public void valueChanged(ListSelectionEvent e) {

	    // get the rank of the tile just selected in the list (0 to nbTiles)
	    int selectedTileRank = tileList.getSelectedIndex();

	    // select this tile in the tileset
	    if (selectedTileRank != -1) {
		tileset.setSelectedTileId(tileset.tileRankToTileId(selectedTileRank));
	    }
	    else {
		tileset.unSelectTile();
	    }
	}
    }
    
    /**
     * List model for the tile list.
     */
    private class TileListModel extends AbstractListModel implements Observer {

	/**
	 * Returns the number of tiles.
	 */
	public int getSize() {
	    int size = 0;

	    if (tileset != null) {
		size = tileset.getNbTiles();
	    }

	    return size;
	}
	
	/**
	 * Returns the a component with a 16*16 image of the specified tile. 
	 */
	public Object getElementAt(int rank) {

	    return tileset.getTile(tileset.tileRankToTileId(rank));
	}

	/**
	 * This function is called when the tileset changes.
	 * @param o the tileset
	 * @param params information about what has changed:
	 *   - a Tile: indicates that this tile has just been created
	 *   - an Integer: indicates that the tile with this id has just been removed
	 *   - null: other cases
	 */
	public void update(Observable o, Object params) {

	    // reload the icons if a tile was added or removed
	    if (params instanceof Integer || params instanceof Tile) {
		loadIcons();
	    }

// 	    // was a tile just removed?
// 	    if (params instanceof Integer) {
// 		int deletedTileId = ((Integer) params).intValue();
// 		tileIcons.remove(deletedTileId);		
// 	    }

// 	    // was a tile just created?
// 	    else if (params instanceof Tile) {
// 		Tile tileAdded = (Tile) params;
// 		tileIcons.add(new TileIcon(tileAdded, tileset));		
// 	    }

	    // update the enabled state of the buttons
	    int tilesetSelectedTileId = tileset.getSelectedTileId();
	    int listSelectedRank = tileList.getSelectedIndex();

	    if (tileset.getSelectedTile() != null) {
		// an existing tile is selected
		// make this tile selected in the list
		int listRank = tileset.tileIdToTileRank(tilesetSelectedTileId);
		if (listRank != listSelectedRank) {
		    tileList.setSelectedIndex(listRank);
		    tileList.ensureIndexIsVisible(listRank);
		}
	    }
	    else if (tileset.getSelectedTile() == null) {
		// no tile is selected anymore
		tileList.removeSelectionInterval(listSelectedRank, listSelectedRank);
	    }
	    // redraw the table
	    fireContentsChanged(this, 0, tileset.getNbTiles() - 1);

	    // notify the tile view
	    tileView.setCurrentTile(tileset.getSelectedTile());
	}
    }

    /**
     * This class defines how the list elements are displayed.
     */
    private class TileListRenderer implements ListCellRenderer {

	/**
	 * Returns a component representing a tile.
	 */
	public Component getListCellRendererComponent(JList list, Object value, int rank,
						      boolean isSelected, boolean cellHasFocus) {
	    if (rank >= tileIcons.size()) {
		// the icon doesn't exist yet
		loadIcons();
	    }

	    return tileIcons.get(rank);
	}
    }
}
