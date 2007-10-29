package editors;

import java.util.*;
import java.io.*;
import java.awt.*;
import java.awt.image.*;
import javax.imageio.*;

/**
 * This class describes a tileset.
 * A tileset is observable. When it changes, the observers are notified with
 * a parameter indicating what has just changed:
 *   - a Tile: indicates that this tile was created
 *   - an Integer: indicates that the tile with this id was removed
 *   - null: other cases
 */
public class Tileset extends Observable implements Serializable, ImageObserver {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 5L;

    // tileset data 

    /**
     * Name of the tileset.
     */
    private String name;

    /**
     * Background color (default is black).
     */
    private Color backgroundColor;

    /**
     * The tiles.
     * The key of a tile is its id in the tileset. The first id is 1.
     */
    private TreeMap<Integer,Tile> tiles;

    /**
     * Maximum id of a tile in the hashtable.
     */
    private int maxId;

    /**
     * The tileset image.
     */
    private transient Image image;
    
    /**
     * The tileset image scaled by 2.
     */
    private transient Image doubleImage;
    
    // information about the user actions on the tileset

    /**
     * Tells whether the tileset has changed since the last save.
     * True if there has been no modifications, false otherwise.
     */
    private transient boolean isSaved; 

    /**
     * Id of the tile currently selected by the user.
     * 0: no tile is selected
     * 1 or more: an existing tile is selected
     * -1: a new tile is selected, ready to be created
     */
    private transient int selectedTileId;

    /**
     * Position of the tile the user is creating,
     * or null if there no new tile selected.
     */
    private transient Rectangle newTileArea;

    /**
     * True if the new tile area is overlapping a tile.
     * Is so, the tile cannot be created.
     */
    private transient boolean isNewTileAreaOverlapping;

    /**
     * Creates a new tileset.
     * @param name name of the tileset to create
     */
    public Tileset(String name) {
	super();
	this.name = name;
	this.backgroundColor = Color.BLACK;
	this.isSaved = false;
	this.selectedTileId = 0; // none
	this.maxId = 0;
	tiles = new TreeMap<Integer,Tile>();
	reloadImage();
    }

    /**
     * Returns the name of the tileset.
     * @return the name of the tileset, for example "house"
     */
    public String getName() {
	return name;
    }

    /**
     * Returns the background color of the tileset.
     * @return the background color
     */
    public Color getBackgroundColor() {
	return backgroundColor;
    }

    /**
     * Changes the background color of the tileset.
     * @param backgroundColor the new background color
     */
    public void setBackgroundColor(Color backgroundColor) {
	this.backgroundColor = backgroundColor;
	setSaved(false);
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the path of the file containing the tileset's image.
     * @return the image file path
     */
    public String getImagePath() {
	return Configuration.getInstance().getZsdxRootPath() + File.separator + "images" +
	    File.separator + "tilesets" + File.separator + name + ".png";
    }

    /**
     * Reloads the tileset's image.
     * This function is called when ZSDX root path is changed.
     * The observers are notified with the new image as parameter.
     */
    public void reloadImage() {
	try {
	    image = ImageIO.read(new File(getImagePath()));
	    doubleImage = image.getScaledInstance(image.getWidth(this) * 2,
						  image.getHeight(this) * 2,
						  Image.SCALE_FAST);
	}
	catch (IOException e) {
	    image = null;
	    doubleImage = null;
	}

	setChanged();
	notifyObservers(image);
    }

    /**
     * Returns the tileset's image, previously loaded by reloadImage().
     * @return the tileset's image
     */
    public Image getImage() {
// 	if (image == null) {
// 	    reloadImage();
// 	}
	return image;
    }

    /**
     * Returns the 200% scaled version of the tileset's image, previously loaded by reloadImage().
     * @return the tileset's image in 200%
     */
    public Image getDoubleImage() {
// 	if (doubleImage == null) {
// 	    reloadImage();
// 	}
	return doubleImage;
    }

    /**
     * This function is called when some requested information about the image comes.
     * @return true
     */
    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height) {
	return true;
    }

