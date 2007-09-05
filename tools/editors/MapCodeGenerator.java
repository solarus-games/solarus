package editors;

import java.io.*;
import java.awt.*;

/**
 * This class provides functionnalities to generate
 * the C++ code corresponding to a map.
 */
public class MapCodeGenerator {

    /**
     * Symbolic constants for the layers in the generated code.
     */
    private static final String[] layerConstants = {
	"LAYER_LOW",
	"LAYER_INTERMEDIATE",
	"LAYER_HIGH"
    };

    private Map map;               // the map
    private String name;           // the map name
    private String upperCaseName;  // the upper case map name
    private String zsdxRootPath;   // the root directory of ZSDX
    private String headerFileName; // name of the header file (without the path)

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
	
	// generate the header file
	generateHeaderFile();

	// generate the source file
	generateSourceFile();

	// generate the creation of the entities
	generateEntityFile();

	// update include/MapList.inc.h
	updateEnumFile();

	// update src/MapsCreation.inc.cpp
	updateCreationFile();
    }

    /**
     * Generates the header file, for example include/maps/MapLinkHouse.h,
     * unless the file already exists.
     */
    private void generateHeaderFile() throws IOException {

	headerFileName = "Map" + name + ".h";
	String fileName = zsdxRootPath + File.separator + "include" +
	    File.separator + "maps" + File.separator + headerFileName;

	File file = new File(fileName);

	if (!file.exists()) {
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
	
	    out.println("#ifndef ZSDX_MAP_" + upperCaseName + "_H");
	    out.println("#define ZSDX_MAP_" + upperCaseName + "_H");
	    out.println();
	    out.println("#include \"Map.h\"");
	    out.println();
	    out.println("/*");
	    out.println(" * Map generated automatically by the tileset editor.");
	    out.println(" */");
	    out.println("class Map" + name + ": public Map {");
	    out.println();
	    out.println(" public:");
	    out.println("  Map" + name + "(void);");
	    out.println("  inline ~Map" + name + "(void) { }");
	    out.println();
	    out.println("  void load(void);");
	    out.println("};");
	    out.println();
	    out.println("#endif");
	    
	    out.close();
	}
    }

    /**
     * Generates the source file, for example src/maps/MapLinkHouse.cpp,
     * unless the file already exists.
     */
    private void generateSourceFile() throws IOException {

	String fileName = zsdxRootPath + File.separator + "src" +
	    File.separator + "maps" + File.separator + "Map" +
	    name + ".cpp";

	File file = new File(fileName);

	if (!file.exists()) {
	    // get some info about the map
	    Tileset tileset = map.getTileset();
	    String tilesetHeaderFileName = "Tileset" + tileset.getName() + ".h";
	    String tilesetConstant = "TILESET_" + tileset.getName().toUpperCase();
	    String musicConstant = "MUSIC_" + map.getMusic().toUpperCase();
	    String className = "Map" + map.getName();
	    Color bgColor = map.getBackgroundColor();
	    
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
	    
	    out.println("/*");
	    out.println(" * Map generated automatically by the map editor.");
	    out.println(" */");
	    out.println();
	    out.println("#include <SDL/SDL.h>");
	    out.println("#include \"Map.h\"");
	    out.println("#include \"GameResource.h\"");
	    out.println("#include \"Tile.h\"");
	    out.println("#include \"Color.h\"");
	    out.println("#include \"ZSDX.h\"");
	    out.println("#include \"tilesets/" + tilesetHeaderFileName + "\"");
	    out.println("#include \"maps/" + headerFileName + "\"");
	    out.println();
	    out.println("/**");
	    out.println(" * Constructor.");
	    out.println(" */");
	    out.println("" + className + "::" + className + "(void):");
	    out.println("  Map(" + map.getWidth() + ", " + map.getHeight() + ",");
	    out.println("  get_color(" + bgColor.getRed() + ", " + bgColor.getGreen() + ", " + bgColor.getBlue() + "),");
	    out.println("  ZSDX::game_resource->get_tileset(" + tilesetConstant + "),");
	    out.println("  ZSDX::game_resource->get_music(" + musicConstant + ")) {");
	    out.println("}");
	    out.println();
	    out.println("/**");
	    out.println(" * Loads the map.");
	    out.println(" */");
	    out.println("void " + className + "::load(void) {");
	    out.println("  Tile *tile;");
	    out.println("  SDL_Rect position_in_map;");
	    out.println();
	    out.println("  if (!tileset->is_loaded()) {");
	    out.println("    tileset->load();");
	    out.println("  }");
	    out.println();
	    out.println("  // link start position");
	    out.println("  link_start_x = 80;");
	    out.println("  link_start_y = 80;");
	    out.println();
	    out.println("#include \"../src/maps/" + className + "Entities.inc\"");
	    out.println("}");

	    out.close();
	}
    }

    /**
     * Generates the entity file, for example src/maps/MapLinkHouseEntities.cpp.
     * This file is included by the source file and it contains the creation of all entities of the map.
     */
    private void generateEntityFile() throws IOException {
	
	String fileName = zsdxRootPath + File.separator + "src" +
	    File.separator + "maps" + File.separator + "Map" +
	    name + "Entities.inc";
	
	PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
	
	for (int layer = 0; layer < Tile.LAYER_NB; layer++) {

	    for (TileOnMap tile: map.getTiles(layer)) {
		
		out.println("  tile = tileset->get_tile(" + tile.getTileId() + ");");
		out.println("  position_in_map.x = " + tile.getX() + ";");
		out.println("  position_in_map.y = " + tile.getY() + ";");
		out.println("  add_new_tile(tile, position_in_map, " + layerConstants[layer] + ", "
			    + tile.getRepeatX() + ", " + tile.getRepeatY() + ");");
		out.println();
	    }
	}
	out.close();
    }

    /**
     * Updates (if needed) the file include/MapList.inc.h.
     * This file contains the declaration of the tileset symbolic constants.
     */
    private void updateEnumFile() throws IOException {
	String fileName = zsdxRootPath + File.separator + "include" +
	    File.separator + "MapList.inc";
	
	// we have to put the following line into the file unless it is already present
	String lineWanted = "MAP_" + upperCaseName + ",";

	FileTools.ensureFileHasLine(fileName, lineWanted);
    }
    
    /** 
     * Updates (if needed) the file src/MapsCreation.inc.cpp.
     * This file contains the creation of each map object.
     */
    private void updateCreationFile() throws IOException {
	String fileName = zsdxRootPath + File.separator + "src" +
	    File.separator + "MapsCreation.inc.cpp";
	
	// we have to put the following line into the file unless it is already present
	String lineWanted = "maps[MAP_" + upperCaseName + "] = new Map" + name + "();";
	
	FileTools.ensureFileHasLine(fileName, lineWanted);
    }
}
