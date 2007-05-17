package tileset_editor;

import java.util.*;
import java.io.*;

/**
 * This class describes a tileset.
 */
public class Tileset extends Observable {

    /**
     * File of the tileset.
     */
    private File tilesetFile;

    /**
     * Name of the tileset.
     */
    private String name;

    /**
     * Number of tiles in the tileset.
     */
    private int nbTiles;

    /**
     * The tiles.
     */
    private TileImage[] tiles;

    /**
     * Creates a new tileset.
     * @param tilesetFile file of the tileset (should be a new file)
     * @param name name of the tileset, for example "house"
     */
    public Tileset(File tilesetFile, String name) throws IOException {
	super();

	this.tilesetFile = tilesetFile;
	this.name = name;

	// save the initial file
	save();
    }

    /**
     * Opens an existing tileset.
     * @param tilesetFile file of the tileset
     */
    public Tileset(File tilesetFile) throws IOException {
	super();

	this.tilesetFile = tilesetFile;

	// laod the tileset file
	load();
    }

    /**
     * Loads the data from the tileset file.
     */
    public void load() throws IOException {
	// open the tileset file
	BufferedReader reader = new BufferedReader(new FileReader(tilesetFile));

	// the first line is the tileset name
	this.name = reader.readLine();

	reader.close();
    }

    /**
     * Saves the data into the tileset file.
     */
    public void save() throws IOException {
	// open the tileset file
	PrintWriter writer = new PrintWriter(new BufferedWriter(new FileWriter(tilesetFile)));

	// the first line is the tileset name
	writer.println(name);

	writer.close();
    }
}