    /**
     * Returns the number of tiles in the tileset.
     * @return the number of tiles
     */
    public int getNbTiles() {
	return tiles.size();
    }

    /**
     * Returns the ids of the tiles
     * @return the ids of the tiles
     */
    public Set<Integer> getTileIds() {
	return tiles.keySet();
    }

    /**
     * Returns the tiles of this tileset.
     * @return the tiles
     */
    public Collection<Tile> getTiles() {
	return tiles.values();
    }

    /**
     * Returns a tile.
     * @param id id of the tile to get
     * @return the tile with this id
     * @throws NoSuchElementException if there is no tile with this id
     */
    public Tile getTile(int id) throws NoSuchElementException {

	Tile tile = tiles.get(id);

	if (tile == null) {
	    throw new NoSuchElementException("There is no tile with id " + id + " in the tileset.");
	}

	return tile;
    }

    /**
     * Returns the id of the tile at a location in the tileset,
     * or 0 if there is no tile there.
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @return id of the tile at this point, or 0 if there is no tile here
     */
    public int getIdOfTileAt(int x, int y) {

	for (int id: getTileIds()) {
	    Rectangle tileRectangle = getTile(id).getPositionInTileset();
	    if (tileRectangle.contains(x, y)) {
		return id; // a tile was found at this point
	    }
	}

	return 0; // no tile found
    }

    /**
     * Returns the id of the selected tile.
     * @return 0 if no tile is selected, 1 or more if an existing tile is selected,
     * or -1 if a new tile is selected
     */
    public int getSelectedTileId() {
	return selectedTileId;
    }

