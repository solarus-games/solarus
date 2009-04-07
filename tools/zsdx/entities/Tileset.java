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
package zsdx.entities;

import java.util.*;
import java.io.*;
import java.awt.*;
import java.awt.image.*;
import javax.imageio.*;
import zsdx.*;

/**
 * This class describes a tileset.
 * A tileset is observable. When it changes, the observers are notified with
 * a parameter indicating what has just changed:
 *   - a TilePattern: indicates that this tile pattern was created
 *   - an Integer: indicates that the tile pattern with this id was removed
 *   - null: other cases
 */
public class Tileset extends Observable {

    // tileset data

    /**
     * Id of the tileset.
     */
    private String tilesetId;

    /**
     * Name of the tileset.
     */
    private String name;

    /**
     * Background color (default is black).
     */
    private Color backgroundColor;

    /**
     * The tile patterns.
     * The key of a tile pattern is its id in the tileset. The first id is 1.
     */
    private TreeMap<Integer,TilePattern> tilePatterns;

    /**
     * Maximum id of a tile in the map.
     */
    private int maxId;

    /**
     * The tileset image.
     */
    private BufferedImage image;

    /**
     * The scaled tileset images.
     */
    private BufferedImage[] scaledImages;

    // information about the user actions on the tileset

    /**
     * Tells whether the tileset has changed since the last save.
     * True if there has been no modifications, false otherwise.
     */
    private boolean isSaved; 

    /**
     * Id of the tile pattern currently selected by the user.
     * 0: no tile pattern is selected
     * 1 or more: an existing tile pattern is selected
     * -1: a new tile pattern is selected, ready to be created
     */
    private int selectedTilePatternId;

    /**
     * Position of the tile pattern the user is creating,
     * or null if there no new tile pattern selected.
     */
    private Rectangle newTilePatternArea;

    /**
     * True if the new tile pattern area is overlapping an existing tile pattern.
     * Is so, the tile pattern cannot be created.
     */
    private boolean isNewTilePatternAreaOverlapping;

    /**
     * Creates a new tileset.
     * @throws ZSDXException if the resource list could not be updated after the map creation
     */
    public Tileset() throws ZSDXException {
	super();

	this.name = "New tileset";
	this.backgroundColor = Color.BLACK;
	this.isSaved = false;
	this.maxId = 0;
	this.selectedTilePatternId = 0; // none
	this.tilePatterns = new TreeMap<Integer,TilePattern>();

	// compute an id and a name for this tileset
	this.name = "New tileset";
	Resource tilesetResource = Project.getResource(ResourceDatabase.RESOURCE_TILESET);
	this.tilesetId = tilesetResource.computeNewId();
	reloadImage();

	setSaved(true);

	setChanged();
	notifyObservers();
    }

    /**
     * Loads an existing tileset.
     * @param tilesetId id of the tileset to load
     * @throws ZSDXException if the tileset could not be loaded
     */
    public Tileset(String tilesetId) throws ZSDXException {
	this.selectedTilePatternId = 0; // none
	this.tilePatterns = new TreeMap<Integer,TilePattern>();
	this.tilesetId = tilesetId;
	load();
    }

    /**
     * Returns the id of the tileset.
     * @return the id of the tileset
     */
    public String getId() {
	return tilesetId;
    }

    /**
     * Returns the name of the tileset.
     * @return the name of the tileset, for example "Light World"
     */
    public String getName() {
	return name;
    }

