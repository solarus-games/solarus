package editors;

import java.util.*;
import java.io.*;
import java.awt.*; // Dimension

/**
 * This class describes a map.
 * A map is observable.
 */
public class Map extends Observable {

    /**
     * Id of the map.
     */
    private String mapId;

    /**
     * Name of the map.
     */
    private String name;

    /**
     * Size of the map, in pixels (the width and the height
     * must be multiples of 8).
     */
    private Dimension size;

    /**
     * Id of the background music: name of a music file, Music.noneId or Music.unchangedId.
     */
    private String musicId;

    /**
     * Id of the tileset of the map (or an empty string if no tileset is set).
     */
    private String tilesetId;

    /**
     * Tileset of the map.
     * The tileset is the set of small images (tiles) used to build the map. 
     */
    private Tileset tileset;

    /**
     * Tiles of the map.
     * This is an array of three tile lists, one for each layer.
     */
    private TileOnMapList[] allTiles;

    /**
     * The tiles selected.
     */
    private MapTileSelection tileSelection;
    
    /**
     * True if some tiles could not be found in the tileset
     * when the tileset was loaded.
     */
    private boolean badTiles;

    /**
     * The history of the actions made by the user on the map.
     */
    private MapEditorHistory history;

    // types of entities
    private static final int ENTITY_TILE = 0;

    /**
     * Creates a new map.
     * @throws ZSDXException if the resource list could not be updated after the map creation
     */
    public Map() throws ZSDXException {
	super();

	this.size = new Dimension(320, 240);
	this.tileset = null;
	this.tilesetId = "";
	this.musicId = Music.noneId;

	initialize();
	
	// compute an id and a name for this map
	this.name = "New map";
	Resource mapResource = ResourceDatabase.getResource(ResourceDatabase.RESOURCE_MAP);
	this.mapId = mapResource.computeNewId();

	setChanged();
	notifyObservers();
    }

    /**
     * Loads an existing map.
     * @param mapId id of the map to load
     * @throws ZSDXException if the map could not be loaded
     */
    public Map(String mapId) throws ZSDXException {
	this.mapId = mapId;
	initialize();
	load();
    }
	
    /**
     * Initializes the object.
     */
    private void initialize() {
	this.allTiles = new TileOnMapList[3];
	for (int i = 0; i < MapEntity.LAYER_NB; i++) {
	    this.allTiles[i] = new TileOnMapList();
	}

	this.tileSelection = new MapTileSelection(this);
	this.history = new MapEditorHistory(this);
    }

    /**
     * Returns the id of the map.
     * @return the id of the map
     */
    public String getId() {
	return mapId;
    }

    /**
     * Returns the map name.
     * @return the name of the map, for example "Link's House"
     */
    public String getName() {
	return name;
    }