    /**
     * Selects a tile and notifies the observers.
     * @param selectedTileId 0 to select no tile, 1 or more to select the existing
     * tile with this id or -1 if a new tile is selected
     */
    public void setSelectedTileId(int selectedTileId) {
	if (selectedTileId != this.selectedTileId) {
	    this.selectedTileId = selectedTileId;

	    if (selectedTileId != getNbTiles()) {
		newTileArea = null;
	    }

	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects the current tile.
     * This is equivalent to call setSelectedTileId(0).
     */
    public void unSelectTile() {
	setSelectedTileId(0);
    }

    /**
     * Starts the selection of a new tile.
     * This is equivalent to call setSelectedTileId(-1).
     */
    public void startSelectingNewTile() {
	setSelectedTileId(-1);
    }

    /**
     * Returns the selected tile.
     * @return the selected tile, or null if there is no selected tile or if doesn't exist yet
     */
    public Tile getSelectedTile() {
	if (selectedTileId > 0) {
	    return getTile(selectedTileId);
	}
	else {
	    return null;
	}	
    }

    /**
     * Returns whether or not the user is selecting a new tile.
     * @return true if the user is selecting a new tile, i.e. if getSelectedTileId() == -1
     */
    public boolean isSelectingNewTile() {
	return selectedTileId == -1;
    }

    /**
     * Returns the rank of a tile, knowing its id.
     * The rank is the position of the tile if you consider all tiles sorted by their ids.
     * It is different from the id because all ids don't exist necessarily.
     * @param id id of the tile in the tileset
     * @return rank of this tile, in [0, getNbTiles()[.
     */
    public int tileIdToTileRank(int id) {
	
	// count the tiles until we find the right one
	int rank = 0;
	for (int idFound: getTileIds()) {
	    
	    if (idFound == id) {
		return rank;
	    }
	    rank++;
	}

	throw new NoSuchElementException("There is no tile at id " + id + " in the tileset.");
    }

    /**
     * Returns the id of a tile knowing its rank.
     * The rank is the position of the tile if you consider all tiles sorted by their ids.
     * It is different from the id because all ids don't exist necessarily.
     * @param rank rank of the tile considered, in [0, getNbTiles()[
     * @return the id of the tile with this rank
     */
    public int tileRankToTileId(int rank) {
	
	// count rank tiles
	int i = 0;
	for (int currentId: getTileIds()) {
	    
	    if (i == rank) {
		return currentId;
	    }
	    i++;
	}

	throw new NoSuchElementException("There is no tile with rank " + rank + " in the tileset.");
    }

    /**
     * Returns the position of the tile the user is creating,
     * @return position of the new tile, or null if there no new tile selected
     */
    public Rectangle getNewTileArea() {
	return newTileArea;
    }

    /**
     * Changes the position of the tile the user is creating.
     * If the specified area is the same than before, nothing is done. 
     * @param newTileArea position of the new tile, or null if there is currently no new tile selected
     */
    public void setNewTileArea(Rectangle newTileArea) {
	if (!newTileArea.equals(this.newTileArea)) {

	    this.newTileArea = newTileArea;
	    
	    // determine whether or not the new tile area is overlapping an existing tile
	    isNewTileAreaOverlapping = false;
	    for (Tile tile: getTiles()) {

		if (tile.getPositionInTileset().intersects(newTileArea)) {
		    isNewTileAreaOverlapping = true;
		    break;
		}
	    }
	    
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns whether or not the area selected by the user to make a new tile
     * is overlapping an existing tile.
     * @return true if the new tile area is overlapping an existing tile, false otherwise
     */
    public boolean isNewTileAreaOverlapping() {
	return isNewTileAreaOverlapping;
    }

    /**
     * Creates the tile specified by the current selection area and adds it to the tileset.
     * The observers are notified with the created Tile as parameter.
     * @param obstacle type of obstacle for the created tile
     * @throws TilesetException if the tile size is incorrect
     */
    public void addTile(int obstacle) throws TilesetException {
	Tile tile = null;

	if (isSelectingNewTile() && !isNewTileAreaOverlapping) {
	    tile = new Tile(newTileArea, Tile.LAYER_LOW, obstacle);
	    
	    maxId++;
	    tiles.put(maxId, tile);

	    setSelectedTileId(maxId);
	    
	    isSaved = false;
	    
	    setChanged();
	    notifyObservers(tile); // indicates that a tile has been created
	}
    }

    /**
     * Removes the selected tile.
     * The oberservers are notified with the removed tile as parameter.
     */
    public void removeTile() {
	Integer id = new Integer(getSelectedTileId());

	if (id > 0) {
	    tiles.remove(id);
	    setSelectedTileId(0);

	    isSaved = false;

	    setChanged();
	    notifyObservers(id); // indicate that the tile has been removed
	}
    }

    /**
     * Returns whether or not the tileset has changed since the last save.
     * @return true if there has been no modifications, false otherwise
     */
    public boolean isSaved() {
	return isSaved;
    }

    /**
     * Sets whether the tileset has changed since the last save.
     * @param isSaved true if there has been no modifications, false otherwise
     */
    public void setSaved(boolean isSaved) {
	this.isSaved = isSaved;
    }
    
    /**
     * Compares this tileset to another one.
     * @param other another tileset
     * @return true if the tilesets have the same name
     */
    public boolean equals(Object other) {
	return other != null && ((Tileset) other).getName() == this.getName();
    }

    /**
     * Loads a tileset from the tileset file.
     */
    public static Tileset load(File tilesetFile) throws IOException {

 	// open the tileset file
	ObjectInputStream in = new ObjectInputStream(new FileInputStream(tilesetFile));
	Tileset tileset = null;

	// read the object
	try {
	    tileset = (Tileset) in.readObject();
	}
	catch (ClassNotFoundException e) {
	    System.err.println("Unable to read the object: " + e.getMessage());
	    e.printStackTrace();
	    System.exit(1);
	}

	in.close();

	tileset.setSaved(true);
	tileset.setSelectedTileId(0); // none
	tileset.reloadImage();

	return tileset;
    }

    /**
     * Saves the data into the tileset file.
     */
    public static void save(File tilesetFile, Tileset tileset) throws IOException {

 	// open the tileset file
	ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream(tilesetFile));

	// write the object
	out.writeObject(tileset);
	out.close();

	tileset.setSaved(true);
    }

}
