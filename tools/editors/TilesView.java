package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

/**
 * This component shows a list of tiles and allows the user to select them.
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
     * Returns the component's size.
     */
//     public Dimension getPreferredSize() {
// 	return new Dimension(400, 600);
//     }

    /**
     * Sets the observed tileset.
     */
    public void setTileset(Tileset tileset) {
	this.tileset = tileset;

	tileset.addObserver(tileListModel);

	tileIcons.clear();
	for (int i = 0; i < tileset.getNbTiles(); i++) {
	    tileIcons.add(new TileIcon(tileset.getTile(i), tileset));
	}

	tileListModel.update(tileset, null);
    }

    /**
     * List selection listener associated to the tile table.
     * When the user selects a row, the corresponding tileset becomes selected in the image.
     */
    private class TileListSelectionListener implements ListSelectionListener {

	/**
	 * This function is called when the selection is changed.
	 */
	public void valueChanged(ListSelectionEvent e) {
	    tileset.setSelectedTileIndex(tileList.getSelectedIndex());
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
	public Object getElementAt(int index) {
	    Tile tile = null;

	    if (tileset != null) {
		tile = tileset.getTile(index);
	    }
	    
	    return tile;
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

	    // was a tile just removed?
	    if (params instanceof Integer) {
		int deletedTileIndex = ((Integer) params).intValue();
		tileIcons.remove(deletedTileIndex);		
	    }

	    // was a tile just created?
	    else if (params instanceof Tile) {
		Tile tileAdded = (Tile) params;
		tileIcons.add(new TileIcon(tileAdded, tileset));		
	    }

	    // update the enabled state of the buttons
	    int tilesetSelectedIndex = tileset.getSelectedTileIndex();
	    int listSelectedIndex = tileList.getSelectedIndex();

	    buttonCreate.setEnabled(false);
	    buttonDelete.setEnabled(false);

	    if (tilesetSelectedIndex == tileset.getNbTiles()) {
		// a new tile is selected: if it is valid, we authorize the user to create it
		buttonCreate.setEnabled(!tileset.isNewTileAreaOverlapping());
	    }
	    else if (tilesetSelectedIndex >= 0) {
		// an existing tile is selected, so the user can remove it
		buttonDelete.setEnabled(true);
		tileList.setSelectedIndex(tilesetSelectedIndex);
		tileList.ensureIndexIsVisible(tilesetSelectedIndex);
	    }
	    else {
		// no tile is selected
		if (listSelectedIndex != -1) {
		    tileList.removeSelectionInterval(listSelectedIndex, listSelectedIndex);
		}
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
