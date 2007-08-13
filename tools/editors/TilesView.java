package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

/**
 * This component shows a list of tiles and allows the user to select them.
 * The list is displayed as a JList with an icon for each tile.
 * The indexes of the tiles are different from the indexes of the list items,
 * because in a tileset, when a tile is removed, the other tiles keep the same
 * indexes.
 */
public class TilesView extends JPanel {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * Text indicating the tileset name.
     */
    private JLabel labelTilesetName;

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
    private Vector<TileIcon> tileIcons;

    // the buttons
    private JButton buttonCreate;
    private JButton buttonDelete;

    /**
     * View of the selected tile.
     */
    private SelectedTileView tileView;

    /**
     * Constructor.
     */
    public TilesView() {
	super();

	tileIcons = new Vector<TileIcon>();

	setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
	setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

	// tileset name
	// buttons
	// the tile list
	// view of the current tile

	// tileset name
	labelTilesetName = new JLabel("Tileset name: ");
	labelTilesetName.setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
	labelTilesetName.setAlignmentX(Component.LEFT_ALIGNMENT);

	// buttons
	JPanel buttons = new JPanel(new GridLayout(1, 3, 5, 0));
	buttons.setMinimumSize(new Dimension(200, 30));
	buttons.setPreferredSize(new Dimension(200, 30));
	buttons.setMaximumSize(new Dimension(200, 30));
	buttons.setAlignmentX(Component.LEFT_ALIGNMENT);

	buttonCreate = new JButton("Create");
	buttonDelete = new JButton("Delete");
	buttonCreate.setEnabled(false);
	buttonDelete.setEnabled(false);
	buttons.add(buttonCreate);
	buttons.add(buttonDelete);

	buttonCreate.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    tileset.addTile(Tile.OBSTACLE_NONE);
		}
	    });

	buttonDelete.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    tileset.removeTile();
		}
	    });

	// list
	tileListModel = new TileListModel();
	tileList = new JList(tileListModel);
	tileList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	tileList.setLayoutOrientation(JList.HORIZONTAL_WRAP);
	tileList.setVisibleRowCount(-1); // make the rows as wide as possible
 	tileList.getSelectionModel().addListSelectionListener(new TileListSelectionListener());
	tileList.setCellRenderer(new TileListRenderer());

	JScrollPane listScroller = new JScrollPane(tileList);
 	listScroller.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));
	listScroller.setAlignmentX(Component.LEFT_ALIGNMENT);

	// tile view
	tileView = new SelectedTileView();
 	tileView.setMaximumSize(new Dimension(Integer.MAX_VALUE, 300));
	tileView.setAlignmentX(Component.LEFT_ALIGNMENT);

	add(labelTilesetName);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(buttons);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(listScroller);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(tileView);
    }

    /**
     * Sets the observed tileset.
     */
    public void setTileset(Tileset tileset) {
	if (tileset != this.tileset) {

	    if (this.tileset != null) {
		tileset.deleteObserver(tileListModel);
	    }

	    this.tileset = tileset;

	    tileset.addObserver(tileListModel);
	    
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
		tileset.setSelectedTileIndex(tileset.tileRankToTileIndex(selectedTileRank));
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

	    return tileset.getTile(tileset.tileRankToTileIndex(rank));
	}

	/**
	 * This function is called when the tileset changes.
	 * @param o the tileset
	 * @param params information about what has changed:
	 *   - a Tile: indicates that this tile has just been created
	 *   - an Integer: indicates that the tile at this index has just been removed
	 *   - null: other cases
	 */
	public void update(Observable o, Object params) {

	    // update the tileset name
	    labelTilesetName.setText("Tileset name: " + tileset.getName());

	    // reload the icons if a tile was added or removed
	    if (params instanceof Integer || params instanceof Tile) {
		loadIcons();
	    }

// 	    // was a tile just removed?
// 	    if (params instanceof Integer) {
// 		int deletedTileIndex = ((Integer) params).intValue();
// 		tileIcons.remove(deletedTileIndex);		
// 	    }

// 	    // was a tile just created?
// 	    else if (params instanceof Tile) {
// 		Tile tileAdded = (Tile) params;
// 		tileIcons.add(new TileIcon(tileAdded, tileset));		
// 	    }

	    // update the enabled state of the buttons
	    int tilesetSelectedIndex = tileset.getSelectedTileIndex();
	    int listSelectedRank = tileList.getSelectedIndex();

	    buttonCreate.setEnabled(false);
	    buttonDelete.setEnabled(false);

	    if (tileset.isSelectingNewTile()) {
		// a new tile is selected: if it is valid, we authorize the user to create it
		buttonCreate.setEnabled(!tileset.isNewTileAreaOverlapping());
	    }
	    else if (tileset.getSelectedTile() != null) {
		// an existing tile is selected, so the user can remove it
		buttonDelete.setEnabled(true);

		// make this tile selected in the list
		int listRank = tileset.tileIndexToTileRank(tilesetSelectedIndex);
		if (listRank != listSelectedRank) {
		    tileList.setSelectedIndex(listRank);
		    tileList.ensureIndexIsVisible(listRank);
		}
	    }
	    else {
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
	public Component getListCellRendererComponent(JList list, Object value, int index,
						      boolean isSelected, boolean cellHasFocus) {
	    return tileIcons.get(index);
	}
    }
}