    /**
     * Changes the name of the tileset.
     * @param name the name of the tileset, for example "Light World"
     */
    public void setName(String name) {

	if (!name.equals(this.name)) {
	    this.name = name;
	    setSaved(false);
	    setChanged();
	    notifyObservers();
	}
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
     * Reloads the tileset's image.
     * This function is called when ZSDX root path is changed.
     * The observers are notified with the new image as parameter.
     */
    public void reloadImage() {
	try {
	    scaledImages = new BufferedImage[4];

	    image = ImageIO.read(getImageFile());
	    int width = image.getWidth(null);
	    int height = image.getHeight(null);

	    scaledImages[0] = createScaledImage(image, width / 4, height / 4);

	    scaledImages[1] = createScaledImage(image, width / 2, height / 2);

	    scaledImages[2] = image;

	    scaledImages[3] = createScaledImage(image, width * 2, height * 2);
	}
	catch (IOException e) {
	    image = null;
	    scaledImages = null;
	}

	setChanged();
	notifyObservers(image);
    }

    private BufferedImage createScaledImage(BufferedImage image, int width, int height) {
        BufferedImage scaledImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = scaledImage.createGraphics();
        g.drawImage(image, 0, 0, width, height, null);
        g.dispose();
	return scaledImage;
    }

    
    /**
     * Returns the tileset's image file.
     * @return the image file of the tileset
     */
    public File getImageFile() {
	return Project.getTilesetImageFile(tilesetId);
    }

    /**
     * Returns the tileset's image, previously loaded by reloadImage().
     * @return the tileset's image
     */
    public BufferedImage getImage() {
// 	if (image == null) {
// 	    reloadImage();
// 	}
	return image;
    }

    /**
     * Returns a scaled version of the tileset image, previously loaded by reloadImage().
     * @param zoom an integer representing the scale (0: 25%, 1: 50%, 2: 100%, 3: 200%) 
     * @return the scaled tileset image
     */
    public BufferedImage getScaledImage(int zoom) {
// 	if (doubleImage == null) {
// 	    reloadImage();
// 	}
	return scaledImages[zoom];
    }

    /**
     * Returns a scaled version of the tileset image, previously loaded by reloadImage().
     * @param zoom the scale (0.25, 0.5, 1 or 2) 
     * @return the scaled tileset image
     */
    public BufferedImage getScaledImage(double zoom) {

	int index;
	if (zoom == 0.25) {
	    index = 0;
	}
	else if (zoom == 0.5) {
	    index = 1;
	}
	else if (zoom == 1.0) {
	    index = 2;
	}
	else {
	    index = 3;
	}

	return scaledImages[index];
    }

    /**
     * Returns the 200% scaled version of the tileset's image, previously loaded by reloadImage().
     * @return the tileset's image in 200%
     */
    public BufferedImage getDoubleImage() {
	return scaledImages[3];
    }

    /**
     * Returns the number of tile patterns in the tileset.
     * @return the number of tile patterns
     */
    public int getNbTilePatterns() {
	return tilePatterns.size();
    }

    /**
     * Returns the ids of the tile patterns.
     * @return the ids of the tile patterns
     */
    public Set<Integer> getTilePatternIds() {
	return tilePatterns.keySet();
    }

    /**
     * Returns the tile patterns of this tileset.
     * @return the tile patterns
     */
    public Collection<TilePattern> getTilePatterns() {
	return tilePatterns.values();
    }

    /**
     * Returns a tile pattern.
     * @param id id of the tile pattern to get
     * @return the tile pattern with this id
     * @throws NoSuchElementException if there is no tile pattern with this id
     */
    public TilePattern getTilePattern(int id) throws NoSuchElementException {

	TilePattern tilePattern = tilePatterns.get(id);

	if (tilePattern == null) {
	    throw new NoSuchElementException("There is no tile pattern with id #" + id + " in the tileset.");
	}

	return tilePattern;
    }

    /**
     * Returns the id of the tile pattern at a location in the tileset,
     * or 0 if there is no tile pattern there.
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @return id of the tile pattern at this point, or 0 if there is no tile pattern here
     */
    public int getTilePatternIdAt(int x, int y) {

	for (int id: getTilePatternIds()) {
	    Rectangle patternRectangle = getTilePattern(id).getPositionInTileset();
	    if (patternRectangle.contains(x, y)) {
		return id; // a tile pattern was found at this point
	    }
	}

	return 0; // no tile pattern found
    }

    /**
     * Returns the id of the selected tile pattern.
     * @return 0 if no tile pattern is selected, 1 or more if an existing tile pattern is selected,
     * or -1 if a new tile pattern is selected
     */
    public int getSelectedTilePatternId() {
	return selectedTilePatternId;
    }

    /**
     * Selects a tile patterns and notifies the observers.
     * @param selectedTilePatternId 0 to select no tile pattern,
     * 1 or more to select the existing
     * tile pattern with this id or -1 if a new tile pattern is selected
     */
    public void setSelectedTilePatternId(int selectedTilePatternId) {
	if (selectedTilePatternId != this.selectedTilePatternId) {
	    this.selectedTilePatternId = selectedTilePatternId;

	    if (selectedTilePatternId != getNbTilePatterns()) {
		newTilePatternArea = null;
	    }

	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects the current tile pattern.
     * This is equivalent to call setSelectedTileId(0).
     */
    public void unselectTilePattern() {
	setSelectedTilePatternId(0);
    }

    /**
     * Starts the selection of a new tile pattern.
     * This is equivalent to call setSelectedTilePatternId(-1).
     */
    public void startSelectingNewTilePattern() {
	setSelectedTilePatternId(-1);
    }

    /**
     * Returns the selected tile pattern.
     * @return the selected tile pattern, or null if there is no selected
     * tile pattern or if doesn't exist yet
     */
    public TilePattern getSelectedTilePattern() {
	if (selectedTilePatternId > 0) {
	    return getTilePattern(selectedTilePatternId);
	}
	else {
	    return null;
	}	
    }

    /**
     * Returns whether or not the user is selecting a new tile pattern.
     * @return true if the user is selecting a new tile pattern,
     * i.e. if getSelectedTilePatternId() == -1
     */
    public boolean isSelectingNewTilePattern() {
	return selectedTilePatternId == -1;
    }

    /**
     * Returns the rank of a tile pattern, knowing its id.
     * The rank is the position of the tile pattern if you consider all tile patterns sorted by their ids.
     * It is different from the id because all ids don't exist necessarily.
     * @param id id of the tile pattern in the tileset
     * @return rank of this tile pattern, in [0, getNbTilePatterns()[.
     */
    public int idToRank(int id) {
	
	// count the tiles until we find the right one
	int rank = 0;
	for (int idFound: getTilePatternIds()) {
	    
	    if (idFound == id) {
		return rank;
	    }
	    rank++;
	}

	throw new NoSuchElementException("There is no tile pattern at id " + id + " in the tileset.");
    }

    /**
     * Returns the id of a tile pattern knowing its rank.
     * The rank is the position of the tile pattern if you consider all tile patterns sorted by their ids.
     * It is different from the id because all ids don't exist necessarily.
     * @param rank rank of the tile pattern considered, in [0, getNbTilePatterns()[
     * @return the id of the tile pattern with this rank
     */
    public int rankToId(int rank) {
	
	// count rank tiles
	int i = 0;
	for (int currentId: getTilePatternIds()) {
	    
	    if (i == rank) {
		return currentId;
	    }
	    i++;
	}

	throw new NoSuchElementException("There is no tile pattern with rank " + rank + " in the tileset.");
    }

    /**
     * Returns the position of the tile pattern the user is creating.
     * @return position of the new tile pattern, or null if there no new tile pattern selected
     */
    public Rectangle getNewTilePatternArea() {
	return newTilePatternArea;
    }

    /**
     * Changes the position of the tile pattern the user is creating.
     * If the specified area is the same than before, nothing is done. 
     * @param newTileArea position of the new tile, or null if there is currently no new tile selected
     */
    public void setNewTilePatternArea(Rectangle newTilePatternArea) {
	if (!newTilePatternArea.equals(this.newTilePatternArea)) {

	    this.newTilePatternArea = newTilePatternArea;
	    
	    // determine whether or not the new tile pattern area is overlapping an existing tile pattern
	    isNewTilePatternAreaOverlapping = false;
	    for (TilePattern pattern: getTilePatterns()) {

		if (pattern.getPositionInTileset().intersects(newTilePatternArea)) {
		    isNewTilePatternAreaOverlapping = true;
		    break;
		}
	    }
	    
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns whether or not the area selected by the user to make a new tile pattern
     * is overlapping an existing tile pattern.
     * @return true if the new tile pattern area is overlapping an existing tile pattern, false otherwise
     */
    public boolean isNewTilePatternAreaOverlapping() {
	return isNewTilePatternAreaOverlapping;
    }

    /**
     * Creates the tile pattern specified by the current selection area
     * and adds it to the tileset.
     * The observers are notified with the created TilePattern as parameter.
     * @param obstacle obstacle property of the created tile pattern
     * @throws TilesetException if the tile size is incorrect
     */
    public void addTilePattern(Obstacle obstacle) throws TilesetException {

	if (isSelectingNewTilePattern() && !isNewTilePatternAreaOverlapping) {
	    TilePattern tilePattern = new TilePattern(newTilePatternArea, MapEntity.LAYER_LOW, obstacle);

	    maxId++;
	    tilePatterns.put(maxId, tilePattern);

	    setSelectedTilePatternId(maxId);
	    
	    isSaved = false;
	    
	    setChanged();
	    notifyObservers(tilePattern); // indicates that a tile pattern has been created
	}
    }

    /**
     * Adds a tile pattern to the tileset.
     * @param tilePatternId id of the tile pattern
     * @param tilePattern the tile pattern to add
     */
    private void addTilePattern(int tilePatternId, TilePattern tilePattern) {
	tilePatterns.put(tilePatternId, tilePattern);
    }

    /**
     * Removes the selected tile pattern.
     * The oberservers are notified with the removed tile pattern as parameter.
     */
    public void removeTilePattern() {
	Integer id = new Integer(getSelectedTilePatternId());

	if (id > 0) {
	    tilePatterns.remove(id);
	    setSelectedTilePatternId(0);

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
     * Loads the tileset from its file.
     * @throws ZSDXException if the file could not be read
     */
    public void load() throws ZSDXException {

	int lineNumber = 0;
	try {

	    // get the tileset name in the game resource database
	    Resource tilesetResource = Project.getResource(ResourceDatabase.RESOURCE_TILESET);
	    setName(tilesetResource.getElementName(tilesetId));

	    File tilesetFile = Project.getTilesetFile(tilesetId);
	    BufferedReader in = new BufferedReader(new FileReader(tilesetFile));

	    // read the tileset general info: "r g b"
	    String line = in.readLine();

	    if (line == null) {
		throw new ZSDXException("The tileset file is empty");
	    }

	    lineNumber++;
	    StringTokenizer tokenizer = new StringTokenizer(line);

	    int r = Integer.parseInt(tokenizer.nextToken());
	    int g = Integer.parseInt(tokenizer.nextToken());
	    int b = Integer.parseInt(tokenizer.nextToken());

	    setBackgroundColor(new Color(r, g, b));

	    // read the tiles of the tileset
	    line = in.readLine();
	    while (line != null) {
		lineNumber++;

		int tabIndex = line.indexOf('\t');
		int id = Integer.parseInt(line.substring(0, tabIndex));
		TilePattern tilePattern = new TilePattern(line.substring(tabIndex + 1));
		addTilePattern(id, tilePattern);

		if (id > maxId) {
		    maxId = id;
		}

		line = in.readLine();
	    }

	    in.close();

	    setSaved(true);
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": Integer expected");
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
     * Saves the tileset into its file.
     * @throws ZSDXException if the file could not be written
     */
    public void save() throws ZSDXException {

	try {

	    // open the tileset file
	    File tilesetFile = Project.getTilesetFile(tilesetId);
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(tilesetFile)));

	    // print the tileset general info: "r g b"
	    out.print(backgroundColor.getRed());
	    out.print('\t');
	    out.print(backgroundColor.getGreen());
	    out.print('\t');
	    out.print(backgroundColor.getBlue());
	    out.println();

	    // print the tile patterns
	    for (int id: getTilePatternIds()) {
		out.print(id);
		out.print('\t');
		out.print(getTilePattern(id).toString());
		out.println();
	    }

	    out.close();

	    setSaved(true);

	    // also update the tileset name in the global resource list
	    Resource tilesetResource = Project.getResource(ResourceDatabase.RESOURCE_TILESET);
	    tilesetResource.setElementName(tilesetId, name);
	    Project.getResourceDatabase().save();
	}
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}
    }
}
