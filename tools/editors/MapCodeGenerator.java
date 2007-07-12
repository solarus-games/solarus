package editors;

import java.io.*;
import java.awt.*;

/**
 * This class provides functionnalities to generate
 * the C++ code corresponding to a map.
 */
public class MapCodeGenerator {

    private Map map;               // the map
    private String name;           // the map name
    private String upperCaseName;  // the upper case map name
    private String zsdxRootPath;   // the root directory of ZSDX

    /**
     * Constructor.
     */
    public MapCodeGenerator() {

    }

    /**
     * Generates the code corresponding to a map.
     * @param map the map
     */
    public void generate(Map map) throws IOException {
	this.map = map;
	zsdxRootPath = Configuration.getInstance().getZsdxRootPath();
	name = map.getName();
	upperCaseName = name.toUpperCase();
    }

    /**
     * Opens a file and looks for a specified line.
     * The line is added to the file if it was not there.
     */
    private void ensureLineIsInFile(String fileName, String lineWanted) throws IOException {

	// read the file to determine whether or not the line is already there
	BufferedReader in = new BufferedReader(new FileReader(fileName));
	String line;
	boolean found = false;
	
	line = in.readLine();
	while (line != null && !found) {
	    found = line.equals(lineWanted);
	    line = in.readLine();
	}
	in.close();

	if (!found) {
	    // the line has not been found: let's add it to the file
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName, true)));
	    out.println(lineWanted);
	    out.close();
	}

    }
}