    /**
     * Changes the name of the map
     * @param name new name of the map
     */
    public void setName(String name) {

	if (!name.equals(this.name)) {
	    this.name = name;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the history of actions of the map.
     * @return the map history
     */
    public MapEditorHistory getHistory() {
	return history;
    }

    /**
     * Returns the map size.
     * @return the map size (in pixels)
     */
    public Dimension getSize() {
	return size;
    }

    /**
     * Returns the map width.
     * @return the map width (in pixels)
     */
    public int getWidth() {
	return size.width;
    }

    /**
     * Returns the map height.
     * @return the map height (in pixels)
     */
    public int getHeight() {
	return size.height;
    }

    /**
     * Changes the map size.
     * If the new size is lower than the old one, the tiles in the removed area
     * are not destroyed. The minimum size of a map is 320*240.
     * The width and the height must be multiples of 8.
     * @param size the new map size (in pixels)
     * @throws MapException if the width or the height is incorrect
     */
    public void setSize(Dimension size) throws MapException {
	if (size.width < 320 || size.height < 240) {
	    throw new MapException("The minimum size of a map is 320*240.");
	}
	
	if (size.width % 8 != 0 || size.height % 8 != 0) {
	    throw new MapException("The width and the height of the map must be multiples of 8.");
	}

	this.size = size;

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the tileset associated to this map.
     * The tileset is the set of small images (tiles) used to build the map. 
     * @return the tileset (null if no tileset is set)
     */
    public Tileset getTileset() {
	return tileset;
    }

    /**
     * Returns the id of the tileset associated to this map.
     * @return the tileset id (or an empty string if no tileset is set)
     */
    public String getTilesetId() {
	return tilesetId;
    }

    /**
     * Changes the tileset of the map.
     * @param tilesetId id of the new tileset, or an empty string to set no tileset
     * @return true if the tileset was loaded successfuly, false if some tiles could
     * not be loaded in this tileset
     * @throws MapException if this tileset could be applied
     */
    public boolean setTileset(String tilesetId) throws ZSDXException {

	this.badTiles = false;

	// if the tileset is removed
	if (tilesetId.length() == 0 && this.tilesetId.length() != 0) {

	    this.tilesetId = tilesetId;
	    this.tileset = null;

	    setChanged();
	    notifyObservers();
	}

	// if the tileset is changed
	else if (!tilesetId.equals(this.tilesetId)) {
	
	    this.tileset = new Tileset(tilesetId);

	    for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
		for (int i = 0; i < allTiles[layer].size(); i++) {

		    try {
			TileOnMap tile = allTiles[layer].get(i);
			tile.setTileset(tileset);
		    }
		    catch (TilesetException ex) {
			// the tile is not valid anymore, we should remove it from the map
			allTiles[layer].remove(i);
			i--;
			badTiles = true;
		    }
		}
	    }

	    this.tilesetId = tilesetId;

	    setChanged();
	    notifyObservers(tileset);
	}

	return !badTiles;
    }

    /**
     * Returns the total number of tiles of the map.
     * @return the total number of tiles of the map.
     */
    public int getNbTiles() {
	
	int nbTiles = 0;

	// add the tiles of each layer
	for (TileOnMapList tiles: allTiles) {
	    nbTiles += tiles.size();
	}

	return nbTiles;
    }

    /**
     * Returns the tiles of the map.
     * @return an array of 3 vectors of TileOnMap: a vector for each layer
     */
    public TileOnMapList[] getAllTiles() {
	return allTiles;
    }

    /**
     * Sets the tiles of the map.
     * @param allTiles an array of 3 vectors of TileOnMap: a vector for each layer (this array is copied)
     */
    public void setAllTiles(TileOnMapList[] allTiles) {

	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
	    this.allTiles[layer] = new TileOnMapList(allTiles[layer]);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the tiles of the map on a given layer.
     * @param layer the layer: MapEntity.LAYER_LOW, MapEntity.LAYER_INTERMEDIATE or MapEntity.LAYER_HIGH
     * @return the vector of TileOnMap for this layer
     */
    public TileOnMapList getTiles(int layer) {
	return allTiles[layer];
    }

    /**
     * Returns the first tile under a point of the map, in the three layers,
     * starting with the high layer.
     * @param x x of the point
     * @param y y of the point
     * @return the tile found, or null if there is no tile here
     */
    public TileOnMap getTileAt(int x, int y) {

	TileOnMap tile = getTileAt(MapEntity.LAYER_HIGH, x, y);

	if (tile == null) {
	    tile = getTileAt(MapEntity.LAYER_INTERMEDIATE, x, y);

	    if (tile == null) {
		tile = getTileAt(MapEntity.LAYER_LOW, x, y);
	    }
	}

	return tile;
    }

    /**
     * Returns the first tile under a point of the map, in the specified layer.
     * @param layer the layer
     * @param x x of the point
     * @param y y of the point
     * @return the tile found, or null if there is no tile here
     */
    public TileOnMap getTileAt(int layer, int x, int y) {
		
	TileOnMapList tileList = allTiles[layer];
	for (int i = tileList.size() - 1; i >= 0; i--) {

	    TileOnMap tile = tileList.get(i);
	    if (tile.containsPoint(x, y)) {
		return tile;
	    }
	}

	return null;
    }

    /**
     * Returns the tiles located in a rectangle defined by two points.
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     */
    public TileOnMapList getTilesInRectangle(int x1, int y1, int x2, int y2) {

	TileOnMapList tilesInRectangle = new TileOnMapList();

	int x = Math.min(x1, x2);
	int width = Math.abs(x2 - x1);

	int y = Math.min(y1, y2);
	int height = Math.abs(y2 - y1);

	Rectangle rectangle = new Rectangle(x, y, width, height);

	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {

	    TileOnMapList tiles = getTiles(layer);
	    
	    for (TileOnMap tile: tiles) {
		if (rectangle.contains(tile.getPositionInMap())) {
		    tilesInRectangle.add(tile);
		}
	    }
	}
	
	return tilesInRectangle;
    }

    /**
     * Adds a new tile on the map.
     * @param tile the tile to add
     */
    public void addTile(TileOnMap tile) {

	allTiles[tile.getLayer()].add(tile);

	setChanged();
	notifyObservers();
    }

    /**
     * Removes a tile from the map.
     * @param tile the tile to remove
     */
    public void removeTile(TileOnMap tile) {

	allTiles[tile.getLayer()].remove(tile);

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the position of a tile on the map, by specifying two points.
     * The tile is resized (i.e. repeatX and repeatY are updated) so that
     * the tile fits exactly in the rectangle formed by the two points.
     * @param tile a tile
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     * @throws MapException if the rectangle width or its height is zero
     * (no other verifications are performed)
     */
    public void setTilePosition(TileOnMap tile, int x1, int y1, int x2, int y2) throws MapException {
	tile.setPositionInMap(x1, y1, x2, y2);

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the position of a tile on the map, by specifying its rectangle.
     * The tile is resized (i.e. repeatX and repeatY are updated) so that
     * the tile fits exactly in the rectangle.
     * @param tile the tile
     * @param position a rectangle
     * @throws MapException if the rectangle width or its height is zero
     * (no other verifications are performed)
     */
    public void setTilePosition(TileOnMap tile, Rectangle position) throws MapException {
	tile.setPositionInMap(position);

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the position of a group of tiles.
     * @param tiles the tiles to move
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public void moveTiles(TileOnMapList tiles, int dx, int dy) {
			  
	for (TileOnMap tile: tiles) {
	    tile.move(dx, dy);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the id of the background music of the map.
     * @return the name of the music, i.e. a music file name with the extension,
     * or Music.noneId or Music.unchangedId
     */
    public String getMusic() {
	return musicId;
    }

    /**
     * Changes the default background music of the map.
     * @param music the name of the music, i.e. a music file name with the extension,
     * or Music.noneId or Music.unchangedId
     */
    public void setMusic(String musicId) {

	if (!musicId.equals(this.musicId)) {

	    this.musicId = musicId;

	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the tiles selected by the user.
     * @return the tiles selected by the user
     */
    public MapTileSelection getTileSelection() {
	return tileSelection;
    }
    
    /**
     * Changes the layer of a tile. You should call this method instead of
     * calling directly TileOnMap.setLayer(), because the tiles of the 3 layers are
     * stored in 3 different arrays.
     * @param tile the tile to move
     * @param layer the new layer
     */
    public void tileSetLayer(TileOnMap tile, int layer) {

	int oldLayer = tile.getLayer();

	if (layer != oldLayer) {
	    tile.setLayer(layer);
	    allTiles[oldLayer].remove(tile);
	    allTiles[layer].add(tile);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the specified tiles, sorted in the order of the map.
     * The first tile is the lowest one, the last tile is the highest one.
     * @param tiles the tiles to sort
     * @return the same tiles, sorted as they are in the map
     */
    private TileOnMapList getSortedTiles(TileOnMapList tiles) {
	
	TileOnMapList sortedTiles = new TileOnMapList();
	
	// sort the selected tiles so that they have the same order as in the map
	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {

	    for (TileOnMap tile: allTiles[layer]) {

		if (tiles.contains(tile)) {
		    sortedTiles.add(tile);
		}
	    }
	}

	// now sortedTiles contains all selected tiles, sorted in the same order as in the map
	return sortedTiles;
    }

    /**
     * Brings the specified tiles to the back, keeping their layer.
     * The order of the specified tiles in the map is unchanged.
     * @param tiles the tiles to move
     */
    public void bringToBack(TileOnMapList tiles) {

	TileOnMapList sortedTiles = getSortedTiles(tiles);

	// bring to back each tile from sortedTiles
	for (int i = sortedTiles.size() - 1; i >= 0; i--) {

	    TileOnMap tile = sortedTiles.get(i);
	    int layer = tile.getLayer();
	    allTiles[layer].remove(tile);
	    allTiles[layer].addFirst(tile);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Brings the specified tiles to the front, keeping their layer.
     * The order of the specified tiles in the map is unchanged.
     * @param tiles the tiles to move
     */
    public void bringToFront(TileOnMapList tiles) {

	TileOnMapList sortedTiles = getSortedTiles(tiles);

	// bring to front each tile from sortedTiles
	for (TileOnMap tile: sortedTiles) {

	    int layer = tile.getLayer();
	    allTiles[layer].remove(tile);
	    allTiles[layer].addLast(tile);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether or not when the tileset was loaded, some tiles of the map refered
     * to non existent tiles in the tileset. These bad tiles were removed.
     * @return true if there was bad tiles when the tiles was loaded, false if all tiles
     * were loaded successfuly.
     */
    public boolean badTiles() {
	return badTiles;
    }

    /**
     * Loads the map from its file.
     * @throws ZSDXException if the file could not be read
     */
    public void load() throws ZSDXException {
	
	int lineNumber = 0;
	try {

	    // get the map name in the game resource database
	    Resource mapResource = ResourceDatabase.getResource(ResourceDatabase.RESOURCE_MAP);
	    setName(mapResource.getElementName(mapId));
	    
	    File mapFile = Configuration.getInstance().getMapFile(mapId);
	    BufferedReader in = new BufferedReader(new FileReader(mapFile));

	    // read the map general info
	    String line = in.readLine();

	    if (line == null) {
		throw new ZSDXException("The map file is empty");
	    }

	    lineNumber++;
	    StringTokenizer tokenizer = new StringTokenizer(line);

	    int width = Integer.parseInt(tokenizer.nextToken());
	    int height = Integer.parseInt(tokenizer.nextToken());

	    setSize(new Dimension(width, height));
	    setTileset(tokenizer.nextToken());
	    setMusic(tokenizer.nextToken());

	    // read the map entities
	    line = in.readLine();
	    while (line != null) {
		lineNumber++;

		int tabIndex = line.indexOf('\t');
		int entityType = Integer.parseInt(line.substring(0, tabIndex));

		if (entityType == ENTITY_TILE) {
		    TileOnMap tileOnMap = new TileOnMap(line.substring(tabIndex + 1), tileset);
		    addTile(tileOnMap);
		}
		else {
		    throw new ZSDXException("Unknown entity type '" + entityType + "'");
		}

		line = in.readLine();
	    }

	    in.close();

	    history.setSaved();
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": Integer expected");
	}
	catch (IndexOutOfBoundsException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": Value expected");
	}
	catch (ZSDXException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": " + ex.getMessage());
	}
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Saves the map into its file.
     * @throws ZSDXException if the file could not be written
     */
    public void save() throws ZSDXException {

	// check that a tileset is selected
	if (tilesetId.length() == 0) {
	    throw new ZSDXException("No tileset is selected");
	}

	try {
	    
	    // open the map file
	    File mapFile = Configuration.getInstance().getMapFile(mapId);
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(mapFile)));
	    
	    // print the map general info
	    out.print(size.width);
	    out.print('\t');
	    out.print(size.height);
	    out.print('\t');
	    out.print(tilesetId);
	    out.print('\t');
	    out.print(musicId);
	    out.println();
	    
	    // print the tiles
	    for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {

		for (TileOnMap tile: getTiles(layer)) {
		    out.print(ENTITY_TILE);
		    out.print('\t');
		    out.print(tile.toString());
		    out.println();
		}
	    }

	    out.close();

	    history.setSaved();

	    // also update the map name in the global resource list
	    Resource mapResource = ResourceDatabase.getResource(ResourceDatabase.RESOURCE_MAP);
	    mapResource.setElementName(mapId, name);
	    ResourceDatabase.save();
	}
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}
    }
}
